#include "font.h"

int main() {
    aa::Font font;
    font.load_font("resources/font/NotoSansSC-Regular.otf");
    font.print_glyphs(U"你好，世界！", 32);
    return 0;
}
