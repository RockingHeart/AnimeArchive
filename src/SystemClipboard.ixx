export module sys_clipboard;

import <string>;
import <Windows.h>;
import <cstddef>;

auto memory_copy(char* tar, const char* buf, std::size_t size) noexcept {
	return memcpy(tar, buf, size);
}

auto memory_copy(wchar_t* tar, const wchar_t* buf, std::size_t size) noexcept {
	return wmemcpy(tar, buf, size);
}

export template <class DataType>
class sys_clipboard {

private:
	using data_type   =		 DataType;
	using string_type = std::basic_string<data_type>;

private:
	bool	closed;
	HGLOBAL global_memory;

private:
	constexpr void close_clipboard() noexcept {
		if (closed) {
			return;
		}
		CloseClipboard();
		closed = true;
	}

	constexpr void open_clipboard() {
		if (!OpenClipboard(nullptr)) {
			throw "Can't open the clipboard";
		}
		closed = false;
	}

	constexpr UINT clipboard_format() noexcept {
		if constexpr (std::is_same_v<data_type, wchar_t>) {
			return CF_UNICODETEXT;
		}
		return CF_TEXT;
	}

	constexpr std::size_t ext() const noexcept {
		if constexpr (std::is_same_v<data_type, wchar_t>) {
			return sizeof(wchar_t);
		}
		return 1;
	}

public:

	constexpr sys_clipboard()
		: closed(false)
	{
		open_clipboard();
	}

public:

	constexpr bool save(const string_type& content) {
		if (closed) {
			open_clipboard();
		}
		if (!EmptyClipboard()) {
			return false;
		}
		static constexpr auto extval = ext();
		std::size_t size = (content.size() + 1) * extval;
		auto global_memory = GlobalAlloc(GMEM_MOVEABLE, size);
		if (global_memory == nullptr) {
			close_clipboard();
			return false;
		}
		data_type* lock_memory = static_cast<data_type*>(GlobalLock(global_memory));
		if (lock_memory) {
			memory_copy(lock_memory, content.c_str(), size);
			GlobalUnlock(global_memory);
		}
		static constexpr auto format = clipboard_format();
		if (!SetClipboardData(format, global_memory)) {
			GlobalFree(global_memory);
			close_clipboard();
			return false;
		}
		close_clipboard();
		return true;
	}

	constexpr bool clear_clipboard(HWND owner = nullptr) {
		if (closed) {
			open_clipboard();
		}
		bool result = EmptyClipboard();
		close_clipboard();
		return result;
	}


public:

	constexpr ~sys_clipboard(void) {
		if (!closed && !CloseClipboard()) {
			throw "Can't close the clipboard";
		}
	}
};