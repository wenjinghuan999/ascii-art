#pragma once

#include <memory>
#include <string>

namespace aa {
    
class Font {
public:
    Font();
    ~Font();

public:
    void load_font(const std::string& filename);
    void print_glyphs(const char32_t* str, int pixel_size) const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace aa
