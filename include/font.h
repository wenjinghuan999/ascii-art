#pragma once

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace aa {

// convert an uint8_t value to another
class Tuning {
public:
    enum class type {
        identity,
        binary,
        linear
    };
public:
    Tuning(type ty = type::identity, uint8_t li = 0, uint8_t lv = 0, uint8_t hi = 255, uint8_t hv = 255) {
        switch (ty) {
        case type::identity:
            li = 0; lv = 0; hi = 255; hv = 255;
            break;
        case type::binary:
            lv = 0; hi = li; hv = 255;
            break;
        case type::linear:
        default:
            break;
        }

        data_.resize(256);
        for (int i = 0; i < 256; ++i) {
            if (i <= li) {
                data_[i] = static_cast<uint8_t>(round(static_cast<double>(lv) * i / li));
            }
            else if (i > hi) {
                data_[i] = static_cast<uint8_t>(round(255.0 - static_cast<double>(255u - hv) * (255 - i) / (255u - hi)));
            }
            else {
                data_[i] = static_cast<uint8_t>(lv + (static_cast<double>(hv) - lv) * (static_cast<double>(i) - li) / (static_cast<double>(hi) - li));
            }
        }
    }

    Tuning(const Tuning&) = default;
    Tuning(Tuning&&) = default;
    Tuning& operator=(const Tuning&) = default;
    Tuning& operator=(Tuning&&) = default;

    uint8_t operator()(uint8_t value) const {
        return data_[value];
    }

private:
    std::vector<uint8_t> data_;
};

class Bitmap {
public:
    Bitmap(int width, int height);
    Bitmap(std::initializer_list<std::vector<uint8_t>>);

    Bitmap(const Bitmap&) = default;
    Bitmap(Bitmap&&) = default;
    Bitmap& operator=(const Bitmap&) = default;
    Bitmap& operator=(Bitmap&&) = default;

    int width() const { return width_; }
    int height() const { return height_; }
    uint8_t& operator()(int x, int y) {
        if (x < 0 || x >= width_ || y < 0 || y >= height_) {
            throw std::out_of_range("Bitmap::operator(): out of range");
        }
        return data_[y * pitch_ + x];
    }
    uint8_t operator()(int x, int y) const {
        return const_cast<Bitmap&>(*this)(x, y);
    }
    uint8_t get_value(int x, int y) const {
        if (x < 0 || x >= width_ || y < 0 || y >= height_) {
            return 0;
        }
        return data_[y * pitch_ + x];
    }

    void tune(const Tuning& tuning) {
        for (int i = 0; i < height_; ++i) {
            for (int j = 0; j < width_; ++j) {
                data_[i * pitch_ + j] = tuning(data_[i * pitch_ + j]);
            }
        }
    }

private:
    int width_{ 0 };
    int height_{ 0 };
    int pitch_{ 8 };
    std::vector<uint8_t> data_;
};

template <typename T>
class GlyphMapper {
public:
    GlyphMapper() {
        distance_function_ = [](const Bitmap& src, const Bitmap& dst) {
            double distance = 0.0;
            for (int i = 0; i < dst.height(); ++i) {
                for (int j = 0; j < dst.width(); ++j) {
                    distance += abs(static_cast<double>(src.get_value(i, j)) - dst(i, j));
                }
            }
            return distance;
        };
    }

public:
    void add_mapping(Bitmap bitmap, T value) {
        mapping_.emplace_back(std::move(bitmap), value);
    }
    void set_input_tuning(Tuning tuning) {
        input_tuning_ = std::move(tuning);
    }
    void set_distance_function(std::function<double(const Bitmap&, const Bitmap&)> distance_function) {
        distance_function_ = std::move(distance_function);
    }
    T operator()(const Bitmap& bitmap) const {
        Bitmap tunned_bitmap = bitmap;
        tunned_bitmap.tune(input_tuning_);
        std::vector<double> distances;
        distances.reserve(mapping_.size());
        std::transform(mapping_.begin(), mapping_.end(), std::back_inserter(distances), [&](const auto& mapping) {
            return distance_function_(tunned_bitmap, mapping.first);
        });
        auto min_it = std::min_element(distances.begin(), distances.end());
        if (min_it != distances.end()) {
            return mapping_[std::distance(distances.begin(), min_it)].second;
        }
        else {
            return static_cast<T>(0);
        }
    }

private:
    std::vector<std::pair<Bitmap, T>> mapping_;
    Tuning input_tuning_;
    std::function<double(const Bitmap&, const Bitmap&)> distance_function_;
};

class Font {
public:
    Font();
    ~Font();

public:
    void load_font(const std::string& filename);
    void print_glyphs(const char32_t* str, int pixel_size, const GlyphMapper<char>& mapper) const;
    void print_glyphs(const char32_t* str, int pixel_size) const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace aa
