#pragma once

#include "App.xaml.g.h"

extern winrt::Microsoft::UI::Xaml::Window window;

namespace winrt::AnimeArchive::implementation
{
    struct App : AppT<App>
    {
        App();

        winrt::Windows::Foundation::IAsyncAction OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

    private:
        winrt::Microsoft::UI::Xaml::Window window = nullptr;
    };
}
