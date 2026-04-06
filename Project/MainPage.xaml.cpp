#include "WinUILib.h"
#include "MainPage.xaml.h"
#if __has_include("MainPage.g.cpp")
#include "MainPage.g.cpp"
#endif

#include "Log.h"
#include <vector>

#include <shobjidl.h>
#include <pplawait.h>

using namespace winrt;
using namespace Microsoft::UI;
using namespace Windows::System;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Core;
using namespace Microsoft::UI::Xaml::Controls::Primitives;
using namespace winrt::Microsoft::UI::Xaml::Media;
using namespace winrt::Microsoft::UI::Input;
using winrt::Windows::Foundation::IAsyncAction;
using winrt::Windows::Foundation::IAsyncOperation;

using namespace Cfg::UI;

namespace {

    bool HasArchive() {
        if (Archives.find(CurrentTag) == Archives.end()) {
            return false;
        }
        return Archives[CurrentTag] != false;
    }

    void UpChooseElemIndex(Controls::UIElementCollection& ChildrenElems, std::vector<std::size_t>& Indexs) {
        for (auto src : ChildrenElems) {
            if (auto elem = src.try_as<Controls::CheckBox>()) {
                std::size_t index = std::stoull(elem.Name().c_str());
                std::size_t up = index - Indexs.size();
                elem.Name(winrt::to_hstring(up));
            }
        }
    }

    void CardStyle(Controls::Primitives::ToggleButton const& ToggleButton) {
        ToggleButton.Margin({ 0, 0, 15, 0 });
        ToggleButton.HorizontalAlignment(HorizontalAlignment::Stretch);
        ToggleButton.HorizontalContentAlignment(HorizontalAlignment::Left);
        ToggleButton.CornerRadius({ 8, 8, 8, 8 });
        ToggleButton.Padding({ 20, 20, 20, 20 });
        ToggleButton.FontWeight(Windows::UI::Text::FontWeights::SemiBold());
        ToggleButton.Background(Application::Current().Resources().Lookup(box_value(L"CardBackgroundFillColorDefaultBrush")).as<Media::Brush>());
        ToggleButton.BorderBrush(Application::Current().Resources().Lookup(box_value(L"CardStrokeColorDefaultBrush")).as<Media::Brush>());
    }

    bool IsNumber(winrt::hstring const& str) {
        if (str.empty()) return false;

        unsigned long long value;
        std::wistringstream iss(str.c_str());
        iss >> value;

        return !iss.fail() && iss.eof();
    }
}

namespace winrt::Project::implementation
{
    IAsyncAction MainPage::DefAddToChoose(Controls::UIElementCollection const& ChildrenElems) {
        auto Viewer = Archives[CurrentTag].view() ^ [this, &ChildrenElems](winrt::hstring LineContent, std::size_t index) -> IAsyncAction {
            Controls::Primitives::ToggleButton ToggleButton;

            ToggleButton.Content(winrt::box_value(LineContent));
            std::size_t sub = index > 0 ? index - 1 : index;
            ToggleButton.Name(winrt::to_hstring(sub));

            AddDeleteEventToButton(ToggleButton);

            CardStyle(ToggleButton);

            ChildrenElems.Append(ToggleButton);
            co_return;
        };
        co_await Viewer;
    }

    void MainPage::AddDeleteEventToButton(Controls::Primitives::ToggleButton const& ToggleButton) {
        ToggleButton.Checked([this](auto const& Sender, auto const&) {
            auto Box = Sender.as<Controls::Primitives::ToggleButton>();
            Archives[CurrentTag].add_to_deletes(Box.Name());
            IsSelected = true;
        });

        ToggleButton.Unchecked([this](auto const& Sender, auto const&) {
            auto Box = Sender.as<Controls::Primitives::ToggleButton>();
            Archives[CurrentTag].remove_elem_from_deletes(Box.Name());
            IsSelected = false;
        });
    }

    Controls::Primitives::ToggleButton MainPage::CreateToggleButton(Controls::Primitives::ToggleButton const& Button) {
        Controls::Primitives::ToggleButton NewButton;
        NewButton.Content(box_value((Button.Content().as<winrt::hstring>())));
        NewButton.Name(Button.Name());
        CardStyle(NewButton);
        AddDeleteEventToButton(NewButton);
        return NewButton;
    }
    void MainPage::ClearOptionElems() {
        ArchiveOption().Children().Clear();
        IsSelected = false;
    }
}

