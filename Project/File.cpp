#include "WinUILib.h"
#include "File.h"
#include <shobjidl.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml;
using namespace Windows;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Microsoft::UI::Windowing;
using winrt::Windows::Foundation::IAsyncOperation;

IAsyncOperation<StorageFile> File::OpenFileSync(winrt::Microsoft::UI::WindowId WindowId) try {
    StorageFile Result = { nullptr };

    HWND WindowHwnd = GetWindowFromWindowId(WindowId);

    if (WindowHwnd == nullptr) {
        co_return Result;
    }

    FileOpenPicker Picker;

    auto InitializeWithWindow = Picker.as<::IInitializeWithWindow>();
    if (!InitializeWithWindow)
    {
        co_return Result;
    }

    InitializeWithWindow->Initialize(WindowHwnd);

    Picker.ViewMode(PickerViewMode::Thumbnail);
    Picker.SuggestedStartLocation(PickerLocationId::PicturesLibrary);
    Picker.FileTypeFilter().ReplaceAll({ L".txt", L".a" });

    Result = co_await Picker.PickSingleFileAsync();

    co_return Result;

}
catch (winrt::hresult_error const& ex)
{
    OutputDebugString((L"错误: " + ex.message() + L"\n").c_str());
    co_return nullptr;
}

IAsyncOperation<StorageFile> File::OpenFolderSync(winrt::hstring const& FileName) {
    try
    {
        StorageFolder LocalFolder = ApplicationData::Current().LocalFolder();
        StorageFile Result = co_await LocalFolder.CreateFileAsync(FileName, CreationCollisionOption::OpenIfExists);
        co_return Result;
    }
    catch (winrt::hresult_error const& ex)
    {
        OutputDebugString((L"错误: " + ex.message() + L"\n").c_str());
        co_return nullptr;
    }
    catch (...)
    {
        OutputDebugString(L"未知错误\n");
        co_return nullptr;
    }
}