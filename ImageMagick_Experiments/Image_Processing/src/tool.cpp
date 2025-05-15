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

  struct FreePixelPacket {
    void operator()(Magick::PixelPacket *pixelPacket) {
      if (pixelPacket) {
        delete pixelPacket;
      }
    }
  };

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
      // std::unique_ptr<Magick::PixelPacket, FreePixelPacket> pixel_data(
      //     pixel_view.get(0, 0, m_width, m_height));
      if (pixel_data) {
        for (size_t i = 0; i < m_width * m_height; ++i) {
          // m_pixels[i] = pixel_data.get()[i];
          m_pixels[i] = pixel_data[i];
        }
        // Free Pixels
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
  std::string colors = "";
  // for (const auto &pixel : m_pixels) {
  //   colors += std::format("({}, {}, {})", pixel.redQuantum(),
  //                         pixel.greenQuantum(), pixel.blueQuantum());
  // }
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
