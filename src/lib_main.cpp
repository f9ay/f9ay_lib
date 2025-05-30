#include "f9ay.h"
#include "png.hpp"
#include "bmp.hpp"
#include "jpeg.hpp"
#include "filesystem"
#include "importer.hpp"

static std::variant<f9ay::Bmp, f9ay::Jpeg, f9ay::PNG> selectImporter(const std::byte *data) {
    if (data[0] == std::byte{0x89} && data[1] == std::byte{0x50} && data[2] == std::byte{0x4E} &&
        data[3] == std::byte{0x47}) {
        return f9ay::PNG();
    }
    if (data[0] == std::byte{0xFFu} && data[1] == std::byte{0xD8u}) {
        return f9ay::Jpeg();
    }
    if (data[0] == std::byte{0x42u} && data[1] == std::byte{0x4Du}) {
        return f9ay::Bmp();
    }
    throw std::runtime_error("Unsupported image format");
}

template<typename T>
static int f9ay_export(const char *filename, const char *data, int row, int col, int channels) {
    try{
        std::unique_ptr<std::byte[]> buffer;
        size_t size;
        if (channels == 3) {  // BGR
            auto color_ptr = (f9ay::colors::BGR *)(void *)(data);
            f9ay::Matrix<f9ay::colors::BGR> mtx(color_ptr, row, col);
            std::tie(buffer, size) = T::exportToByte(mtx);
        } else if (channels == 4) {  // BGRA
            auto color_ptr = (f9ay::colors::BGRA *)(void *)(data);
            f9ay::Matrix<f9ay::colors::BGRA> mtx(color_ptr, row, col);
            std::tie(buffer, size) = T::exportToByte(mtx);
        }
        std::filesystem::path path(filename);
        std::ofstream out(path, std::ios::binary);
        if(!out.is_open()) {
            return -1;
        }
        out.write(reinterpret_cast<const char *>(buffer.get()), size);
    } catch (...) {
        return -1;
    }
    return 0;
}

static int f9ay_png_export_internal(const char *filename, const char *data, int row, int col, int channels) {
    try{
        std::unique_ptr<std::byte[]> buffer;
        size_t size;
        if (channels == 3) {  // BGR
            auto color_ptr = (f9ay::colors::BGR *)(void *)(data);
            f9ay::Matrix<f9ay::colors::BGR> mtx(color_ptr, row, col);
            auto rgb_mtx = mtx.trans_convert([](const auto &c) {
                return f9ay::colors::color_cast<f9ay::colors::RGB>(c);
            });
            std::tie(buffer, size) = f9ay::PNG::exportToByte(rgb_mtx, FilterType::Paeth);
        } else if (channels == 4) {  // BGRA
            auto color_ptr = (f9ay::colors::BGRA *)(void *)(data);
            f9ay::Matrix<f9ay::colors::BGRA> mtx(color_ptr, row, col);
            auto rgba_mtx = mtx.trans_convert([](const auto &c) {
                return f9ay::colors::color_cast<f9ay::colors::RGBA>(c);
            });
            std::tie(buffer, size) = f9ay::PNG::exportToByte(rgba_mtx, FilterType::Paeth);
        }
        std::filesystem::path path(filename);
        std::ofstream out(path, std::ios::binary);
        if(!out.is_open()) {
            return -1;
        }
        out.write(reinterpret_cast<const char *>(buffer.get()), size);
    } catch (...) {
        return -1;
    }
    return 0;
}

extern "C" {

    char *f9ay_read(const char *filename, int *row, int *col, int *channels) {
        try{
            std::filesystem::path path(filename);
            std::ifstream ifs(path, std::ios::binary);
            if (!ifs.is_open()) {
                return nullptr;
            }
            auto buffer = f9ay::readFile(ifs);
            ifs.close();
            auto importer = selectImporter(buffer.get());
            f9ay::Midway image_midway;
            std::visit(
                [&image_midway, &buffer]<typename T>(T &&imp) {
                    if constexpr (requires { imp.importFromByte(buffer.get()); }) {
                        image_midway = imp.importFromByte(buffer.get());
                    } else {
                        throw std::runtime_error("no implement file format");
                    }
                },
                importer);
            std::tuple<char *, int, int, int> res;
            std::visit(
                [&res]<typename T>(T &&mtx) {
                    int r = mtx.row();
                    int c = mtx.col();
                    int ch = sizeof(mtx[0, 0]);
                    res = {reinterpret_cast<char *>(mtx.transfer_ownership()), r, c, ch};
                },
                image_midway);
            *row = std::get<1>(res);
            *col = std::get<2>(res);
            *channels = std::get<3>(res);
            return std::get<0>(res);
        } catch (...) {
            *row = 0;
            *col = 0;
            *channels = 0;
            return nullptr;
        }
    }

    int f9ay_bmp_export(const char *filename, const char *data, int row, int col, int channels) {
        return f9ay_export<f9ay::Bmp>(filename, data, row, col, channels);
    }

    int f9ay_jpeg_export(const char *filename, const char *data, int row, int col, int channels) {
        return f9ay_export<f9ay::Jpeg>(filename, data, row, col, channels);
    }

    int f9ay_png_export(const char *filename, const char *data, int row, int col, int channels) {
        return f9ay_png_export_internal(filename, data, row, col, channels);
    }

    void f9ay_free(char *ptr) {
        delete[] ptr;
    }
}