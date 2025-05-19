#include "tool.hpp"
#include "common.hpp"

#include <Magick++.h>

#include <filesystem>
#include <format>
#include <iostream>
#include <print>

ProcessImage::ProcessImage()
    : m_width(0), m_height(0), m_pixels({}), m_isRGBA(false), m_depth(0),
      m_path(""), m_status(false) {};

auto ProcessImage::process_image(std::string_view path) -> bool {

  m_status = false;
  m_isRGBA = false;

  if (path.empty()) {
    std::cerr << "Empty image path!\n";
    return false;
  }

  Magick::InitializeMagick(std::filesystem::current_path().c_str());
  Magick::Image image;

  std::vector<char> buffer;
  try {
    // Load
    buffer = Common::load(path);
  } catch (...) {
    std::cerr << "Error loading image.\n";
    return false;
  }

  if (!buffer.empty()) {
    Magick::Blob blob(buffer.data(), buffer.size());
    try {
      // Read
      image.read(blob);
      // Dimensions
      m_width = image.columns();
      m_height = image.rows();
      m_pixels.resize(m_width * m_height);
      // RGBA
      m_isRGBA = image.type() == Magick::ImageType::TrueColorMatteType;
      // Depth
      m_depth = image.depth();
      // Pixels
      Magick::Pixels pixel_view(image);
      Magick::PixelPacket *pixel_data = pixel_view.get(0, 0, m_width, m_height);
      if (pixel_data) {
        m_pixels.assign(pixel_data, pixel_data + m_width * m_height);
        pixel_view.sync();
      } else {
        std::cerr << "Error getting image pixels.\n";
        return false;
      }
    } catch (Magick::Exception &e) {
      std::cerr << "Error loading image from blob: " << e.what() << "\n";
      return false;
    }
  } else {
    std::cerr << "Error reading image file.\n";
    return false;
  }

  m_path = path;
  m_status = true;

  return true;
}

auto ProcessImage::str() -> std::string {
  size_t limit = m_width;
  std::span<Magick::Color> span1(m_pixels.begin(), limit);
  std::span<Magick::Color> span2(m_pixels.end() - limit, limit);

  std::string colors = "\n[ ";
  for (const auto &pixel : span1) {
    colors += std::format(
        "({}) ",
        Common::toStr(std::vector<unsigned>{
            (unsigned)pixel.redQuantum(), (unsigned)pixel.greenQuantum(),
            (unsigned)pixel.blueQuantum(),
            m_isRGBA ? (unsigned)pixel.alphaQuantum() : 255}));
  }
  colors += "\n ... ...\n ";
  for (const auto &pixel : span2) {
    colors += std::format(
        "({}) ",
        Common::toStr(std::vector<unsigned>{
            (unsigned)pixel.redQuantum(), (unsigned)pixel.greenQuantum(),
            (unsigned)pixel.blueQuantum(),
            m_isRGBA ? (unsigned)pixel.alphaQuantum() : 255}));
  }
  colors += " ]";

  std::string description = std::format(
      "Image: {}\nDimension: {} x {}\nRGBA: {}\nDepth: {}\nPixels: {}", m_path,
      m_width, m_height, (m_isRGBA ? "Yes" : "No"), m_depth, colors);

  return m_status ? description : "No image processed!";
}

auto ProcessImage::export_csv(std::string_view path, std::string delimiter,
                              bool hexadecimal) -> bool {

  auto toHex = [](unsigned value) -> std::string {
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(2) << value;
    return ss.str();
  };

  std::string colors{};
  for (size_t r = 0; r < m_height; r++) {
    for (size_t c = 0; c < m_width; c++) {
      auto pixel = m_pixels.at(r * m_width + c);
      auto R = (unsigned)pixel.redQuantum();
      auto G = (unsigned)pixel.greenQuantum();
      auto B = (unsigned)pixel.blueQuantum();
      auto A = m_isRGBA ? (unsigned)pixel.alphaQuantum() : 255;
      if (hexadecimal) {
        colors += std::format(
            "#{}{}",
            Common::toStr(std::vector<std::string>{toHex(R), toHex(G), toHex(B),
                                                   toHex(A)},
                          ""),
            delimiter);
      } else {
        colors += std::format(
            "{}{}", Common::toStr(std::vector<unsigned>{R, G, B, A}, ","),
            delimiter);
      }
    }
    colors += "\n";
  }

  return Common::save(path, std::string_view(colors));
}
