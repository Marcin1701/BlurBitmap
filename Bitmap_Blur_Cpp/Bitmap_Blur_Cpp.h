#pragma once

#include <fstream>

#ifdef BITMAP_BLUR_Cpp_EXPORTS
#define BITMAP_BLUR_Cpp__declspec(dllexport)
#else
#define BITMAP_BLUR_Cpp __declspec(dllimport)
#endif
extern "C++" BITMAP_BLUR_Cpp void startAlgorithm(unsigned char* pixelBuffer, unsigned char* bitmapPart, int higth, int width, int start, int end);