namespace winrt::Project::implementation
{
    int32_t MainPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void MainPage::ShowContext(winrt::Windows::Foundation::IInspectable const& Sender,
                            winrt::Microsoft::UI::Xaml::RoutedEventArgs const& E)
    {
        if (!HasArchive()) {
            PushLog(Logs(), LogScrollViewer(), L"没有数据");
            return;
        }

        ArchiveOptionViewName().Text(L"档案查看");
        SearchText().IsReadOnly(false);
        SearchButtonFontIcon().Glyph(L"\uE721");
        SearchText().Visibility(Xaml::Visibility::Collapsed);
        SearchButton().Visibility(Xaml::Visibility::Collapsed);
        
        auto ChildrenElems = ArchiveOption().Children();

        if (State == Status::Choose) {
            ChooseButton().Content(winrt::box_value(L"选定"));
        }

        if (ChildrenElems.Size() > 0) {
            PushLog(Logs(), LogScrollViewer(), L"刷新查看框");
            ClearOptionElems();
        }

        auto TextBlock = winrt::Microsoft::UI::Xaml::Controls::TextBlock();
        std::wstring resu;
        Archives[CurrentTag].view() | [&resu](winrt::hstring& lineContent, std::size_t index) {
            resu += lineContent.c_str();
            resu += L'\n';
        };
        TextBlock.Text(resu);
        TextBlock.Foreground(winrt::Microsoft::UI::Xaml::Media::SolidColorBrush(
            winrt::Microsoft::UI::Colors::Black()));

        ArchiveOption().Children().Append(TextBlock);
        Archives[CurrentTag].clear_deletes();
        State = Status::View;
    }

    IAsyncAction MainPage::Choose(winrt::Windows::Foundation::IInspectable const& Sender,
                               winrt::Microsoft::UI::Xaml::RoutedEventArgs const& E)
    {
        auto Panel         = ArchiveOption();
        auto ChildrenElems = Panel.Children();

        if (!HasArchive()) {
            PushLog(Logs(), LogScrollViewer(), L"没有数据");
            co_return;
        }

        ArchiveOptionViewName().Text(L"档案操作");
        SearchText().Visibility(Xaml::Visibility::Visible);
        SearchButton().Visibility(Xaml::Visibility::Visible);

        auto Named = ChooseButton().Content().as<winrt::hstring>();

        if (Named != L"提取" && ChildrenElems.Size()) {
            PushLog(Logs(), LogScrollViewer(), L"已清理操作框");
            ClearOptionElems();
        }

        if (Named == L"提取" && (!IsSelected || !ChildrenElems.Size())) {
            PushLog(Logs(), LogScrollViewer(), L"没有档案或者没有选中档案");
            co_return;
        }

        auto& Archive = Archives[CurrentTag];
        if (Archive.must_delete()) {
            std::vector<size_t> Indexs;
            co_await Archive.take_deletes (
                [this, &ChildrenElems, &Indexs](winrt::hstring resu) -> IAsyncAction {
                    std::size_t Index = std::stoull(resu.c_str());
                    std::wstring URL;
                    switch (JumpTag) {
                        case 1:
                            auto Target = ChildrenElems.GetAt(Index).as<Controls::Primitives::ToggleButton>().Content()
                                .as<winrt::hstring>();
                            if (IsNumber(Target)) {
                                URL = L"https://18comic.vip/album/";
                            }
                            else {
                                URL = L"https://18comic.vip/search/photos?main_tag=0&search_query=";
                            }
                            Launcher::LaunchUriAsync(Windows::Foundation::Uri(URL + Target));
                            break;
                    }
                    Indexs.push_back(Index);
                    co_return;
                }
            );
            
            std::sort(Indexs.begin(), Indexs.end(), std::greater<size_t>());
            for (auto& index : Indexs) {
                ChildrenElems.RemoveAt(index);
            }
            co_await Archive.update();
            UpChooseElemIndex(ChildrenElems, Indexs);
            co_return;
        }

        ChooseButton().Content(winrt::box_value(L"提取"));

        Controls::ComboBox ComboBox;

        Controls::ComboBoxItem DefCommboxItem;
        DefCommboxItem.Content(winrt::box_value(L"不跳转"));
        DefCommboxItem.Tag(winrt::box_value(0));

        Controls::ComboBoxItem JmItem;
        JmItem.Content(winrt::box_value(L"跳转到JM"));
        JmItem.Tag(winrt::box_value(1));

        ComboBox.Items().Append(DefCommboxItem);
        ComboBox.Items().Append(JmItem);

        ComboBox.SelectionChanged([this](const auto& Sender, const auto& E) -> IAsyncAction {
            auto Currne = Sender.as<Controls::ComboBox>();
            if (auto Item = Currne.SelectedItem().try_as<Controls::ComboBoxItem>())
            {
                JumpTag = Item.Tag().as<int>();
            }
            co_return;
        });

        ComboBox.SelectedIndex(0);

        ArchiveOpitons().Children().Append(ComboBox);
        co_await DefAddToChoose(ChildrenElems);
        State = Status::Choose;
    }

