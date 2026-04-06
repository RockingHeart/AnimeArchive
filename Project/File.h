#pragma once
#include "WinUILib.h"

namespace File
{
	winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::StorageFile> OpenFileSync(winrt::Microsoft::UI::WindowId);
	winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::StorageFile> OpenFolderSync(winrt::hstring const&);
}