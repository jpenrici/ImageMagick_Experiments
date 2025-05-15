#include "tool.hpp"

#include <Magick++.h>

#include <fstream>
#include <print>

auto load_image(std::string_view path) -> std::vector<char> {

  try {
    std::ifstream file(path.data(), std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {
      return buffer;
    }
  } catch (...) {
    // error
  }

  return {};
}

auto process_image(std::string_view path, char **argv) -> int {
  Magick::InitializeMagick(*argv);
  Magick::Image image;

  auto buffer = load_image(path);
  if (!buffer.empty()) {
    Magick::Blob blob(buffer.data(), buffer.size());
    try {
      image.read(blob);
      std::println("Image loaded from blob successfully! Dimension: {} x {}",
                   image.columns(), image.rows());
    } catch (Magick::Exception &e) {
      std::println("Error loading image from blob: {}", e.what());
      return EXIT_FAILURE;
    }
  } else {
    std::println("Error reading image file.");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
