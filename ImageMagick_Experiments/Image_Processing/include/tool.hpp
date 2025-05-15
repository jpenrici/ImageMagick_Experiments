/*
 * References:
 *  https://imagemagick.org/Magick++/Documentation.html
*/
#pragma once

#include <Magick++/Color.h>

#include <string_view>
#include <vector>

class ProcessImage {

public:
    ProcessImage();
    virtual ~ProcessImage() = default;

    auto process_image(std::string_view path) -> int;
    auto str() -> std::string;

private:
    // Current image
    size_t m_width;
    size_t m_height;
    std::vector<Magick::Color> m_pixels;

    std::string_view m_path;
    bool m_status;

    auto load_image(std::string_view path) -> std::vector<char>;

};
