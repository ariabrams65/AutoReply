#include "pch.h"
#include "AutoReply.h"
#define WINVER 0x0500
#include <windows.h>


BAKKESMOD_PLUGIN(AutoReply, "Reply automatically", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void pressKey(char key)
{
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;
	ip.ki.wVk = key;
	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));

	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));
}

void AutoReply::onLoad()
{
	_globalCvarManager = cvarManager;
	LOG("AutoTHanks loaded");
	responded = false;

	gameWrapper->HookEventWithCallerPost<ActorWrapper>("Function TAGame.PlayerController_TA.ChatMessage_TA",
		[this](ActorWrapper caller, void* params, std::string eventname)
		{
			onChatMessage(params);
		});
	gameWrapper->HookEventWithCallerPost<ServerWrapper>("Function TAGame.GFxHUD_TA.HandleStatEvent",
		[this](ServerWrapper caller, void* params, std::string eventname)
		{
			onStatEvent(params);
		});
}

void AutoReply::sendChat(char input1, char input2, float delay)
{
	gameWrapper->SetTimeout([=](GameWrapper* gw)
	{
		pressKey(input1);
		pressKey(input2);
	}, delay);

}

void AutoReply::evalDuration(char input1, char input2, TimePoint timestamp, int seconds)
{
	auto curTimeStamp = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsedSeconds = curTimeStamp - timestamp;
	LOG(std::to_string(elapsedSeconds.count()));
	if (elapsedSeconds.count() < seconds)
	{
		LOG("should say something!");
		responded = true;
		sendChat(input1, input2, 1);
	}
}

void AutoReply::onChatMessage(void* params)
{
	struct ChatMessageParams
	{
		void* InPRI;
		uint8_t Message[0x10];
		uint8_t ChatChannel;
		bool bPreset;
	};
	if (!params) return;
	ChatMessageParams* chatParams = reinterpret_cast<ChatMessageParams*>(params);
	std::string msg = UnrealStringWrapper(reinterpret_cast<uintptr_t>(chatParams->Message)).ToString();
	PriWrapper chatterPRI(reinterpret_cast<uintptr_t>(chatParams->InPRI));

	PlayerControllerWrapper playerController = gameWrapper->GetPlayerController();
	if (!playerController) return;
	PriWrapper primaryPRI = playerController.GetPRI();
	if (!primaryPRI) return;

	if (primaryPRI.memory_address == chatterPRI.memory_address) return;
	if (primaryPRI.GetTeam().GetTeamNum() != chatterPRI.GetTeam().GetTeamNum()) return;

	handleMessage(msg);
}

void AutoReply::handleMessage(const std::string& msg)
{
	if (msg == "Group2Message5" || msg == "Group2Message1" || msg == "Group5Message2")
	{
		lastGoalComp = std::chrono::system_clock::now();
		if (!responded) evalDuration('2', '3', lastGoal, 15);
	}
	else if (msg == "Group2Message2" || msg == "Group2Message5" || msg == "Group5Message2")
	{
		lastAssistComp = std::chrono::system_clock::now();
		if (!responded) evalDuration('2', '1', lastAssist, 15);
	}
	else if (msg == "Group4Message5" || msg == "Group4Message7" || msg == "Group4Message6" || msg == "Group4Message4" || msg == "Group4Message3")
	{
		auto curTimeStamp = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsedSeconds = curTimeStamp - lastApology;
		if (elapsedSeconds.count() > 15)
		{
			lastApology = std::chrono::system_clock::now();
			sendChat('4', '2', 2);
		}
	}
}

void AutoReply::onStatEvent(void* params)
{
	struct StatEventParams
	{
		uintptr_t PRI;
		uintptr_t StatEvent;
	};
	StatEventParams* pStruct = (StatEventParams*)params;
	StatEventWrapper statEvent = StatEventWrapper(pStruct->StatEvent);
	if (statEvent.GetEventName() == "Goal")
	{
		lastGoal = std::chrono::system_clock::now();
		responded = false;
		evalDuration('2', '3', lastGoalComp, 5);
	}
	else if (statEvent.GetEventName() == "Assist")
	{
		lastAssist = std::chrono::system_clock::now();
		responded = false;
		evalDuration('2', '1', lastAssistComp, 5);
	}
}

void AutoReply::onUnload()
{
}