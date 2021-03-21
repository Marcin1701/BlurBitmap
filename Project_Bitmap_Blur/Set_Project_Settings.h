#pragma once

#include <ObjIdl.h>
#include <gdiplus.h>
#include <cstdlib>
#include <thread>
#include <ctime>
#include <string>
#include <stdio.h>

#include "Blur_Algorithm_Class.h"
#include "Bitmap_Blur_Cpp.h"

#pragma comment (lib,"Gdiplus.Lib")
#pragma warning(disable:4996)

struct ProjectSettings
{
	int numberOfThreads;
	int libSelection;	// 0 - C++, 1 - ASM, -1 None
	std::string filePath;
	std::string printFileName;
	int maskWeight;
	ProjectSettings()
		: numberOfThreads(1), libSelection(-1),
		filePath(""), maskWeight(0),
		printFileName("")
	{}
};

class StartAlgorithms
{
	clock_t time;
	double doubleTime;
	int numberOfThreads;
	int libSelection;
	int maskWeight;
	std::string filePath;
	std::string printFileName;
	std::fstream timeFile;
public:
	StartAlgorithms(ProjectSettings p);
	void stopTime();
	bool checkDataInput();
	void start() { startApplication(); }
	bool startApplication();
	static ProjectSettings eraseData();
	std::string returnStringTime();
	~StartAlgorithms() = default;
};