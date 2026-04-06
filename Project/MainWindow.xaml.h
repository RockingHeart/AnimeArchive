#pragma once

#include "WinUILib.h"
#include "MainWindow.g.h"

using namespace winrt::Microsoft::UI::Xaml;

namespace winrt::Project::implementation
{
    class MainWindow : public MainWindowT<MainWindow>
    {

    public:

        MainWindow();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        winrt::Windows::Foundation::IAsyncAction MainNavigationEvent(Controls::NavigationView const&,
            Controls::NavigationViewItemInvokedEventArgs const&);

        winrt::Windows::Foundation::IAsyncAction UnLoadNavigation(winrt::Windows::Foundation::IInspectable const&,
            winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:

        winrt::Windows::Foundation::IAsyncOperation<Controls::NavigationViewItem>         AddFile(winrt::Windows::Storage::StorageFile File, Controls::Frame&);
        winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::StorageFile> SelectFile();
        void RemoveTag(winrt::hstring);
    };
}

namespace winrt::Project::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
