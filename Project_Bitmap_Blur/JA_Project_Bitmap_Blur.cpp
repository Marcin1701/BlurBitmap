#include <Windows.h>
#include <ObjIdl.h>
#include <gdiplus.h>
#include <commctrl.h>
#include <iostream>
#include <cstdio>
#include <thread>
#include <cstring>

#include "Bitmap_Blur_Cpp.h"
#include "Set_Project_Settings.h"

#pragma warning(disable:4996)
#pragma comment (lib,"Gdiplus.Lib")
#pragma comment (lib,"Comctl32.Lib")

void CreateControls(HWND hwnd);
void UpdateLabel(void);
void CreateControlsMask(HWND hwnd);
void UpdateLabelMask(void);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND hTrack;
HWND hlbl;
HWND hTrackMask;
HWND hlblMask;
ProjectSettings projSet;

const char* WindowName = "Bitmap Blur Project";

MSG message;
ULONG_PTR gdiplusToken;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{	
 //   Gdiplus::GdiplusStartupInput gdiplusStartupInput;
  //  Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    srand(time(NULL));

    //HINSTANCE dllHandle = NULL;
   // dllHandle = LoadLibrary(LPCSTR("Bitmap_Blur_ASSEMBLER.dll"));    
	
    WNDCLASSEX wc;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = WindowName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClassEx(&wc);
    HWND hwnd;
    
	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, WindowName, "Rozmycie Bitmapy - Projekt JA", WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 500, NULL, NULL, hInstance, NULL);
    
    if (hwnd == NULL)
    {
        MessageBox(NULL, "Bład tworzenia okna!", "Bład!", MB_ICONEXCLAMATION);
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
	
    while (GetMessage(&message, NULL, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
    return message.wParam;    
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HDC WindowHdc;
    PAINTSTRUCT ps;
    std::string filePath = "";
    std::string printFileName = "";

    switch (msg)
    {
    case WM_CREATE:
        CreateControls(hwnd);
        CreateControlsMask(hwnd);
        CreateWindowW(L"BUTTON", L"C++",
            WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
            100, 50, 150, 50, hwnd, (HMENU)1,
            NULL, NULL);
        CreateWindowW(L"BUTTON", L"ASM",
            WS_VISIBLE | WS_CHILD |BS_RADIOBUTTON,
            300, 50, 150, 50, hwnd, (HMENU)2, 
            NULL, NULL);
        CreateWindowW(L"BUTTON", L"Rozpocznij",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            200, 400, 150, 50, hwnd, (HMENU)3,
            NULL, NULL);
        CreateWindowW(L"BUTTON", L"Wybierz Plik",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            100, 300, 150, 50, hwnd, (HMENU)4,
            NULL, NULL);
        CheckDlgButton(hwnd, 1, BST_UNCHECKED);
        CheckDlgButton(hwnd, 2, BST_UNCHECKED);
        break;
    case WM_COMMAND:
        switch LOWORD(wParam)
        {
        case 1:
            CheckDlgButton(hwnd, 1, BST_CHECKED);
            CheckDlgButton(hwnd, 2, BST_UNCHECKED);
            projSet.libSelection = 0;
            break;
        case 2:
            CheckDlgButton(hwnd, 1, BST_UNCHECKED);
            CheckDlgButton(hwnd, 2, BST_CHECKED);
            projSet.libSelection = 1;    	
            break;
        case 3:
	        {
            MessageBox(hwnd, "Zaczynam pracę! Kliknij 'OK'.", "Rozpocznij",
                MB_ICONINFORMATION | MB_OK);
            StartAlgorithms test(projSet);
            if (!test.startApplication()) {
                MessageBox(hwnd, "Wystąpił błąd - Sprawdź wprowadzone dane!", "Rozpocznij",
                    MB_ICONINFORMATION | MB_OKCANCEL);
                break;
            }
            std::string score = "Skonczylem w czasie: " + test.returnStringTime() + "ms.";
        		MessageBox(hwnd, score.c_str(), "Bitmapa",
                MB_ICONINFORMATION | MB_OKCANCEL);
            break;
	        }
        case 4:
            filePath.erase();
            printFileName.erase();
            OPENFILENAME ofn = { 0 };
            TCHAR szFile[260] = { 0 };
            ofn.lStructSize = sizeof(ofn);
            ofn.hInstance = GetModuleHandle(NULL);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = NULL;
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
            
            if (GetOpenFileName(&ofn) != true)
                break;
            filePath = szFile;
            if (filePath[filePath.size() - 3] != 'b' ||
                filePath[filePath.size() - 2] != 'm' ||
                filePath[filePath.size() - 1] != 'p') {
                MessageBox(hwnd, "To nie jest bitmapa!", "Error!",
                    MB_ICONERROR );
                break;
            }
            projSet.filePath = filePath;
            bool endFound = false;
            std::vector<char> temp;
            for (int i = 0; i < filePath.size(); i++) {
                if (filePath[i] == '\\') {
                    for (int j = i + 1; j < filePath.size(); j++) {
                        if (filePath[j] == '\\') {
                            endFound = false;
                            break;
                        }
                        if (j == filePath.size() - 1) {
                            endFound = true;
                            i++;
                        }
                    }
                }
                if (endFound)
                    temp.emplace_back(filePath[i]);
            }
            printFileName = std::string(temp.begin(), temp.end());
            projSet.printFileName = printFileName;
    		TextOut(GetDC(hwnd), 300, 320, printFileName.c_str(), printFileName.size());
            break;
        }
        break;
    case WM_HSCROLL:
        UpdateLabel();
        UpdateLabelMask();
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        WindowHdc = BeginPaint(hwnd, &ps);
        TextOut(GetDC(hwnd), 100, 150, "Wybierz liczbę wątków:", 23);
        TextOut(GetDC(hwnd), 225, 25, "Wybierz bibliotekę:", 19);
        TextOut(GetDC(hwnd), 100, 230, "Wybierz stopień rozmycia:", 25);
        EndPaint(hwnd, &ps);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

void CreateControls(HWND hwnd)
{
    hlbl = CreateWindowW(L"Static", L"1", WS_CHILD | WS_VISIBLE,
        460, 150, 30, 30, hwnd, (HMENU)5, NULL, NULL);

    INITCOMMONCONTROLSEX icex;

    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    hTrack = CreateWindowW(TRACKBAR_CLASSW, L"Trackbar Control",
        WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
        300, 150, 150, 30, hwnd, (HMENU)6, NULL, NULL);

    SendMessageW(hTrack, TBM_SETRANGE, TRUE, MAKELONG(1, 64));
    SendMessageW(hTrack, TBM_SETPAGESIZE, 0, 10);
    SendMessageW(hTrack, TBM_SETTICFREQ, 8, 0);
    SendMessageW(hTrack, TBM_SETPOS, FALSE, 0);
}

void UpdateLabel(void) {

    LRESULT pos = SendMessageW(hTrack, TBM_GETPOS, 0, 0);
    wchar_t buf[4];
    wsprintfW(buf, L"%ld", pos);
    projSet.numberOfThreads = (int)pos;
    SetWindowTextW(hlbl, buf);
}

void CreateControlsMask(HWND hwnd)
{
    hlblMask = CreateWindowW(L"Static", L"1", WS_CHILD | WS_VISIBLE,
        460, 230, 30, 30, hwnd, (HMENU)7, NULL, NULL);

    INITCOMMONCONTROLSEX icex;

    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    hTrackMask = CreateWindowW(TRACKBAR_CLASSW, L"Trackbar Control",
        WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
        300, 230, 150, 30, hwnd, (HMENU)8, NULL, NULL);

    SendMessageW(hTrackMask, TBM_SETRANGE, TRUE, MAKELONG(1, 16));
    SendMessageW(hTrackMask, TBM_SETPAGESIZE, 0, 10);
    SendMessageW(hTrackMask, TBM_SETTICFREQ, 3, 0);
    SendMessageW(hTrackMask, TBM_SETPOS, FALSE, 0);
}

void UpdateLabelMask(void) {

    LRESULT pos = SendMessageW(hTrackMask, TBM_GETPOS, 0, 0);
    wchar_t buf[4];
    wsprintfW(buf, L"%ld", pos);
    projSet.maskWeight = (int)pos;
    SetWindowTextW(hlblMask, buf);
}