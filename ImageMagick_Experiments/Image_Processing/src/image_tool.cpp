#include "image_tool.hpp"
#include "common.hpp"

#include <Magick++.h>

#include <filesystem>
#include <iostream>
#include <print>

ProcessImage::ProcessImage()
    : m_width(0), m_height(0), m_pixels({}), m_isRGBA(false), m_depth(0),
      m_path(""), m_status(false) {
  m_quantumRange = checkQuantumRange();
};

auto ProcessImage::process_image(const std::string_view path) -> bool {

  if (m_quantumRange < 8) {
    std::cerr << "Unable to properly verify ImageMagick. This algorithm "
                 "requires a QuantumRange value to proceed!\n";
    return false;
  }

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
      // RGBA
      m_isRGBA = image.type() == Magick::ImageType::TrueColorMatteType;
      // Depth
      m_depth = image.depth();
      // Pixels
      m_pixels.clear();
      for (size_t r = 0; r < m_height; ++r) {
        for (size_t c = 0; c < m_width; ++c) {
          auto pixel = image.pixelColor(c, r);
          // Map channel values
          // ​​from the range [0, quantumRange] to [0, 255].
          auto R =
              static_cast<unsigned>(pixel.redQuantum() * 255 / m_quantumRange);
          auto G = static_cast<unsigned>(pixel.greenQuantum() * 255 /
                                         m_quantumRange);
          auto B =
              static_cast<unsigned>(pixel.blueQuantum() * 255 / m_quantumRange);
          auto A = static_cast<unsigned>(pixel.alphaQuantum() * 255 /
                                         m_quantumRange);
          m_pixels.push_back(R);
          m_pixels.push_back(G);
          m_pixels.push_back(B);
          m_pixels.push_back(m_isRGBA ? A : 255);
        }
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
  std::span<unsigned> span1(m_pixels.begin(), limit * 4);
  std::span<unsigned> span2(m_pixels.end() - limit * 4, limit * 4);

  std::string colors = "\n[ ";
  for (size_t i = 0; i < span1.size(); i += 4) {
    colors += std::format(
        "({}) ", Common::toStr(std::vector<unsigned>{
                     span1[i + 0], span1[i + 1], span1[i + 2], span1[i + 3]}));
  }
  colors += "\n ... ...\n ";
  for (size_t i = 0; i < span2.size(); i += 4) {
    colors += std::format(
        "({}) ", Common::toStr(std::vector<unsigned>{
                     span2[i + 0], span2[i + 1], span2[i + 2], span2[i + 3]}));
  }
  colors += " ]";

  std::string description = std::format(
      "Image: {}\nDimension: {} x {}\nRGBA: {}\nDepth: {}\nPixels: {}", m_path,
      m_width, m_height, (m_isRGBA ? "Yes" : "No"), m_depth, colors);

  return m_status ? description : "No image processed!";
}

auto ProcessImage::export_csv(const std::string_view path,
                              const ExportConfig &config) -> bool {

  if (path.empty()) {
    std::cerr << "Invalid file path or name!\n";
    return false;
  }

  if (!m_status) {
    std::cerr << "Image has not been processed!\n";
    return false;
  }

  auto toHex = [](unsigned value) -> std::string {
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(2) << value;
    return ss.str();
  };

  std::string colors{};
  int i = 0;
  for (size_t r = 0; r < m_height; r++) {
    for (size_t c = 0; c < m_width; c++) {
      auto R = m_pixels[i++]; // red
      auto G = m_pixels[i++]; // green
      auto B = m_pixels[i++]; // blue
      auto A = m_pixels[i++]; // alpha

      if (config.hexadecimal) {
        colors += std::format(
            "#{}{}{}",
            Common::toStr(
                std::vector<std::string>{toHex(R), toHex(G), toHex(B)}, ""),
            config.includeAlpha ? toHex(A) : "", config.delimiter);
      } else {
        colors +=
            std::format("{}{}{}", Common::toStr(std::vector<unsigned>{R, G, B}),
                        config.includeAlpha ? "," + std::to_string(A) : "",
                        config.delimiter);
      }
    }
    colors += "\n";
  }

  return Common::save(path, std::string_view(colors));
}

auto ProcessImage::checkQuantumRange() -> size_t {
  std::string versionInfo;

  using Pipe = std::unique_ptr<FILE, int (*)(FILE *)>;
  Pipe pipe(popen("identify -version", "r"), pclose);

  if (!pipe)
    throw std::runtime_error("popen failed!");

  char buffer[128];
  while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
    versionInfo += buffer;
  }
  std::size_t pos = versionInfo.find("Q");
  if (pos != std::string::npos && std::isdigit(versionInfo[pos + 1])) {
    auto q = versionInfo.substr(pos, 3);
    if (q == "Q8") {
      return Q8_RANGE;
    }
    if (q == "Q16") {
      return Q16_RANGE;
    }
    if (q == "Q32") {
      return Q32_RANGE;
    }
  }

  return 0;
}
