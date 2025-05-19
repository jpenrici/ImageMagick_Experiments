#include "common.hpp"

#include <fstream>
#include <iostream>
#include <string_view>

auto Common::load(std::string_view path) -> std::vector<char> {

  try {
    std::ifstream file(path.data(), std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {
      return buffer;
    }
  } catch (const std::exception &e) {
    std::cerr << "Error open file: " << path << "\n";
    std::cerr << "Error: " << e.what() << "\n";
  }

  return {};
}

auto Common::save(std::string_view path, std::string_view text) -> bool {
  try {
    std::ofstream outFile(std::string(path),
                          std::ofstream::out | std::ofstream::trunc);
    if (outFile.is_open()) {
      outFile << text;
    }
    outFile.close();
    return true;
  } catch (const std::exception &e) {
    std::cerr << "Error saving file: " << path;
    std::cerr << "Error: " << e.what();
  }

  return false;
}
