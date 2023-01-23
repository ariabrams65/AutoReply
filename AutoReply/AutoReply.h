#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "bakkesmod/wrappers/GameObject/Stats/StatEventWrapper.h"

#include "version.h"

#include <chrono>

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class AutoReply : public BakkesMod::Plugin::BakkesModPlugin/*, public BakkesMod::Plugin::PluginSettingsWindow*//*, public BakkesMod::Plugin::PluginWindow*/
{
	using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

	//std::shared_ptr<bool> enabled;

	//Boilerplate
public:
	virtual void onLoad();
	virtual void onUnload();
	void onChatMessage(void* params);
	void onStatEvent(void* params);
	void handleMessage(const std::string& msg);
	void evalDuration(char keyPress1, char keyPress2, TimePoint timestamp, int seconds);
	void sendChat(char input1, char input2, float delay);


private:
	bool responded;
	TimePoint lastGoal;
	TimePoint lastAssist;
	TimePoint lastGoalComp;
	TimePoint lastAssistComp;
	TimePoint lastApology;


	// Inherited via PluginSettingsWindow
	/*
	void RenderSettings() override;
	std::string GetPluginName() override;
	void SetImGuiContext(uintptr_t ctx) override;
	*/

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


