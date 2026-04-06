#pragma once
#include <WinUILib.h>
#include <string>
#include <vector>
#include <cstddef>
#include <sstream>
#include <unordered_map>

#include "SystemClipboard.h"

using namespace winrt;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using winrt::Windows::Foundation::IAsyncAction;
using winrt::Windows::Foundation::IAsyncOperation;

template <class VectorType>
struct view_data {
    using vector_type = VectorType;
    vector_type& data;
    constexpr view_data(vector_type& data) : data(data) {};
    template <class FunType>
    void operator|(FunType&& fun) {
        std::size_t count = 0;
        for (; count < data.size(); count++) {
            fun(data[count], static_cast<std::size_t>(count + 1));
        }
    }

    template <class FunType>
    IAsyncAction operator^(FunType&& fun) {
        std::size_t count = 0;
        for (; count < data.size(); count++) {
            co_await fun(data[count], static_cast<std::size_t>(count + 1));
        }
    }
};

inline constexpr const char* err_table[] = {
    "The open application is unsuccessful, file path: ",
    "Out of range, index: ",
    "Can't found: ",
    "Can't write, file path: ",
    "Found duplicate file: "
};

class archive {

private:
    using str_type           = winrt::hstring;
    using vector_type        = std::vector<str_type>;
    using istringstream_type = std::wistringstream;
    using storage_file_type  = IStorageFile;

private:

    storage_file_type  file;
    vector_type        data;
    vector_type        need_delete;

private:

