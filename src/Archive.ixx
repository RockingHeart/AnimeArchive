export module archive;

import <vector>;
import <fstream>;
import <cstddef>;
import <string>;

import <print>;

import sys_clipboard;

export template <class VectorType>
struct view_data {
	using vector_type = VectorType;
	vector_type& data;
	constexpr view_data(vector_type& data) : data(data) {};
	template <class FunType>
	constexpr void operator|(FunType&& fun) noexcept {
		static std::size_t count = 1;
		while (count < data.size()) {
			// Why did static_cast? For limit the parameter type to std::size_t
			fun(data[count++], static_cast<std::size_t>(count));
		}
	}
};

extern constexpr const char* err_table[] = {
	"The open application is unsuccessful, file path: ",
	"Out of range, index: ",
	"Can't found: ",
	"Can't write, file path: ",
	"Found duplicate file: "
};

export template <class DataType>
class archive {

private:
	using data_type			 =	  DataType;
	using string_type		 = std::basic_string<data_type>;
	using vector_type		 = std::vector<string_type>;
	using fstream_type		 = std::basic_fstream<data_type>;
	using sys_clipboard_type = sys_clipboard<data_type>;
	using istringstream_type = std::basic_istringstream<data_type>;

private:
	string_type  path;
	fstream_type stream;
	vector_type  data;

private:

	constexpr std::size_t search_content(std::size_t bufline) noexcept {
		string_type line;
		std::size_t count = 0;
		while (std::getline(stream, line)) {
			if (line.empty()) {
				continue;
			}
			if (count < bufline) {
				data[count] = line;
			}
			else {
				data.emplace_back(line);
			}
			count += 1;
		}
		return count;
	}

	constexpr void save_buffer(std::size_t count, std::size_t bufline) noexcept {
		if (!count) {
			data.resize(0);
			return;
		}
		data.resize(count);
	}

	constexpr void pop_element(std::size_t index) noexcept {
		std::size_t size = data.size() - 1;
		for (; index < size; ++index) {
			data[index] = data[index + 1];
		}
		data.resize(size);
	}

	constexpr auto format_str(std::string_view str) noexcept {
		if constexpr (std::is_same_v<data_type, wchar_t>) {
			static std::wstring buffer;
			buffer = std::wstring(str.begin(), str.end());
			return buffer.c_str();
		}
		else {
			return str.data();
		}
	}

	constexpr void open_archive(const string_type& file_path, std::size_t bufline = 5) {
		stream.open(file_path, std::ios::in | std::ios::out);
		if (!stream.is_open()) {
			string_type why = format_str(err_table[0]);
			why += file_path;
			throw why;
		}

		stream.imbue(std::locale());

		std::size_t search_count = search_content(bufline);
		save_buffer(search_count, bufline);
	}

	[[noreturn]]
	constexpr void throw_why(const char* err, const string_type& txt) {
		string_type why = format_str(err);
		why += txt;
		throw why;
	}

public:

	constexpr archive()
		noexcept {}

	constexpr archive(const string_type& file_path, std::size_t bufline = 5)
		: path(file_path), data(bufline)
	{
		open_archive(file_path, bufline);
	}

	constexpr void clear() noexcept {
		if (data.empty()) {
			return;
		}
		data.resize(0);
	}

	constexpr void reopen_archive(const string_type& file_path, std::size_t bufline = 5) {
		path = string_type(file_path);
		data = vector_type(bufline);
		open_archive(file_path, bufline);
	}

	constexpr std::size_t find_dup(const string_type& file, std::size_t size) noexcept {
		std::size_t index = 1;
		for (std::size_t i = 0; i < size; ++i) {
			if (data[i] == file) {
				break;
			}
			++index;
		}
		return index;
	}

	constexpr string_type take(const string_type& file) {
		std::size_t size  = data.size();
		std::size_t index = find_dup(file, size);
		if (index > size) {
			istringstream_type iss(file);
			if (bool resu = bool(iss >> index); resu) {
				if (index > size) {
					throw_why(err_table[1], file);
				}
			}
			else {
				throw_why(err_table[2], file);
			}
		}
		index != 0 ? --index : 0;
		sys_clipboard_type clipboard;
		string_type retval = data[index];
		clipboard.save(data[index]);
		pop_element(index);
		return retval;
	}

	constexpr void save(const string_type& file) {
		std::size_t size  = data.size();
		std::size_t index = find_dup(file, size);
		if (index < size) {
			throw_why(err_table[4], file);
		}
		data.push_back(file);
	}

	constexpr bool remove_dup() noexcept {
		if (!stream.is_open()) {
			return false;
		}
		bool poped = false;
		for (std::size_t i = 0; i < data.size(); i++) {
			for (std::size_t j = i + 1; j < data.size(); j++) {
				if (data[i] == data[j]) {
					pop_element(j);
					poped = true;
				}
			}
		}
		return poped;
	}

	constexpr view_data<vector_type> view() noexcept {
		return data;
	}

	constexpr void update() {
		if (stream.is_open()) {
			stream.close();
		}
		stream.open(path, std::ios::out);
		if (!stream.is_open()) {
			throw_why(err_table[1], path);
		}
		std::size_t size = data.size();
		for (std::size_t i = 0; i < size; i++) {
			stream << data[i] << '\n';
		}
		stream.flush();
		stream.close();
		if (stream.fail()) {
			throw_why(err_table[3], path);
		}
	}

public:

	constexpr ~archive() {
		if (stream.is_open()) {
			stream.close();
		}
	}

};