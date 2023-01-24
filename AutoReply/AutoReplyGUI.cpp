#include "pch.h"
#include "AutoReply.h"

std::string AutoReply::GetPluginName() {
	return "AutoReply";
}

void AutoReply::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

// Render the plugin settings here
// This will show up in bakkesmod when the plugin is loaded at
//  f2 -> plugins -> AutoReply

void AutoReply::renderCheckbox(const std::string& cvar, const char* desc, const char* hoverText)
{
	CVarWrapper enableCvar = cvarManager->getCvar(cvar);
	if (!enableCvar) return;
	bool enabled = enableCvar.getBoolValue();
	if (ImGui::Checkbox(desc, &enabled))
	{
		LOG("cvar toggled");
		enableCvar.setValue(enabled);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(hoverText);
	}

}

void AutoReply::RenderSettings() {
	renderCheckbox("AutoReplyEnabled", "Enable plugin", "Toggle AutoReply");
	renderCheckbox("goalCompRepliesEnabled", "Enable replies to goal compliments",
		"When a teammate sends a message such as \"Nice shot!\" after you score, automatically thank them");
	renderCheckbox("assistCompRepliesEnabled", "Enable replies to assist compliments",
		"When a teammate sends a message such as \"Great pass!\" after you assist them, automatically compliment their shot");
	renderCheckbox("apologyRepliesEnabled", "Enable replies to teammate apologies",
		"When a teammate sends a message such as \"Sorry!\" automatically let them now that there is no problem");
}

/*
// Do ImGui rendering here
void AutoReply::Render()
{
	if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, ImGuiWindowFlags_None))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	ImGui::End();

	if (!isWindowOpen_)
	{
		cvarManager->executeCommand("togglemenu " + GetMenuName());
	}
}

// Name of the menu that is used to toggle the window.
std::string AutoReply::GetMenuName()
{
	return "autoreply";
}

// Title to give the menu
std::string AutoReply::GetMenuTitle()
{
	return menuTitle_;
}

// Don't call this yourself, BM will call this function with a pointer to the current ImGui context
void AutoReply::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

// Should events such as mouse clicks/key inputs be blocked so they won't reach the game
bool AutoReply::ShouldBlockInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

// Return true if window should be interactive
bool AutoReply::IsActiveOverlay()
{
	return true;
}

// Called when window is opened
void AutoReply::OnOpen()
{
	isWindowOpen_ = true;
}

// Called when window is closed
void AutoReply::OnClose()
{
	isWindowOpen_ = false;
}
*/
