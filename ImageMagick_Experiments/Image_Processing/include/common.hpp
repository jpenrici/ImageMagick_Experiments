#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace Common {

auto load(std::string_view path) -> std::vector<char>;
auto save(std::string_view path, std::string_view text) -> bool;

template <typename T>
auto toStr(std::vector<T> vec, std::string delimiter = ",") -> std::string {
    std::string out{};
    for (size_t i = 0; i < vec.size(); i++) {
        try {
            out += std::format("{}{}", vec.at(i), (i < vec.size() - 1 ? delimiter : ""));
        } catch (const std::exception &e) {
            std::cerr << "Error Tool: Could not convert data to string!\n";
            std::cerr << "Error: " << e.what() << "\n";
            return {};
        }
    }
    return out;
}

}
