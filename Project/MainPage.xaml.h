#pragma once

#include "WinUILib.h"
#include "MainPage.g.h"

#include "Config.h"
#include <vector>

using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI;

namespace winrt::AnimeArchive::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage() : State(Status::Non), IsStared(false), IsSearching(false), IsSelected(false), JumpTag(0)
        {
            InitializeComponent();
            FileNameText().Text(L"当前文件：" + Cfg::UI::Archives[Cfg::UI::CurrentTag].file_name());
        }

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void ShowContext(winrt::Windows::Foundation::IInspectable const&,
            winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);

        winrt::Windows::Foundation::IAsyncAction Choose(winrt::Windows::Foundation::IInspectable const&,
            winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);

        winrt::Windows::Foundation::IAsyncAction Add(winrt::Windows::Foundation::IInspectable const&,
            winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);

        winrt::Windows::Foundation::IAsyncAction StarClick(winrt::Windows::Foundation::IInspectable const&,
            winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);

        winrt::Windows::Foundation::IAsyncAction LoadStar(winrt::Windows::Foundation::IInspectable const&,
            winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);

        winrt::Windows::Foundation::IAsyncAction UnloadStar(winrt::Windows::Foundation::IInspectable const&,
            winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);

        winrt::Windows::Foundation::IAsyncAction SearchButtonClick(winrt::Windows::Foundation::IInspectable const&,
            winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:

        enum class Status {
            Non,
            View,
            Choose
        };

        Status State;
        bool IsStared;
        bool IsSearching;
        bool IsSelected;
        std::size_t JumpTag;

    private:

        winrt::Windows::Foundation::IAsyncAction DefAddToChoose(Controls::UIElementCollection const& ChildrenElems);
        void AddDeleteEventToButton(Controls::Primitives::ToggleButton const& ToggleButton);
        Controls::Primitives::ToggleButton CreateToggleButton(Controls::Primitives::ToggleButton const& Button);
        void ClearOptionElems();
    };
}

namespace winrt::AnimeArchive::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
