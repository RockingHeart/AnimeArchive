import std;
import archive;

import <cstddef>;
import <Windows.h>;

static archive<wchar_t> archive;

void show_result(const char* it) {
	std::cout << it << '\n';
	archive.view() | [](std::wstring& line, std::size_t size) {
		std::wcout << "\t[" << std::to_wstring(size) << "]: ";
		std::wcout << line << '\n';
	};
}

std::wstring take_file;

void archive_save(const std::wstring& value) {
	if (value.find(' ') == static_cast<std::size_t>(-1)) {
		archive.save(value);
		return;
	}
	std::wstring record;
	for (auto& elem : value) {
		if (elem == L' ') {
			archive.save(record);
			record.resize(0);
		}
		else {
			record += elem;
		}
	}
	if (!record.empty()) {
		archive.save(record);
	}
}

static constexpr const wchar_t* helps[] = {
	L"This is help of commits: ",
	L"\t -v [+]: all",
	L"\t\t The all value is for viewing all files",
	L"\t -t [arg: string]",
	L"\t\t Take the specified file",
	L"\t -a [arg: string]",
	L"\t\t Add the specified file",
	L"\t -o [+]: jm or pixiv",
	L"\t\t The value is for accessing the specified notebook via the take value"
};

bool handle_single(const std::wstring& commit, std::size_t& index) {
	if (commit == L"-h") {
		for (auto& help : helps) {
			std::wcout << help << '\n';
		}
		--index;
		return false;
	}
	if (commit == L"-cl") {
		archive.clear();
		return true;
	}
	std::wstring message(L"Unknow commit: ");
	message += commit;
	throw message;
}

std::unordered_map<std::wstring, void(*)(const std::wstring&)> commit_map;

void hander(const std::wstring& commit, const std::wstring& value, std::size_t& index) {
	if (value.empty()) {
		if (bool needup = handle_single(commit, index); !needup) {
			return;
		}
		goto update_data;
	}
	if (archive.remove_dup()) {
		goto update_data;
	}
	if (commit == L"-v") {
		if (value == L"all") {
			archive.view() | [](std::wstring& line, std::size_t size) {
				std::wcout << '[' << std::to_wstring(size) << "]: ";
				std::wcout << line << '\n';
			};
			return;
		}
	}
	else if (commit == L"-t") {
		take_file = archive.take(value);
		std::wcout << "Take out: " << take_file << '\n';
		show_result("The result after removal:");
	}
	else if (commit == L"-a") {
		archive_save(value);
		show_result("The result of save:");
	}
	else if (commit == L"-o" && !take_file.empty()) {
		std::wstring url;
		if (value == L"jm") {
			url = L"https://18comic.vip/album/";
		}
		else if (value == L"pixiv") {
			url = L"https://www.pixiv.net/artworks/";
		}
		else {
			std::wstring message(L"Unknow target: ");
			message += value;
			throw message;
		}
		url += take_file;
		HINSTANCE result = ShellExecuteW (
			NULL,
			L"open",
			url.c_str(),
			NULL,
			NULL,
			SW_SHOWNORMAL
		);
	}
	else {
		std::wstring message(L"Unknow commit: ");
		message += commit;
		throw message;
	}
update_data:
	archive.update();
	return;
}

struct wstr_cast {
	wchar_t* buffer;
	std::wstring data;
	wstr_cast(const char* cstr, std::size_t len, std::size_t size = 0)
		noexcept : data (
			(size = MultiByteToWideChar (
				GetACP(), 0, cstr, -1,
				buffer = new wchar_t[len + 1],
				len + 1
			)) != 0 ? size -= 1 : 0, L'\0'
		)
	{
		std::wmemcpy(&data[0], buffer, data.size());
	}
	~wstr_cast() {
		delete[] buffer;
	}
};

int main(int argc, char* argv[]) try {
	if (argc == 1) {
		return -1;
	}
	std::locale::global(std::locale(""));
	std::size_t count = static_cast<std::size_t>(argc);
	wstr_cast path(argv[1], std::strlen(argv[1]));
	archive.reopen_archive(path.data);
	for (std::size_t i = 2; i <= count - 1; i += 2) {
		wstr_cast arg(argv[i], std::strlen(argv[i]));
		std::wstring value_context = {};
		if (i < count - 1) {
			value_context = wstr_cast(argv[i + 1], std::strlen(argv[i + 1])).data;
		}
		hander(arg.data, value_context, i);
	}
}
catch (const std::wstring& why) {
	std::wcout << why << '\n';
	return -2;
}