    IAsyncAction MainPage::Add(winrt::Windows::Foundation::IInspectable const& Sender,
                            winrt::Microsoft::UI::Xaml::RoutedEventArgs const& E)
    {
        if (HasArchive() && State != Status::Choose) {
            PushLog(Logs(), LogScrollViewer(), L"已打开选择标签");
            Choose(nullptr, nullptr);
        }

        Controls::TextBox InputTextBox;
        InputTextBox.PlaceholderText(L"输入内容");
        InputTextBox.Width(300);
        InputTextBox.AcceptsReturn(true);

        Controls::ContentDialog Dialog;
        Dialog.Title(box_value(L"添加档案"));
        Dialog.Content(InputTextBox);
        Dialog.PrimaryButtonText(L"确定");
        Dialog.CloseButtonText(L"取消");

        Dialog.XamlRoot(Content().XamlRoot());

        Controls::ContentDialogResult result = co_await Dialog.ShowAsync();

        if (result != Controls::ContentDialogResult::Primary) {
            co_return;
        }

        auto InputContext = InputTextBox.Text();

        auto& archive = Archives[CurrentTag];

        if (!co_await archive.save(InputContext)) {
            co_await PromptWindow(Content().XamlRoot(), L"提示", L"已存在该档案");
            co_return;
        }

        /*Controls::Primitives::ToggleButton ToggleButton;
        ToggleButton.Content(winrt::box_value(InputContext));
        ToggleButton.Name(winrt::to_hstring(archive.lastsub()));
        CardStyle(ToggleButton);
        AddDeleteEventToButton(ToggleButton);
        ArchiveOption().Children().Append(ToggleButton);*/
        ClearOptionElems();
        DefAddToChoose(ArchiveOption().Children());
        co_await archive.update();
    }

    IAsyncAction MainPage::StarClick(winrt::Windows::Foundation::IInspectable const& Sender,
        winrt::Microsoft::UI::Xaml::RoutedEventArgs const& E)
    {
        if (IsStared = !IsStared; IsStared) {
            PushLog(Logs(), LogScrollViewer(), L"已钉该档案");
        }
        else {
            PushLog(Logs(), LogScrollViewer(), L"已取钉该档案");
        }
        co_return;
    }

    IAsyncAction MainPage::LoadStar(winrt::Windows::Foundation::IInspectable const& Sender,
        winrt::Microsoft::UI::Xaml::RoutedEventArgs const& E)
    {
        auto Stapler = Archives[L"Stapler"];
        auto FileLogo = CurrentTag + L'~';
        auto StarFile = FileLogo + Archives[CurrentTag].file_info().Path();
        IsStared = Stapler.find_dup(StarFile);
        Star().IsChecked(IsStared);
        co_return;
    }

    IAsyncAction MainPage::UnloadStar(winrt::Windows::Foundation::IInspectable const& Sender,
        winrt::Microsoft::UI::Xaml::RoutedEventArgs const& E)
    {
        auto Stapler = Archives[L"Stapler"];
        auto FileLogo = CurrentTag + L'~';
        auto StarFile = FileLogo + Archives[CurrentTag].file_info().Path();
        if (IsStared) {
            Stapler.save(StarFile);
        }
        else {
            Stapler.take(StarFile);
        }
        Stapler.update();
        co_return;
    }

    winrt::Windows::Foundation::IAsyncAction MainPage::SearchButtonClick(winrt::Windows::Foundation::IInspectable const&,
        winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        if (!HasArchive()) {
            PushLog(Logs(), LogScrollViewer(), L"没有数据");
            co_return;
        }

        auto OptionPanel = ArchiveOption();
        auto OptionChildren = OptionPanel.Children();

        if (IsSearching = !IsSearching; IsSearching) {
            SearchText().IsReadOnly(false);
            SearchButtonFontIcon().Glyph(L"\uE721");
            DefAddToChoose(OptionChildren);
            co_return;
        }

        Controls::StackPanel NewPanel;
        auto PanelChildren = NewPanel.Children();

        for (auto const& Elem : OptionChildren) {
            if (auto Button = Elem.try_as<Controls::Primitives::ToggleButton>()) {
                if (auto Content = Button.Content().try_as<winrt::hstring>()) {
                    auto SearchContext = SearchText().Text();
                    std::wstring ContentOfWstr = Content.value().c_str();
                    if (ContentOfWstr.find(SearchContext) != std::wstring::npos) {
                        auto NewButton = CreateToggleButton(Button);
                        PanelChildren.Append(NewButton);
                    }
                }
            }
        }

        if (std::size_t NewSize = PanelChildren.Size(); NewSize) {
            ClearOptionElems();
            for (auto const& Elem : PanelChildren) {
                if (auto Button = Elem.try_as<Controls::Primitives::ToggleButton>()) {
                    auto NewButton = CreateToggleButton(Button);
                    OptionChildren.Append(NewButton);
                }
            }
            
            PushLog(Logs(), LogScrollViewer(), (L"搜索到：" + winrt::to_hstring(NewSize)) + L"个结果");
            PushLog(Logs(), LogScrollViewer(), L"搜索内容：" + SearchText().Text());
        }
        else {
            PushLog(Logs(), LogScrollViewer(), winrt::hstring(L"搜索不到：") + SearchText().Text());
            co_return;
        }

        SearchText().IsReadOnly(true);
        SearchButtonFontIcon().Glyph(L"\uE711");
        co_return;
    }
}
