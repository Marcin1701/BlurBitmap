#pragma once

#include <vector>
#include <Windows.h>
#include <ObjIdl.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.Lib")

#define SIZE 3


/*
class BlurAlgorithmClass
{
	int maskWeight = -1;
	int mask[SIZE][SIZE];
public:
	BlurAlgorithmClass() = default;
	BlurAlgorithmClass(int _maskWeight)
		: maskWeight(_maskWeight) {
		initializeMask();
	}
	void startAlgorithm(Gdiplus::Color* pixels, std::promise<std::tuple<int, int, int>>&& promise)
	{
		//return ((pixels[0] * mask[0][0] + pixels[1] * mask[0][1] + pixels[2] * mask[0][2]
			//+ pixels[3] * mask[1][0] + pixels[4] + mask[1][1] * pixels[5] + mask[1][2] * pixels[6]
			//+ pixels[6] * mask[2][0] + pixels[7] * mask[2][1] + pixels[8] * mask[2][2]) / (mask[0][0] + mask[0][1] + mask[0][2] + mask[1][0] + mask[1][1] 
			//	+ mask[1][2] + mask[2][0] + mask[2][1]+  mask[2][2]));
	}
	bool initializeMask()
	{
		if (maskWeight == -1)
			exit(EXIT_FAILURE);

		for (int i = 0; i < SIZE; i++)
		{
			for (int j = 0; j < SIZE; j++)
			{
				if (i == j)
					mask[i][j] = maskWeight;
				else
					mask[i][j] = 1;
			}
		}
	}
};
*/
