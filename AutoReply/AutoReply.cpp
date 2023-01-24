#include "pch.h"
#include "AutoReply.h"
#define WINVER 0x0500
#include <windows.h>


BAKKESMOD_PLUGIN(AutoReply, "Reply automatically", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void AutoReply::pressKey(char key)
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

	cvarManager->registerCvar("AutoReplyEnabled", "1", "Whether AutoReply is enabled")
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar)
		{
			cVarEnabledChanged();
		});
	cvarManager->registerCvar("goalCompRepliesEnabled", "1", "Determines whether to reply to goal compliments");
	cvarManager->registerCvar("assistCompRepliesEnabled", "1", "Determines whether to reply to assist compliments");
	cvarManager->registerCvar("apologyRepliesEnabled", "1", "Determines whether to reply to apologies");
	cvarManager->registerCvar("shotCompEnabled", "0", "Determines whether to compliment teammate's shots");

	responded = false;
	hookAll();

}

void AutoReply::cVarEnabledChanged()
{
	bool enabled = cvarManager->getCvar("AutoReplyEnabled").getBoolValue();
	if (enabled && !hooked)
	{
		hookAll();
	}
	else if (!enabled && hooked)
	{
		unhookAll();
	}
}

void AutoReply::hookAll()
{
	gameWrapper->HookEventWithCallerPost<ActorWrapper>("Function TAGame.PlayerController_TA.ChatMessage_TA",
		[this](ActorWrapper caller, void* params, std::string eventname)
		{
			onChatMessage(params);
		});
	gameWrapper->HookEventWithCallerPost<ServerWrapper>("Function TAGame.GFxHUD_TA.HandleStatTickerMessage",
		[this](ServerWrapper caller, void* params, std::string eventname)
		{
			onStatEvent(params);
		});
	hooked = true;
}

void AutoReply::unhookAll()
{
	gameWrapper->UnhookEvent("Function TAGame.PlayerController_TA.ChatMessage_TA");
	gameWrapper->UnhookEvent("Function TAGame.GFxHUD_TA.HandleStatTickerMessage");
	hooked = false;
}

void AutoReply::sendChat(char input1, char input2, float delay)
{
	gameWrapper->SetTimeout([=](GameWrapper* gw)
	{
		pressKey(input1);
		pressKey(input2);
	}, delay);

}

bool AutoReply::withinDuration(TimePoint timestamp, int seconds, bool lessThan = true)
{
	auto curTimeStamp = std::chrono::system_clock::now();
	std::chrono::duration<double> dur = curTimeStamp - timestamp;
	LOG(std::to_string(dur.count()));
	return lessThan ? dur.count() < seconds : dur.count() > seconds;
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
	ChatMessageParams* chatParams = (ChatMessageParams*)params;
	std::string msg = UnrealStringWrapper((uintptr_t)chatParams->Message).ToString();
	PriWrapper chatterPRI((uintptr_t)chatParams->InPRI);

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
	if (cvarManager->getCvar("goalCompRepliesEnabled")
		&& (msg == "Group2Message5" || msg == "Group2Message1" || msg == "Group5Message2"))
	{
		lastGoalComp = std::chrono::system_clock::now();
		if (!responded && withinDuration(lastGoal, 15))
		{
			responded = true;
			sendChat('2', '3', 1);
		}
	}
	else if (cvarManager->getCvar("assistCompRepliesEnabled")
		&& msg == "Group2Message2" || msg == "Group2Message5" || msg == "Group5Message2")
	{
		lastAssistComp = std::chrono::system_clock::now();
		if (!responded && withinDuration(lastAssist, 15))
		{
			responded = true;
			sendChat('2', '1', 1);
		}
	}
	else if (cvarManager->getCvar("apologyRepliesEnabled")
		&& msg == "Group4Message5" || msg == "Group4Message7" || msg == "Group4Message6" || msg == "Group4Message4" || msg == "Group4Message3")
	{
		if (withinDuration(lastApologyReply, 15, false))
		{
			lastApologyReply = std::chrono::system_clock::now();
			sendChat('4', '2', 2);
		}
	}
}

void AutoReply::onStatEvent(void* params)
{
	struct StatTickerParams 
	{
		uintptr_t Receiver;
		uintptr_t Victim;
		uintptr_t StatEvent;
	};
	StatTickerParams* pStruct = (StatTickerParams*)params;
	StatEventWrapper statEvent = StatEventWrapper(pStruct->StatEvent);
	if (statEvent.GetEventName() == "Goal")
	{
		lastGoal = std::chrono::system_clock::now();
		responded = false;
		if (withinDuration(lastGoalComp, 5))
		{
			responded = true;
			sendChat('2', '3', 1);
		}
	}
	else if (statEvent.GetEventName() == "Assist")
	{
		lastAssist = std::chrono::system_clock::now();
		responded = false;
		if (withinDuration(lastAssistComp, 5))
		{
			responded = true;
			sendChat('2', '1', 1);
		}
	}
}

void AutoReply::onUnload()
{
}