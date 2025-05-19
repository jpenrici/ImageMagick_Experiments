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

    auto process_image(std::string_view path) -> bool;
    auto export_csv(std::string_view path, std::string delimiter, bool hexadecimal) -> bool;
    auto str() -> std::string;

private:
    // Current image
    bool m_isRGBA;
    size_t m_width;
    size_t m_height;
    size_t m_depth;
    std::vector<Magick::Color> m_pixels;

    std::string_view m_path;
    bool m_status;
};
