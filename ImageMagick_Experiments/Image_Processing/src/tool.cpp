#include "tool.hpp"

#include <Magick++.h>

#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <print>

ProcessImage::ProcessImage()
    : m_width(0), m_height(0), m_pixels({}), m_path(""), m_status(false) {};

auto ProcessImage::process_image(std::string_view path) -> int {

  m_status = false;

  if (path.empty()) {
    std::println("Empty image path!");
    return EXIT_FAILURE;
  }

  auto currentPath = std::filesystem::current_path();

  Magick::InitializeMagick(currentPath.c_str());
  Magick::Image image;

  auto buffer = load_image(path);
  if (!buffer.empty()) {
    Magick::Blob blob(buffer.data(), buffer.size());
    try {
      // Load
      image.read(blob);
      // Dimensions
      m_width = image.columns();
      m_height = image.rows();
      m_pixels.resize(m_width * m_height);
      // Pixels
      Magick::Pixels pixel_view(image);
      Magick::PixelPacket *pixel_data = pixel_view.get(0, 0, m_width, m_height);
      if (pixel_data) {
        m_pixels.assign(pixel_data, pixel_data + m_width * m_height);
        pixel_view.sync();
      } else {
        std::println("Error getting image pixels.");
        return EXIT_FAILURE;
      }
    } catch (Magick::Exception &e) {
      std::println("Error loading image from blob: {}", e.what());
      return EXIT_FAILURE;
    }
  } else {
    std::println("Error reading image file.");
    return EXIT_FAILURE;
  }

  m_status = true;

  return EXIT_SUCCESS;
}

auto ProcessImage::str() -> std::string {
  size_t limit = 10;
  std::span<Magick::Color> span1(m_pixels.begin(), limit);
  std::span<Magick::Color> span2(m_pixels.end() - limit, limit);

  std::string colors = "\n[ ";
  for (const auto &pixel : span1) {
    colors += std::format("({}, {}, {}) ", pixel.redQuantum(),
                          pixel.greenQuantum(), pixel.blueQuantum());
  }
  colors += "\n ... ...\n ";
  for (const auto &pixel : span2) {
    colors += std::format("({}, {}, {}) ", pixel.redQuantum(),
                          pixel.greenQuantum(), pixel.blueQuantum());
  }
  colors += " ]";
  std::string description =
      std::format("Image: {}\nDimension: {} x {}\nPixels: {}", m_path, m_width,
                  m_height, colors);

  return m_status ? description : "No image processed!";
}

auto ProcessImage::load_image(std::string_view path) -> std::vector<char> {

  try {
    std::ifstream file(path.data(), std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {
      m_path = path;
      return buffer;
    }
  } catch (...) {
    // error
  }

  return {};
}
