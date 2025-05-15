/*
 * References:
 *  https://imagemagick.org/Magick++/Documentation.html
*/
#pragma once

#include <string_view>
#include <vector>

auto load_image(std::string_view path) -> std::vector<char>;

auto process_image(std::string_view path, char **argv) -> int;
