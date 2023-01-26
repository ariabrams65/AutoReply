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

void AutoReply::renderSlider(const std::string& cvar, const char* desc)
{		
	CVarWrapper delayCvar = cvarManager->getCvar(cvar);
	if (!delayCvar) return;
	float delay = delayCvar.getFloatValue();
	if (ImGui::SliderFloat(desc, &delay, SLIDER_MIN, SLIDER_MAX))
	{
		delayCvar.setValue(delay);
	}
}

void AutoReply::resetCvar(const std::string& cvar)
{
	CVarWrapper delayCvar = cvarManager->getCvar(cvar);
	if (!delayCvar) return;
	delayCvar.ResetToDefault();
}

void AutoReply::RenderSettings()
{
	renderCheckbox("AutoReplyEnabled", "Enable plugin");
	ImGui::Separator();
	renderCheckbox("goalComplimentRepliesEnabled", "Say \"Thanks!\" when teammates compliment your goal");
	renderSlider("goalComplimentReplyDelay", "Goal compliment reply delay (in seconds)");
	ImGui::Separator();
	renderCheckbox("assistComplimentRepliesEnabled", "Say \"Thanks!\" when teammates compliment your assist");
	renderSlider("assistComplimentReplyDelay", "Assist compliment reply delay (in seconds)");
	ImGui::Separator();
	renderCheckbox("apologyRepliesEnabled", "Say \"No problem.\" when teammate apologizes");
	renderSlider("apologyReplyDelay", "Apology reply delay (in seconds)");
	ImGui::Separator();
	renderCheckbox("goalComplimentEnabled", "Say \"Nice shot!\" after a teammate scores");
	renderSlider("goalComplimentDelay", "Goal compliment delay (in seconds)");
	ImGui::Separator();
	if (ImGui::Button("Reset sliders"))
	{
		resetCvar("goalComplimentReplyDelay");
		resetCvar("assistComplimentReplyDelay");
		resetCvar("assistComplimentReplyDelay");
		resetCvar("apologyReplyDelay");
		resetCvar("goalComplimentDelay");
	}
}
