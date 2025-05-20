#include "image_tool.hpp"

#include <print>

auto main(int argc, char **argv) -> int {

  // Test
  ProcessImage img;
  if (img.process_image("Resources/bonsai.png")) {
    img.export_csv("Resources/test_decimal.csv",
                   ProcessImage::ExportConfig{";", false, true});
    img.export_csv("Resources/test_hexadecimal.csv",
                   ProcessImage::ExportConfig{";", true, false});
    std::println("{}", img.str());
  } else {
    std::println("Unable to process image!");
  }

  return 0;
}
