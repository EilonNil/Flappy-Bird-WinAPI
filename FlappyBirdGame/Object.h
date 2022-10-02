#pragma once
#include <windows.h>
namespace Objects {
	typedef struct _OBJ_INFO {
		int width;
		int height;
		int x;
		int y;

		int dx;
		int dy;
	} OBJ_INFO;


	class Object
	{
	protected:
		HBITMAP g_hbm;
		BITMAP bm;
		PAINTSTRUCT ps;
		HWND hwnd;
		HBITMAP g_hbmMask;
	public:
		OBJ_INFO info; 
		//general functions
		Object(HBITMAP& g_hbm, BITMAP& bm, PAINTSTRUCT& ps, HWND& hwnd);
		Object();
		OBJ_INFO getInfo();

		//graphic related functions
		void draw(HDC hdc, int x, int y);
		void draw(HDC hdc);
		void erase(HDC hdc, bool& jumping);
		HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent);
		void move(HDC hdc);
		
		//info related functions
		void updatePos();
		void setSpeed(int x);
		void sendBack(int i, int& score, int& speed);

		//intersection functions
		bool intersect(OBJ_INFO info, int score, Object pillars[]);
		bool isIntersect(OBJ_INFO info);
		void dealWithIntersect(int score, Object pillars[], int y);
	};

}

