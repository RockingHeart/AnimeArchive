#include "WinUILib.h"
#include "Settings.xaml.h"
#if __has_include("Settings.g.cpp")
#include "Settings.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::AnimeArchive::implementation
{
    int32_t Settings::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void Settings::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void Settings::SettingsLoaded(winrt::Windows::Foundation::IInspectable const& Sender,
        winrt::Microsoft::UI::Xaml::RoutedEventArgs const& E)
    {
        Cfg::UI::LoadLogLimitSetValue();
        LogLimit().Value(Cfg::UI::LogLimit);
    }

    void Settings::SettingsUnLoaded(winrt::Windows::Foundation::IInspectable const& p0,
        winrt::Microsoft::UI::Xaml::RoutedEventArgs const& p1)
    {
        auto& SettingsCfg = Cfg::UI::Archives[L"Settings"];
        std::size_t Index = SettingsCfg.get(L"LogLimit.");
        if (Index != std::wstring::npos) {
            SettingsCfg[Index] = (L"LogLimit." + winrt::to_hstring(Cfg::UI::LogLimit)).c_str();
        }
        else {
            SettingsCfg.save(L"LogLimit." + winrt::to_hstring(Cfg::UI::LogLimit));
        }
        SettingsCfg.update();
        return;
    }

    void Settings::LogLimitValueChanged(winrt::Microsoft::UI::Xaml::Controls::NumberBox const& Sender,
        winrt::Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs const& E)
    {
        Cfg::UI::LogLimit = E.NewValue();
    }

}
