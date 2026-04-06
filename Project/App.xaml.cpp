#include "WinUILib.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"

#include "Config.h"
#include "File.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Windowing;
using namespace Windows::Foundation;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::AnimeArchive::implementation
{
    /// <summary>
    /// Initializes the singleton application object.  This is the first line of authored code
    /// executed, and as such is the logical equivalent of main() or WinMain().
    /// </summary>
    App::App()
    {
        
#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
        UnhandledException([](IInspectable const&, UnhandledExceptionEventArgs const& e)
        {
            if (IsDebuggerPresent())
            {
                auto errorMessage = e.Message();
                __debugbreak();
            }
        });
#endif
    }

    IAsyncAction App::OnLaunched([[maybe_unused]] LaunchActivatedEventArgs const& e)
    {
        window = make<MainWindow>();
        auto appwindow = window.AppWindow();
        appwindow.Resize({1000, 800});
        auto presenter = appwindow.Presenter().as<OverlappedPresenter>();
        //presenter.IsResizable(false);
        //presenter.IsMaximizable(false);
        window.Activate();

        auto SettingsFile = co_await File::OpenFolderSync(L"Settings");
        Cfg::UI::Archives.insert({ L"Settings", SettingsFile });
        co_await Cfg::UI::Archives[L"Settings"].init();

        Cfg::UI::Load();

        auto StaplerFile = co_await File::OpenFolderSync(L"Stapler");
        Cfg::UI::Archives.insert({ L"Stapler", StaplerFile });
        co_await Cfg::UI::Archives[L"Stapler"].init();

        Controls::NavigationView Sender;
        Sender.Name(L"InitStaplers");
        window.as<MainWindow>().get()->MainNavigationEvent(Sender, nullptr);
    }
}
