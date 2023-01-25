#include "pch.h"
#include "AutoReply.h"

std::string AutoReply::GetPluginName() {
	return "AutoReply";
}

void AutoReply::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

void AutoReply::renderCheckbox(const std::string& cvar, const char* desc)
{
	CVarWrapper enableCvar = cvarManager->getCvar(cvar);
	if (!enableCvar) return;
	bool enabled = enableCvar.getBoolValue();
	if (ImGui::Checkbox(desc, &enabled))
	{
		enableCvar.setValue(enabled);
	}
}

void AutoReply::RenderSettings()
{
	renderCheckbox("AutoReplyEnabled", "Enable plugin");
	ImGui::Separator();
	renderCheckbox("goalComplimentRepliesEnabled", "Say \"Thanks!\" when teammates compliment your goal");
	renderCheckbox("assistComplimentRepliesEnabled", "Say \"Thanks!\" when teammates compliment your assist");
	renderCheckbox("apologyRepliesEnabled", "Say \"No problem.\" when teammate apologizes");
	renderCheckbox("goalComplimentEnabled", "Say \"Nice shot!\" after a teammate scores");
}
