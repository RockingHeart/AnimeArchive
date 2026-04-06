#pragma once

#include "Settings.g.h"
#include "Config.h"

namespace winrt::Project::implementation
{
    struct Settings : SettingsT<Settings>
    {
        Settings()
        {

        }

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void SettingsLoaded(winrt::Windows::Foundation::IInspectable const& p0,
            winrt::Microsoft::UI::Xaml::RoutedEventArgs const& p1);

        void SettingsUnLoaded(winrt::Windows::Foundation::IInspectable const& p0,
            winrt::Microsoft::UI::Xaml::RoutedEventArgs const& p1);

        void LogLimitValueChanged(winrt::Microsoft::UI::Xaml::Controls::NumberBox const&,
            winrt::Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs const&);

    public:

        constexpr static std::size_t GetLogLimitDefValue();
    };
}

namespace winrt::Project::factory_implementation
{
    struct Settings : SettingsT<Settings, implementation::Settings>
    {
    };
}
