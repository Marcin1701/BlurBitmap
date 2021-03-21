#include "Set_Project_Settings.h"

#include <iostream>
#include <algorithm>

//typedef void(_fastcall* Main)(unsigned char*, unsigned char*, int, int, int, int);
//HINSTANCE dllHandle = LoadLibrary("Bitmap_Blur_ASSEMBLER.dll");
//Main procedura = (Main)GetProcAddress(dllHandle, "Main");

StartAlgorithms::StartAlgorithms(ProjectSettings p)
	: numberOfThreads(p.numberOfThreads),
	libSelection(p.libSelection),
	maskWeight(p.maskWeight),
	filePath(p.filePath),
	printFileName(p.printFileName)
{}

void StartAlgorithms::stopTime()
{

}

bool StartAlgorithms::checkDataInput()
{
	if (filePath == "" || printFileName == "" || libSelection == -1)
		return false;
	return true;
}

void passInformationToAssembler(unsigned char* pixelBuffer, int higth, int width, int start, int end, unsigned char* bitmapPart)
{
	//procedura(pixelBuffer, std::ref(bitmapPart), higth, width, start, end);
}

bool StartAlgorithms::startApplication()
{
	typedef void(_fastcall* Main)(unsigned char*, unsigned char*, int, int, int, int);
	HINSTANCE dllHandle = LoadLibrary("Bitmap_Blur_ASSEMBLER.dll");
	Main procedura = (Main)GetProcAddress(dllHandle, "Main");

	// SprawdŸ czy u¿ytkownik wprowadzi³ dane
	if (!checkDataInput())
		return false;

	// Nag³ówek bitmapy
	char information[54];
	// Plik wejœciowy
	FILE* input = fopen(filePath.c_str(), "rb");
	// Plik wyjœciowy 
	//std::string outputFileName = std::to_string(rand() % 1000000 + 1) + ".bmp";

	std::string outputFileName = "output.bmp";
	FILE* output = fopen(outputFileName.c_str(), "wb");

	// Wczytaj nag³ówek
	fread(information, 1, 54, input);
	// Zapisz nag³ówek do pliku wyjœciowego
	fwrite(information, 1, 54, output);

	int width = *(int*)(information + 18);	// Pobierz szerokoœæ
	int higth = *(int*)(information + 22);	// Pobierz wysokoœæ
	
	// Lista z indeksami
	std::vector<int> lineIndexes;
	int bitmapDivision = higth / (numberOfThreads);
	int lineIndex = 0;

	// Bufor pikseli
	unsigned char* pixelBuffer = new unsigned char[width * 3 * higth];
	fread(pixelBuffer, sizeof(unsigned char), width * 3 * higth, input);

	// Inicjalizacja listy indeksów
	for (int i = 0; i < numberOfThreads; i++) {
		if (i == 0) {
			lineIndexes.push_back(0);
			lineIndexes.push_back((i + 1) * bitmapDivision * width * 3);
		}
		else {
			lineIndexes.push_back(i * bitmapDivision * width * 3);
			if (i == numberOfThreads - 1) {
				lineIndexes.push_back(width * higth * 3);
				break;
			}
			lineIndexes.push_back((i + 1) * bitmapDivision * width * 3);
		}
	}

	// Czêœci bitmapy 
	std::vector<unsigned char*> bitmapParts;

	// Podziel bitmapê na czêœci
	int x = 0, y = 1;
	for (int i = 0; i < numberOfThreads; i++) {
		int size = lineIndexes[y] - lineIndexes[x];
		unsigned char* temp = new unsigned char[size];
		bitmapParts.push_back(temp);
		x += 2;
		y += 2;
	}

	// Ustal w¹tki
	std::vector<std::thread> threads;

	//clock_t timeStart = clock();
	auto clockStart = std::chrono::steady_clock::now();
	int howManyParts = maskWeight;
	for (int i = 0; i < howManyParts; i++) {
		y = 1;
		x = 0;
		lineIndex = 0;
		if (libSelection == 1) {
			for (int i = 0; i < numberOfThreads; i++) {
				//threads.push_back(std::thread(&passInformationToAssembler, pixelBuffer, higth, width, lineIndexes[lineIndex], lineIndexes[lineIndex + 1], std::ref(bitmapParts[i])));
				threads.push_back(std::thread(procedura, pixelBuffer, std::ref(bitmapParts[i]), higth, width, lineIndexes[lineIndex], lineIndexes[lineIndex + 1]));
				lineIndex += 2;
			}
			for (int i = 0; i < numberOfThreads; i++) {
				if (threads[i].joinable()) {
					threads[i].join();
				}
			}
			threads.empty();
		}
		else if (libSelection == 0) {
			for (int i = 0; i < numberOfThreads; i++) {
				threads.push_back(std::thread(&startAlgorithm, pixelBuffer, std::ref(bitmapParts[i]), higth, width, lineIndexes[lineIndex], lineIndexes[lineIndex + 1]));
				lineIndex += 2;
			}
			for (int i = 0; i < numberOfThreads; i++) {
				if (threads[i].joinable()) {
					threads[i].join();
				}
			}
			threads.empty();
		}
		if (maskWeight == 0) {
			break;
		}
		int pixCounter = 0;
		for (int i = 0; i < bitmapParts.size(); i++) {
			int size = lineIndexes[y] - lineIndexes[x];
			for (int j = 0; j < lineIndexes[y] - lineIndexes[x]; j++) {
				pixelBuffer[pixCounter] = bitmapParts[i][j];
				pixCounter++;
			}
			x += 2;
			y += 2;
		}
		threads.clear();
	}

	auto clockEnd = std::chrono::steady_clock::now();
	auto finalTime = clockEnd - clockStart;
	doubleTime = std::chrono::duration<double, std::milli>(finalTime).count();

	x = 0;
	y = 1;
	for (int i = 0; i < bitmapParts.size(); i++) {
		int size = lineIndexes[y] - lineIndexes[x];
		fwrite(bitmapParts[i], sizeof(unsigned char), size, output);
		x += 2;
		y += 2;
	}

	fclose(output);
	for (int i = 0; i < numberOfThreads; i++) {
		delete bitmapParts[i];
	}
	bitmapParts.clear();
	delete[] pixelBuffer;
	//doubleTime = double(clock() - timeStart) / CLOCKS_PER_SEC;

	return true;
}

ProjectSettings StartAlgorithms::eraseData()
{
	ProjectSettings temp;
	temp.libSelection = -1;
	temp.filePath = "";
	temp.maskWeight = 0;
	temp.numberOfThreads = 0;
	return temp;
}

std::string StartAlgorithms::returnStringTime()
{
	return std::string(std::to_string(doubleTime));
}