    std::size_t search_content(str_type context, std::size_t bufline) noexcept {
        std::size_t count = 0;
        istringstream_type wiss(context.c_str());
        std::wstring line;
        while (std::getline(wiss, line, L'\n')) {
            if (line.empty()) {
                continue;
            }

            if (line.back() == L'\r')
                line.pop_back();

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

    void save_buffer(std::size_t count, std::size_t bufline) noexcept {
        if (!count) {
            data.resize(0);
            return;
        }
        data.resize(count);
    }

    IAsyncAction open_archive(str_type context, std::size_t bufline = 5) {
        std::size_t search_count = search_content(context, bufline);
        save_buffer(search_count, bufline);
        co_return;
    }

    [[noreturn]]
    void throw_why(const char* err, str_type txt) {
        std::wstring why = winrt::to_hstring(err).c_str();
        why += txt.c_str();
        throw winrt::hstring(why);
    }

    std::size_t find_dup(vector_type& vec, str_type file, std::size_t size) noexcept {
        for (std::size_t i = 0; i < size; ++i) {
            if (vec[i] == file) {
                return i;
            }
        }
        return vec.size();
    }

    IAsyncOperation<str_type> take_impl(str_type file) {
        std::size_t size = data.size();
        std::size_t index = find_dup(data, file, size);
        if (index >= size) {
            istringstream_type iss(file.c_str());
            if (bool resu = bool(iss >> index); resu) {
                if (index > size) {
                    throw_why(err_table[1], file);
                }
            }
            else {
                throw_why(err_table[2], file);
            }
        }
        str_type retval = data[index];
        remove_element(data, index);
        co_return retval;
    }

    void save_impl(str_type const& file) {
        std::size_t size = data.size();
        std::size_t index = find_dup(data, file, size);
        if (index < size) {
            return;
        }
        data.push_back(file);
    }

public:

    archive(std::size_t bufline = 0)
        : data(bufline), file(nullptr), need_delete()
    {}

    archive(const storage_file_type& file, std::size_t bufline = 5)
        : data(bufline), file(file), need_delete()
    {}

    IAsyncAction init(std::size_t bufline = 5) {
        hstring file_content = co_await FileIO::ReadTextAsync(this->file);
        co_await open_archive(file_content, bufline);
    }

    void clear() noexcept {
        if (data.empty()) {
            return;
        }
        data.clear();
    }

    template <class VectorType>
    static void remove_element(VectorType& vec, std::size_t index) noexcept {
        std::size_t size = vec.size();
        size > 0 ? size -= 1 : size;
        if (size == 0) {
            vec.resize(0);
            return;
        }
        for (; index < size; ++index) {
            vec[index] = vec[index + 1];
        }
        vec.resize(size);
    }

    IAsyncAction reopen_archive(const storage_file_type& file, std::size_t bufline = 5) {
        this->file = file;
        hstring file_content = co_await FileIO::ReadTextAsync(this->file);
        data.clear();
        data.resize(bufline);
        need_delete.clear();
        co_await open_archive(file_content, bufline);
    }

    bool find_dup(str_type file) noexcept {
        std::size_t size = data.size();
        for (std::size_t i = 0; i < size; ++i) {
            if (data[i] == file) {
                return true;
            }
        }
        return false;
    }

    std::size_t get(str_type file) noexcept {
        std::size_t size = data.size();
        for (std::size_t i = 0; i < size; ++i) {
            std::wstring wdata = data[i].c_str();
            if (wdata.find(file) != std::wstring::npos) {
                return i;
            }
        }
        return std::wstring::npos;
    }

    void add_to_deletes(str_type elem) {
        std::size_t size  = need_delete.size();
        std::size_t index = find_dup(need_delete, elem, size);
        if (index < size) {
            need_delete[index] = elem;
            return;
        }
        need_delete.emplace_back(elem);
    }

    void remove_elem_from_deletes(str_type elem) {
        for (std::size_t i = 0; i < need_delete.size(); i++) {
            for (std::size_t j = i + 1; j < need_delete.size(); j++) {
                if (need_delete[i] == elem) {
                    remove_element(need_delete, i);
                }
            }
        }
    }

    void clear_deletes() {
        if (need_delete.empty()) {
            return;
        }
        need_delete.clear();
    }

    bool must_delete() {
        return !need_delete.empty();
    }

    const auto file_info() const {
        return file;
    }

    str_type file_name() {
        return file.Name();
    }

    IAsyncOperation<str_type> take_out(str_type file) {
        auto resu = co_await take_impl(file);
        co_await sys_clipboard::save(resu);
        co_return resu;
    }

    IAsyncOperation<str_type> take(str_type file) {
        co_return co_await take_impl(file);
    }

    template <class FunType>
    IAsyncAction take_deletes(FunType&& handler) {
        std::size_t count = 0;
        for (auto& elem : need_delete) {
            co_await take_out(elem);
            co_await handler(elem);
            ++count;
        }
        need_delete.resize(need_delete.size() - count);
        co_return;
    }

     const auto data_info() const {
         return data;
     }

    std::size_t lastsub() {
        std::size_t size = data.size();
        return size > 0 ? size - 1 : 0;
    }

    IAsyncOperation<bool> save(str_type file) {
        std::wstring record;
        vector_type files;
        for (auto& ch : file) {
            if (ch == '\r') {
                files.emplace_back(record);
                record.resize(0);
                continue;
            }
            record += ch;
        }
        if (files.size()) {
            if (record.size()) {
                files.emplace_back(record);
            }
            for (auto& f : files) {
                save_impl(f);
            }
        }
        else {
            save_impl(file);
        }
        
        co_return true;
    }

    bool remove_dup() noexcept {
        bool poped = false;
        for (std::size_t i = 0; i < data.size(); i++) {
            for (std::size_t j = i + 1; j < data.size(); j++) {
                if (data[i] == data[j]) {
                    remove_element(data, j);
                    poped = true;
                }
            }
        }
        return poped;
    }

    view_data<vector_type> view() noexcept {
        return data;
    }

    IAsyncAction update() {
        std::wostringstream woss;
        for (auto& elem : data) {
            woss << elem;
            woss << L"\n";
        }
        co_await FileIO::WriteTextAsync(file, woss.str());
    }

    operator bool() {
        return !data.empty();
    }

    winrt::hstring& operator[](std::size_t position) {
        return data[position];
    }

    const winrt::hstring& operator[](std::size_t position) const {
        return data[position];
    }

public:

    ~archive() {};

};