#include <windows.h>
#include <iostream> 
#include <string>
#include "Object.h"
#include "resource.h"
#include "Bird.h"

using namespace Objects; //namespace i created for all the objects, no functions with the same name as base c++ functions

const char g_szClassName[] = "myWindowClass"; //variables for window initiation
const int WINDOW_HEIGHT = 480;
const int WINDOW_WIDTH = 720;
const int ID_TIMER = 1;

bool firstDraw = true; //indicating the first drawing of the pillars
int random;
int score = 0;
int speed = -10;
bool jumping = false;
bool canJump = true;
HWND scoreDisplay; //used to display the current score

HBITMAP g_hbmBird = NULL;
HBITMAP g_hbmTop1 = NULL;
HBITMAP g_hbmTop2 = NULL;
HBITMAP g_hbmBtm1 = NULL;
HBITMAP g_hbmBtm2 = NULL;

Object pillars[4];
BITMAP bm;
PAINTSTRUCT ps;
HWND hwnd;
Bird bird(g_hbmBird, bm, ps, hwnd); //initializing the bird with empty values to have it globally, will later be initalized

void updateAll(HWND hwnd, Bird& bird, Object pillars[], HDC hdc) 
{
	random = std::rand() % 200; 
	if (firstDraw) { //used to draw them for the first time. this is done so they have 
		bird.draw(hdc); // bitmap values used for erasing them later on
		pillars[0].draw(hdc, 800, random - 350); //(-350) - (-150)
		pillars[1].draw(hdc, 800, random + 230); //(-120) - (80)
		random = std::rand() % 200;
		pillars[2].draw(hdc, 1250, random - 350);
		pillars[3].draw(hdc, 1250, random + 230);
		firstDraw = false;
	}
	bird.move(hdc, jumping);

	for (int i = 0; i < 4; i++) { //main loop; loops over all the pillars
		pillars[i].setSpeed(speed);
		pillars[i].move(hdc);
		std::string scoreStr = std::to_string(score);
		const char* scoreChar = scoreStr.c_str();
		SetWindowText(scoreDisplay, TEXT(scoreChar)); //displays current score
		if (i % 2 == 1) {// if the pillar is a bottom pillar i send the height of the top pillar so i can adjust
			pillars[i].sendBack(pillars[i - 1].info.y, score, speed); // the bottom's height accordingly
		}
		else {
			pillars[i].sendBack(i, score, speed);
		}
		pillars[i].intersect(bird.getInfo(), score, pillars); //checks and handles intersection and game end
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		//loading the bird
		g_hbmBird = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BIRD));
		if (g_hbmBird == NULL)
			MessageBox(hwnd, "Could not load IDB_BIRD!", "Error", MB_OK | MB_ICONEXCLAMATION);
		//loading the pillars
		g_hbmTop1 = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_TOP1));
		if (g_hbmTop1 == NULL)
			MessageBox(hwnd, "Could not load IDB_TOP1!", "Error", MB_OK | MB_ICONEXCLAMATION);
		g_hbmTop2 = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_TOP2));
		if (g_hbmTop2 == NULL)
			MessageBox(hwnd, "Could not load IDB_TOP2!", "Error", MB_OK | MB_ICONEXCLAMATION);
		g_hbmBtm1 = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BTM1));
		if (g_hbmBtm1 == NULL)
			MessageBox(hwnd, "Could not load IDB_BTM1!", "Error", MB_OK | MB_ICONEXCLAMATION);
		g_hbmBtm2 = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BTM2));
		if (g_hbmBtm2 == NULL)
			MessageBox(hwnd, "Could not load IDB_BTM2!", "Error", MB_OK | MB_ICONEXCLAMATION);

		UINT ret; //timer
		ret = SetTimer(hwnd, ID_TIMER, 50, NULL);
		if (ret == 0)
			MessageBox(hwnd, "Could not SetTimer()!", "Error", MB_OK | MB_ICONEXCLAMATION);

		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_PAINT:
	{
		//initialzing the bird
		bird.init(g_hbmBird, bm, ps, hwnd);

		//initialzing the pillars
		Object top1(g_hbmTop1, bm, ps, hwnd);
		Object top2(g_hbmTop2, bm, ps, hwnd);
		Object btm1(g_hbmBtm1, bm, ps, hwnd);
		Object btm2(g_hbmBtm2, bm, ps, hwnd);

		pillars[0] = top1;
		pillars[1] = btm1;
		pillars[2] = top2;
		pillars[3] = btm2;

		HDC hdc = BeginPaint(hwnd, &ps);
		bird.draw(hdc);
		top1.draw(hdc, 800, 0);
		btm1.draw(hdc, 800, 600);
		top2.draw(hdc, 1160, 0);
		btm2.draw(hdc, 1160, 600);
		EndPaint(hwnd, &ps); //inital draw
	}
	break;
	case WM_TIMER:
	{
		HDC hdc = GetDC(hwnd);
		updateAll(hwnd, bird, pillars, hdc);
		ReleaseDC(hwnd, hdc);

		if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && canJump) {
			canJump = false;
			jumping = true;
			bird.jump();

		}
		else if (GetAsyncKeyState(VK_SPACE) == 0) { //so can't jump twice
			canJump = true;
		}
	}
	break;
	case WM_GETTEXTLENGTH: //called when bird falls to the ground (this specific call is meaningless in this program)
	{
		bird.setSpeed(0);
		bird.getOut();
	}
	break;
	case WM_GETTITLEBARINFOEX: //called when bird intersects with pillar (this specific call is meaningless in this program)
	{
		bird.setSpeed(0);
	}
	break;
	case WM_DESTROY:
		KillTimer(hwnd, ID_TIMER);
		DeleteObject(g_hbmBird);
		DeleteObject(g_hbmBtm1);
		DeleteObject(g_hbmBtm2);
		DeleteObject(g_hbmTop1);
		DeleteObject(g_hbmTop2);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	srand(time(0)); //resets the random with seed 0

	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 9); //+10 - white background, +9 - blue background
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"Flappy Bird Game",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT,
		NULL, NULL, hInstance, NULL);

	std::string scoreStr = std::to_string(score); //creates score dispaly window
	const char* scoreChar = scoreStr.c_str();
	scoreDisplay = CreateWindow("STATIC", scoreChar, WS_VISIBLE | WS_CHILD | SS_LEFT, 300, 0, 24, 20, hwnd, NULL, hInstance, NULL);
	

	if (hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}

