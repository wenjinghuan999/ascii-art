#include <argparse/argparse.hpp>
#include <utf8.h>

#include "font.h"

#include <iostream>
#include <fstream>

int main(int argc, const char* argv[]) {
    argparse::ArgumentParser program("ascii-art");
    program.add_argument("text")
        .help("text to print")
        .default_value(std::string("Hello!"));
    program.add_argument("-f", "--textfile")
        .help("text file to print");
    program.add_argument("--font")
        .help("font file name")
        .default_value(std::string("resources/font/NotoSansSC-Regular.otf"));
    
    try {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        exit(EXIT_FAILURE);
    }

    auto font_name = program.get<std::string>("--font");
    auto text = program.get<std::string>("text");
    std::u32string u32text;
    if (program.present("--textfile")) {
        std::ifstream fin(program.get<std::string>("--textfile"));
        if (!fin) {
            std::cerr << "Could not open text file" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::stringstream ss;
        ss << fin.rdbuf();
        text = ss.str();

        auto end_it = utf8::find_invalid(text.begin(), text.end());
        if (end_it != text.end()) {
            std::cerr << "Invalid UTF-8 encoding detected at byte " << (end_it - text.begin()) << "\n";
            utf8::replace_invalid(text.begin(), text.end(), back_inserter(text));
        }

        utf8::utf8to32(text.begin(), text.end(), back_inserter(u32text));
    }
    else {
        u32text = std::u32string(text.begin(), text.end());
    }

    aa::GlyphMapper<char> mapper;
    mapper.add_mapping({{ 0 }}, ' ');
    mapper.add_mapping({{ 63 }}, '+');
    mapper.add_mapping({{ 127 }}, '*');
    mapper.add_mapping({{ 255 }}, '#');

    aa::Font font;
    font.load_font(font_name);
    font.print_glyphs(u32text.c_str(), 32, mapper);
    return 0;
}
