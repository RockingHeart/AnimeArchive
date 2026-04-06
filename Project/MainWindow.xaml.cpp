#include "WinUILib.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include "Log.h"
#include "Config.h"
#include "File.h"

#include <pplawait.h>
#include <string>
#include <string_view>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml;
using namespace Windows;
using namespace Windows::Storage;
using winrt::Windows::Foundation::IAsyncAction;
using winrt::Windows::Foundation::IAsyncOperation;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace Windows::UI::Core;
using namespace Microsoft::UI::Windowing;

using namespace Cfg::UI;

namespace {
    Controls::NavigationViewItem FindNavViewItem(auto Container, const hstring& Tag)
    {
        for (const auto& Item : Container)
        {
            if (auto NavItem = Item.try_as<Controls::NavigationViewItem>())
            {
                auto ItemTag = NavItem.Tag().try_as<Foundation::IPropertyValue>();
                if (ItemTag && ItemTag.as<winrt::hstring>() == Tag)
                {
                    return NavItem;
                }
            }
        }
        return nullptr;
    }
}

namespace winrt::AnimeArchive::implementation
{
    IAsyncOperation<StorageFile> MainWindow::SelectFile() {
        StorageFile File = nullptr;
        auto WindowId = AppWindow().Id();

        try {
            File = co_await File::OpenFileSync(WindowId);
        }
        catch (...) {
            co_return File;
        }

        if (!File) {
            PromptWindow(Content().XamlRoot(), L"文件未打开", L"提示");
            MainNavigation().SelectedItem(nullptr);
        }

        co_return File;
    }

    void MainWindow::RemoveTag(winrt::hstring TargetTag) {
        auto MenuItems = MainNavigation().MenuItems();
        for (std::size_t i = 0; i < MenuItems.Size(); i++) {
            if (auto Tag = MenuItems.GetAt(i).try_as<Controls::NavigationViewItem>()) {
                auto TagNamed = Tag.Tag().as<winrt::hstring>();
                if (TagNamed == TargetTag) {
                    MenuItems.RemoveAt(i);
                    break;
                }
            }
        }
    }
}

namespace winrt::AnimeArchive::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
    }
}

namespace winrt::AnimeArchive::implementation
{

    int32_t MainWindow::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainWindow::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    IAsyncOperation<Controls::NavigationViewItem> MainWindow::AddFile(StorageFile File, Controls::Frame& Frame) {
        winrt::hstring TagNamed = File.Name() + L"Tag";
        auto MenuItems = MainNavigation().MenuItems();

        if (Archives.find(TagNamed) != Archives.end()) {
            Controls::ContentDialog Dialog;
            Dialog.Title(box_value(L"提示"));
            Dialog.Content(box_value(L"检测到你已打开该档案，请选择操作"));
            Dialog.PrimaryButtonText(L"激活");
            Dialog.SecondaryButtonText(L"重新打开");

            Dialog.XamlRoot(Content().XamlRoot());

            Controls::ContentDialogResult result = co_await Dialog.ShowAsync();
            if (result == Controls::ContentDialogResult::Secondary) {
                RemoveTag(TagNamed);
                Archives.erase(TagNamed);
            }
            else {
                CurrentTag = TagNamed;
                MainNavigation().SelectedItem(FindNavViewItem(MenuItems, TagNamed));
                auto TargetPageType = winrt::xaml_typename<AnimeArchive::MainPage>();
                Frame.Navigate(TargetPageType);
                co_return nullptr;
            }
        }

        auto Item = Controls::NavigationViewItem();
        auto TextBlock = Controls::TextBlock();
        TextBlock.Text(File.Name());
        Item.Content(TextBlock);
        auto Icon = Controls::FontIcon();
        Icon.Glyph(L"\uE8A5");
        Item.Icon(Icon);
        Item.Tag(winrt::box_value(TagNamed));
        Archives.insert({ TagNamed, File });
        co_await Archives[TagNamed].init();
        CurrentTag = TagNamed;
        MenuItems.Append(Item);
        co_return Item;
    }

    IAsyncAction MainWindow::MainNavigationEvent (Controls::NavigationView const& Sender,
                      Controls::NavigationViewItemInvokedEventArgs const& args)
    {
        auto Frame = NavigationFrame();
        if (Sender.Name() == L"InitStaplers") {
            auto Stapler = Archives[L"Stapler"];
            auto Viewer = Stapler.view() ^ [this, &Frame](winrt::hstring context, std::size_t) -> IAsyncAction {
                std::wstring Context(context);
                std::size_t Start = Context.find(L'~');
                winrt::hstring Path = Context.substr(Start + 1, Context.size()).c_str();
                auto File = co_await StorageFile::GetFileFromPathAsync(Path);
                co_await AddFile(File, Frame);
            };
            co_await Viewer;
            MainNavigation().SelectedItem(nullptr);
            co_return;
        }

        if (args.IsSettingsInvoked())
        {
            auto TargetPageType = winrt::xaml_typename<AnimeArchive::Settings>();
            Frame.Navigate(TargetPageType);
            co_return;
        }

        auto Container = args.InvokedItemContainer();
        auto Tag = Container.Tag();
        if (!Tag) {
            co_return;
        }
        hstring TagNamed = unbox_value<hstring>(Tag);

        if (TagNamed == L"OpenArchive") {
            Frame.Content(0);
            auto File = co_await SelectFile();
            if (!File) {
                co_return;
            }
            auto Item = co_await AddFile(File, Frame);
            MainNavigation().SelectedItem(Item);
            auto TargetPageType = winrt::xaml_typename<AnimeArchive::MainPage>();
            Frame.Navigate(TargetPageType);
            co_return;
        }
        else {
            if (Archives.find(TagNamed) == Archives.end()) {
                co_return;
            }
            CurrentTag = TagNamed;
            auto TargetPageType = winrt::xaml_typename<AnimeArchive::MainPage>();
            Frame.Navigate(TargetPageType);
        }
    }

    IAsyncAction MainWindow::UnLoadNavigation(winrt::Windows::Foundation::IInspectable const& Sender,
        winrt::Microsoft::UI::Xaml::RoutedEventArgs const& E)
    {
        auto Frame = NavigationFrame();
        if (auto Page = Frame.Content().try_as<AnimeArchive::MainPage>())
        {
            Page.ContextCanceled(nullptr);
        }
        co_return;
    }
}
