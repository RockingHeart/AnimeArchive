#pragma once
#include <WinUILib.h>
#include <string>
#include <type_traits>

namespace sys_clipboard
{
    using str_t = winrt::hstring;

    winrt::Windows::Foundation::IAsyncOperation<bool>  save(str_t content);
    winrt::Windows::Foundation::IAsyncOperation<str_t> read();
    winrt::Windows::Foundation::IAsyncOperation<bool>  clear();
    winrt::Windows::Foundation::IAsyncOperation<bool>  exist();
};