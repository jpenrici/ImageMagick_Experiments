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

    struct ExportConfig {
        std::string delimiter = ";";
        bool hexadecimal = false;
        bool includeAlpha = true;
    };

    auto process_image(const std::string_view path) -> bool;
    auto export_csv(const std::string_view path, const ExportConfig& config) -> bool;
    auto str() -> std::string;

private:
    // Current image
    bool m_isRGBA;
    size_t m_width;
    size_t m_height;
    size_t m_depth;
    size_t m_quantumRange;
    std::vector<unsigned> m_pixels;

    std::string_view m_path;
    bool m_status;

    // Check ImageMagick version
    auto checkQuantumRange() -> size_t;

    static constexpr size_t Q8_RANGE = 255;
    static constexpr size_t Q16_RANGE = 65535;
    static constexpr size_t Q32_RANGE = 4294967295;
};
