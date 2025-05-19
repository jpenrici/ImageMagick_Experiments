#include "tool.hpp"

#include <print>

auto main(int argc, char **argv) -> int {

  // Test
  ProcessImage img;
  img.process_image("Resources/bonsai.png");
  img.export_csv("Resources/teste_1.csv", ";", false);

  std::println("{}", img.str());

  return 0;
}
