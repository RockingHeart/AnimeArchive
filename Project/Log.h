#include "WinUILib.h"

using namespace winrt::Windows::Globalization;
using winrt::Microsoft::UI::Xaml::Controls::StackPanel;
using winrt::Microsoft::UI::Xaml::Controls::ScrollViewer;

std::wstring GetCurrentTime();

void PushLog(const winrt::Microsoft::UI::Xaml::Controls::StackPanel&,
	const winrt::Microsoft::UI::Xaml::Controls::ScrollViewer&, const winrt::hstring&);

winrt::Windows::Foundation::IAsyncAction PromptWindow(winrt::Microsoft::UI::Xaml::XamlRoot,
	winrt::hstring, winrt::hstring);