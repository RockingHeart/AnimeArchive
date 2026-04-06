#include "WinUILib.h"
#include "Config.h"

void Cfg::UI::Load() {
	LoadLogLimitSetValue();
}

void Cfg::UI::LoadLogLimitSetValue() {
	std::size_t Index = Cfg::UI::Archives[L"Settings"].get(L"LogLimit.");
	if (Index != std::wstring::npos) {
		std::wstring LogLimitSet(Cfg::UI::Archives[L"Settings"][Index]);
		std::size_t ValueIndex = LogLimitSet.find(L'.');
		if (ValueIndex != std::wstring::npos) {
			std::wstring ValueContext = LogLimitSet.substr(ValueIndex + 1);
			Cfg::UI::LogLimit = std::stoull(ValueContext);
			return;
		}
	}
	else {
		Cfg::UI::LogLimit = Cfg::UI::GetLogLimitDefValue();
	}
}