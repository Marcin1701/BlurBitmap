#include "pch.h"

#include "Bitmap_Blur_Cpp.h"
#include <vector>

void startAlgorithm(unsigned char* pixelBuffer, unsigned char* bitmapPart, int higth, int width, int start, int end)
{
	// Indeks nowej czêœci
	int indexPart = 0;
	// Aktualnie przerabiane piksele
	std::vector<unsigned char> pixels;
	// Rozpocznij przerabianie od okreœlonego momentu
	for (int i = start; i < end; i++) {
		// Warunki pobrania danych ze zdjêcia
		if (i >= width * 3 - 3
			&& i + width * 3 + 3 < higth * width * 3
			&& i < higth * width * 3 + 3) {
			bitmapPart[indexPart] = (pixelBuffer[i - width * 3 - 3] + pixelBuffer[i - width * 3]
				+ pixelBuffer[i - width * 3 + 3] + pixelBuffer[i - 3] + pixelBuffer[i + 3] +
				pixelBuffer[i + width * 3 - 3] + pixelBuffer[i + width * 3] + pixelBuffer[i + width * 3 + 3]) / 8;
			indexPart++;
		}
		else {
			bitmapPart[indexPart] = 0;
			indexPart++;
		}
	}
}