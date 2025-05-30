#pragma once


#ifdef F9AY_LIB_EXPORTS
#define F9AY_API __declspec(dllexport)
#else
#define F9AY_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
    F9AY_API char* f9ay_read(const char *filename, int *row, int *col, int *channles);
    F9AY_API int f9ay_bmp_export(const char *filename, const char *data, int row, int col, int channels);
    F9AY_API int f9ay_jpeg_export(const char *filename, const char *data, int row, int col, int channels);
    F9AY_API int f9ay_png_export(const char *filename, const char *data, int row, int col, int channels);
    F9AY_API void f9ay_free(char*);
#ifdef __cplusplus
}
#endif