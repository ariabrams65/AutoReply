#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "bakkesmod/wrappers/GameObject/Stats/StatEventWrapper.h"

#include "version.h"

#include <chrono>

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class AutoReply : public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow/*, public BakkesMod::Plugin::PluginWindow*/
{
	using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

	//std::shared_ptr<bool> enabled;

	//Boilerplate
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
	bool withinDuration(TimePoint timestamp, int seconds, bool lessThan);
	void pressKey(char key);
	void cVarEnabledChanged();
	void hookAll();
	void unhookAll();
	void renderCheckbox(const std::string& cvar, const char* desc, const char* hoverText);

private:
	bool responded;
	TimePoint lastGoal;
	TimePoint lastAssist;
	TimePoint lastGoalComp;
	TimePoint lastAssistComp;
	TimePoint lastApologyReply;

	std::shared_ptr<bool> goalCompRepliesEnabled;
	std::shared_ptr<bool> assistCompRepliesEnabled;
	std::shared_ptr<bool> apologyRepliesEnabled;
	std::shared_ptr<bool> goalCompEnabled;

	bool hooked;

	// Inherited via PluginWindow
	/*

	bool isWindowOpen_ = false;
	bool isMinimized_ = false;
	std::string menuTitle_ = "AutoReply";

	virtual void Render() override;
	virtual std::string GetMenuName() override;
	virtual std::string GetMenuTitle() override;
	virtual void SetImGuiContext(uintptr_t ctx) override;
	virtual bool ShouldBlockInput() override;
	virtual bool IsActiveOverlay() override;
	virtual void OnOpen() override;
	virtual void OnClose() override;

	*/
};


