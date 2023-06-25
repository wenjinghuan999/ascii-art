#include <ft2build.h>
#include FT_FREETYPE_H

#include "font.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

namespace aa {

Bitmap::Bitmap(int width, int height) : width_(width), height_(height) {
    assert(width_ > 0 && height_ > 0);
    pitch_ = (width_ + 7) / 8 * 8;
    data_.resize(pitch_ * height_);
}

Bitmap::Bitmap(std::initializer_list<std::vector<uint8_t>> init) {
    assert(init.size() > 0);
    width_ = static_cast<int>(init.begin()->size());
    height_ = static_cast<int>(init.size());
    pitch_ = (width_ + 7) / 8 * 8;
    data_.resize(pitch_ * height_);

    int y = 0;
    for (const std::vector<uint8_t>& row : init) {
        assert(static_cast<int>(row.size()) == width_);
        std::copy(row.begin(), row.end(), data_.begin() + y * pitch_);
        ++y;
    }
}

class Font::Impl {
    friend class Font;

private:
    FT_Face face = nullptr;

private:
    static FT_Library library() {
        static FT_Library ft_library = nullptr;
        if (!ft_library) {
            FT_Error error = FT_Init_FreeType(&ft_library);
            if (error) {
                std::cerr << "Could not init freetype library" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        return ft_library;
    }
};


Font::Font() : impl_(std::make_unique<Impl>()) {
}

Font::~Font() {
}

void Font::load_font(const std::string& filename) {
    FT_Error error;

    error = FT_New_Face(
        impl_->library(), 
        filename.c_str(), 
        0, 
        &impl_->face);
    if (error == FT_Err_Unknown_File_Format) {
        std::cerr << "Unsupported font format" << std::endl;
    }
    else if (error) {
        std::cerr << "Could not open font file, error code 0x" << std::hex << error << std::endl;
    }

    // Select encoding
    error = FT_Select_Charmap(impl_->face, FT_ENCODING_UNICODE);
    if (error) {
        std::cerr << "Could not select unicode encoding" << std::endl;
    }
}

void Font::print_glyphs(const char32_t* str, int pixel_size, const GlyphMapper<char>& mapper) const {
    if (!impl_->face) {
        std::cerr << "Font not loaded" << std::endl;
        return;
    }

    FT_Error error;

    // Set font size
    error = FT_Set_Pixel_Sizes(
        impl_->face,                /* handle to face object */
        0,                          /* pixel_width           */
        (unsigned int)pixel_size);  /* pixel_height          */
    if (error) {
        std::cerr << "Could not set font size" << std::endl;
    }

    // Print glyphs
    std::vector<std::vector<std::string>> bitmaps;
    for (const char32_t* p = str; *p; ++p) {
        unsigned long charcode = *p;

        FT_UInt glyph_index = FT_Get_Char_Index(impl_->face, charcode);
        error = FT_Load_Glyph(impl_->face, glyph_index, FT_LOAD_DEFAULT);
        if (error) {
            std::cerr << "Could not load glyph, charcode 0x" << std::hex << charcode << ", error code 0x" << std::hex << error << std::endl;
        }

        FT_GlyphSlot slot = impl_->face->glyph;
        if (slot->format != FT_GLYPH_FORMAT_BITMAP) {
            error = FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
            if (error) {
                std::cerr << "Could not render glyph, error code 0x" << std::hex << error << std::endl;
            }
        }

        // print bitmap
        // note: we set glyph origin to the bottom-left corner of a pixel_size x pixel_size area
        FT_Bitmap& bitmap = slot->bitmap;
        int top = pixel_size - slot->bitmap_top;
        int left = slot->bitmap_left;
        int rows = std::max(top + int(bitmap.rows), pixel_size);
        int cols = std::max(left + int(bitmap.width), pixel_size);

        std::vector<std::string> lines(rows, std::string(cols, ' '));

        for (int y = 0; y < int(bitmap.rows); ++y) {
            for (int x = 0; x < int(bitmap.width); ++x) {
                uint8_t pixel = bitmap.buffer[y * bitmap.pitch + x];
                Bitmap bitmap{ { pixel } };
                lines[y + top][x + left] = mapper(bitmap);
            }
        }
        bitmaps.emplace_back(std::move(lines));
    }

    // Print bitmaps
    int rows = std::accumulate(bitmaps.begin(), bitmaps.end(), 0, [](int max_rows, const std::vector<std::string> &bitmap) {
        return std::max(max_rows, int(bitmap.size()));
    });
    for (int y = 0; y < rows; ++y) {
        for (int i = 0; i < int(bitmaps.size()); ++i) {
            if (y < bitmaps[i].size()) {
                std::cout << bitmaps[i][y] << " ";
            }
            else {
                std::cout << std::string(bitmaps[i][0].size(), ' ') << " ";
            }
        }
        std::cout << std::endl;
    }
}

void Font::print_glyphs(const char32_t* str, int pixel_size) const {
    GlyphMapper<char> mapper;
    mapper.set_input_tuning(Tuning(Tuning::type::binary, 0u));
    mapper.add_mapping({{ 0 }}, ' ');
    mapper.add_mapping({{ 255 }}, '*');

    print_glyphs(str, pixel_size, mapper);
}


} // namespace aa
