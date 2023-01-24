#include "pch.h"
#include "AutoReply.h"

std::string AutoReply::GetPluginName() {
	return "AutoReply";
}

void AutoReply::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

void AutoReply::renderCheckbox(const std::string& cvar, const char* desc, const char* hoverText)
{
	CVarWrapper enableCvar = cvarManager->getCvar(cvar);
	if (!enableCvar) return;
	bool enabled = enableCvar.getBoolValue();
	if (ImGui::Checkbox(desc, &enabled))
	{
		enableCvar.setValue(enabled);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(hoverText);
	}

}

void AutoReply::RenderSettings()
{
	renderCheckbox("AutoReplyEnabled", "Enable plugin", "Toggle AutoReply");
	renderCheckbox("goalCompRepliesEnabled", "Enable replies to goal compliments",
		"When a teammate sends a message such as \"Nice shot!\" after you score, automatically thank them");
	renderCheckbox("assistCompRepliesEnabled", "Enable replies to assist compliments",
		"When a teammate sends a message such as \"Great pass!\" after you assist them, automatically compliment their shot");
	renderCheckbox("apologyRepliesEnabled", "Enable replies to teammate apologies",
		"When a teammate sends a message such as \"Sorry!\" automatically let them now that there is no problem");
	renderCheckbox("goalCompEnabled", "Enable compliments to teammate's goals",
		"Automatically compliment teammotes goals");
}
