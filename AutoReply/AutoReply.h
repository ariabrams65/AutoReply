#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "bakkesmod/wrappers/GameObject/Stats/StatEventWrapper.h"

#include "version.h"

#include <chrono>

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class AutoReply : public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow
{
	using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

public:
	virtual void onLoad();
	virtual void onUnload();

	void RenderSettings() override;
	std::string GetPluginName() override;
	void SetImGuiContext(uintptr_t ctx) override;

private:
	void onChatMessage(void* params);
	void onStatEvent(void* params);
	void handleMessage(const std::string& msg);
	void sendChat(char input1, char input2, float delay);
	bool isWithinGivenSeconds(TimePoint timestamp, int seconds);
	bool givenSecondsHavePassed(TimePoint timestamp, int seconds);
	void pressKey(char key);
	void cVarEnabledChanged();
	void hookAll();
	void unhookAll();
	void renderCheckbox(const std::string& cvar, const char* desc);
	void handleGoalEvent(PriWrapper& primaryPRI, PriWrapper& receiverPRI);
	void handleAssistEvent(PriWrapper& primaryPRI, PriWrapper& receiverPRI);

private:
	bool responded;
	bool hooked;

	TimePoint lastGoal;
	TimePoint lastAssist;
	TimePoint lastGoalCompliment;
	TimePoint lastAssistCompliment;
	TimePoint lastApologyReply;

	std::shared_ptr<bool> goalComplimentRepliesEnabled;
	std::shared_ptr<bool> assistComplimentRepliesEnabled;
	std::shared_ptr<bool> apologyRepliesEnabled;
	std::shared_ptr<bool> goalComplimentEnabled;
};


