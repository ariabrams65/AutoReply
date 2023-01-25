#include "pch.h"
#include "AutoReply.h"
#include <windows.h>

#define COMPLIMENT_REPLY_DELAY 2
#define APOLOGY_REPLY_DELAY 2
#define COMPLIMENT_DELAY 3

BAKKESMOD_PLUGIN(AutoReply, "Reply automatically", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void AutoReply::onLoad()
{
	_globalCvarManager = cvarManager;

	goalComplimentRepliesEnabled = std::make_shared<bool>(true);
	assistComplimentRepliesEnabled = std::make_shared<bool>(true);
	apologyRepliesEnabled = std::make_shared<bool>(true);
	goalComplimentEnabled = std::make_shared<bool>(false);

	cvarManager->registerCvar("AutoReplyEnabled", "1", "Whether AutoReply is enabled")
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar)
		{
			cVarEnabledChanged();
		});
	cvarManager->registerCvar("goalComplimentRepliesEnabled", "1")
		.bindTo(goalComplimentRepliesEnabled);
	cvarManager->registerCvar("assistComplimentRepliesEnabled", "1")
		.bindTo(assistComplimentRepliesEnabled);
	cvarManager->registerCvar("apologyRepliesEnabled", "1")
		.bindTo(apologyRepliesEnabled);
	cvarManager->registerCvar("goalComplimentEnabled", "0")
		.bindTo(goalComplimentEnabled);

	respondedToCompliment = false;
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
	gameWrapper->UnhookEventPost("Function TAGame.PlayerController_TA.ChatMessage_TA");
	gameWrapper->UnhookEventPost("Function TAGame.GFxHUD_TA.HandleStatTickerMessage");
	hooked = false;
}

void AutoReply::sendChat(char input1, char input2, float delay)
{
	gameWrapper->SetTimeout([=](GameWrapper* gw)
	{
		INPUT inputs[4];
		ZeroMemory(inputs, sizeof(inputs));
		auto inputLocale = GetKeyboardLayout(0);
		auto input1VirtualKey = VkKeyScanExA(input1, inputLocale);
		auto input2VirtualKey = VkKeyScanExA(input2, inputLocale);

		inputs[0].type = INPUT_KEYBOARD;
		inputs[0].ki.wVk = input1VirtualKey;

		inputs[1].type = INPUT_KEYBOARD;
		inputs[1].ki.wVk = input1VirtualKey;
		inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

		inputs[2].type = INPUT_KEYBOARD;
		inputs[2].ki.wVk = input2VirtualKey;

		inputs[3].type = INPUT_KEYBOARD;
		inputs[3].ki.wVk = input2VirtualKey;
		inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

		SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
	}, delay);
}

bool AutoReply::isWithinGivenSeconds(TimePoint timestamp, int seconds)
{
	auto curTimeStamp = std::chrono::system_clock::now();
	std::chrono::duration<double> dur = curTimeStamp - timestamp;
	return dur.count() < seconds;
}

bool AutoReply::givenSecondsHavePassed(TimePoint timestamp, int seconds)
{
	auto curTimeStamp = std::chrono::system_clock::now();
	std::chrono::duration<double> dur = curTimeStamp - timestamp;
	return dur.count() > seconds;
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
	//teammate compliments goal;
	if (msg == "Group2Message5" || msg == "Group2Message1" || msg == "Group5Message2")
	{
		lastGoalCompliment = std::chrono::system_clock::now();
		if (*goalComplimentRepliesEnabled && !respondedToCompliment && isWithinGivenSeconds(lastGoal, 15))
		{
			respondedToCompliment = true;
			sendChat('2', '3', COMPLIMENT_REPLY_DELAY);
		}
	}
	//teammate compliments assist
	else if (msg == "Group2Message2" || msg == "Group2Message5" || msg == "Group5Message2")
	{
		lastAssistCompliment = std::chrono::system_clock::now();
		if (*assistComplimentRepliesEnabled && !respondedToCompliment && isWithinGivenSeconds(lastAssist, 15))
		{
			respondedToCompliment = true;
			sendChat('2', '3', COMPLIMENT_REPLY_DELAY);
		}
	}
	//teammate apologizes
	else if (msg == "Group4Message5" || msg == "Group4Message7" || msg == "Group4Message6" || msg == "Group4Message4" || msg == "Group4Message3")
	{
		if (*apologyRepliesEnabled && givenSecondsHavePassed(lastApologyReply, 15))
		{
			lastApologyReply = std::chrono::system_clock::now();
			sendChat('4', '2', APOLOGY_REPLY_DELAY);
		}
	}
}

void AutoReply::handleGoalEvent(PriWrapper& primaryPRI, PriWrapper& receiverPRI)
{
	if (primaryPRI.memory_address == receiverPRI.memory_address)
	{
		lastGoal = std::chrono::system_clock::now();
		respondedToCompliment = false;
		if (*goalComplimentRepliesEnabled && isWithinGivenSeconds(lastGoalCompliment, 5))
		{
			respondedToCompliment = true;
			sendChat('2', '3', COMPLIMENT_REPLY_DELAY);
		}
	}
	else if (*goalComplimentEnabled && primaryPRI.GetTeam().GetTeamNum() == receiverPRI.GetTeam().GetTeamNum())
	{
		sendChat('2', '1', COMPLIMENT_DELAY);
	}
}

void AutoReply::handleAssistEvent(PriWrapper& primaryPRI, PriWrapper& receiverPRI)
{
	if (primaryPRI.memory_address == receiverPRI.memory_address)
	{
		lastAssist = std::chrono::system_clock::now();
		respondedToCompliment = false;
		if (*assistComplimentRepliesEnabled && isWithinGivenSeconds(lastAssistCompliment, 5))
		{
			respondedToCompliment = true;
			sendChat('2', '3', COMPLIMENT_REPLY_DELAY);
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
	PriWrapper receiverPRI = PriWrapper(pStruct->Receiver);
	if (!receiverPRI) return;
	StatEventWrapper statEvent = StatEventWrapper(pStruct->StatEvent);

	PlayerControllerWrapper playerController = gameWrapper->GetPlayerController();
	if (!playerController) return;
	PriWrapper primaryPRI = playerController.GetPRI();
	if (!primaryPRI) return;

	if (statEvent.GetEventName() == "Goal")
	{
		handleGoalEvent(primaryPRI, receiverPRI);
	}
	else if (statEvent.GetEventName() == "Assist")
	{
		handleAssistEvent(primaryPRI, receiverPRI);
	}
}

void AutoReply::onUnload()
{
}