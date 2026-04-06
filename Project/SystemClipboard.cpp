#include "WinUILib.h"
#include "SystemClipboard.h"

using namespace winrt;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::Storage::Streams;
using winrt::Windows::Foundation::IAsyncOperation;

using str_t = typename sys_clipboard::str_t;

IAsyncOperation<bool> sys_clipboard::save(str_t content) {
    try {
        DataPackage DataPackage;
        DataPackage.SetText(to_hstring(content));
        Clipboard::SetContent(DataPackage);
        co_return true;
    }
    catch (...) {
        co_return false;
    }
}

IAsyncOperation<str_t> sys_clipboard::read() {
    try {
        auto DataPackageView = Clipboard::GetContent();
        if (DataPackageView.Contains(StandardDataFormats::Text())) {
            co_await DataPackageView.GetTextAsync();
        }
    }
    catch (...) {
    }
    co_return str_t();
}

IAsyncOperation<bool> sys_clipboard::clear() {
    try {
        DataPackage DataPackage;
        Clipboard::SetContent(DataPackage);
        co_return true;
    }
    catch (...) {
    }
    co_return false;
}

IAsyncOperation<bool> sys_clipboard::exist() {
    try {
        auto DataPackageView = Clipboard::GetContent();
        co_return DataPackageView.Contains(StandardDataFormats::Text());
    }
    catch (...) {
    }
    co_return false;
}