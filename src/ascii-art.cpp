#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <string>
#include <vector>
#include <utility>

class Font {
public:
    void load_font(const std::string& filename) {
        FT_Error error;

        error = FT_New_Face(
            library(), 
            filename.c_str(), 
            0, 
            &face);
        if (error == FT_Err_Unknown_File_Format) {
            std::cerr << "Unsupported font format" << std::endl;
        }
        else if (error) {
            std::cerr << "Could not open font file, error code 0x" << std::hex << error << std::endl;
        }

        // Select encoding
        error = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
        if (error) {
            std::cerr << "Could not select unicode encoding" << std::endl;
        }
    }

    void print_glyphs(const char32_t* str, int pixel_size) const {
        if (!face) {
            std::cerr << "Font not loaded" << std::endl;
            return;
        }

        FT_Error error;

        // Set font size
        error = FT_Set_Pixel_Sizes(
            face,                       /* handle to face object */
            0,                          /* pixel_width           */
            (unsigned int)pixel_size);  /* pixel_height          */
        if (error) {
            std::cerr << "Could not set font size" << std::endl;
        }

        // Print glyphs
        std::vector<std::vector<std::string>> bitmaps;
        for (const char32_t* p = str; *p; ++p) {
            unsigned long charcode = *p;

            FT_UInt glyph_index = FT_Get_Char_Index(face, charcode);
            error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
            if (error) {
                std::cerr << "Could not load glyph, charcode 0x" << std::hex << charcode << ", error code 0x" << std::hex << error << std::endl;
            }

            FT_GlyphSlot slot = face->glyph;
            if (slot->format != FT_GLYPH_FORMAT_BITMAP) {
                error = FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
                if (error) {
                    std::cerr << "Could not render glyph, error code 0x" << std::hex << error << std::endl;
                }
            }

            // print bitmap
            FT_Bitmap& bitmap = slot->bitmap;
            int rows = std::max(int(bitmap.rows), pixel_size);
            int cols = std::max(int(bitmap.width), pixel_size);

            std::vector<std::string> lines(rows, std::string(cols, ' '));
            int top = (rows - int(bitmap.rows)) / 2;
            int left = (cols - int(bitmap.width)) / 2;

            for (int y = 0; y < int(bitmap.rows); ++y) {
                for (int x = 0; x < int(bitmap.width); ++x) {
                    unsigned char pixel = bitmap.buffer[y * bitmap.pitch + x];
                    lines[y + top][x + left] = pixel > 0 ? '*' : ' ';
                }
            }
            bitmaps.emplace_back(std::move(lines));
        }

        // Print bitmaps
        for (int y = 0; y < pixel_size; ++y) {
            for (int i = 0; i < int(bitmaps.size()); ++i) {
                std::cout << bitmaps[i][y] << " ";
            }
            std::cout << std::endl;
        }
    }

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

int main() {
    Font font;
    font.load_font("resources/font/NotoSansSC-Regular.otf");
    font.print_glyphs(U"你好，世界！", 32);
    return 0;
}