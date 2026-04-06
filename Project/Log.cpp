#include "WinUILib.h"
#include "Log.h"
#include "Config.h"

using namespace winrt;
using namespace Microsoft::UI;
using namespace Microsoft::UI::Xaml;
using winrt::Windows::Foundation::IAsyncAction;
using winrt::Microsoft::UI::Xaml::Controls::StackPanel;
using winrt::Microsoft::UI::Xaml::Controls::ScrollViewer;

std::wstring GetCurrentTime() {
    Calendar Calendar;
    Calendar.SetToNow();

    std::wstring time(std::to_wstring(Calendar.Hour()) + L'.');
    time += (std::to_wstring(Calendar.Minute()) + L'.');
    time += (std::to_wstring(Calendar.Second()));
    time += L"：";

    return time;
}

void PushLog(const StackPanel& Panel, const ScrollViewer& Viewer, const winrt::hstring& Log) {
    auto LogsChildren = Panel.Children();

    if (LogsChildren.Size() >= Cfg::UI::LogLimit) {
        LogsChildren.Clear();
    }

    auto TextBlock = winrt::Microsoft::UI::Xaml::Controls::TextBlock();
    TextBlock.Text(GetCurrentTime().c_str() + Log);
    TextBlock.Foreground(winrt::Microsoft::UI::Xaml::Media::SolidColorBrush(
        winrt::Microsoft::UI::Colors::WhiteSmoke()));
    LogsChildren.Append(TextBlock);

    Viewer.ChangeView(nullptr, Viewer.ScrollableHeight(), nullptr, true);
}

IAsyncAction PromptWindow(Xaml::XamlRoot Root, winrt::hstring Text, winrt::hstring Title) {
    Controls::ContentDialog Dialog;
    Dialog.Title(box_value(Title));
    auto TextBlock = Controls::TextBlock();
    TextBlock.Text(Text);
    Dialog.Content(TextBlock);
    Dialog.PrimaryButtonText(L"确定");

    Dialog.XamlRoot(Root);

    co_await Dialog.ShowAsync();
}