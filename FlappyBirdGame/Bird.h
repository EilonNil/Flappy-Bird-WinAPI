#pragma once
#include "Object.h"
#include <windows.h>
namespace Objects {
	class Bird : Object {
	public:
		//main functions
		Bird(HBITMAP& g_hbm, BITMAP& bm, PAINTSTRUCT& ps, HWND& hwnd);
		void init(HBITMAP& g_hbm, BITMAP& bm, PAINTSTRUCT& ps, HWND& hwnd); //used for later initalization of fields
		OBJ_INFO getInfo();

		// graphic functions
		void draw(HDC hdc);
		void erase(HDC hdc, bool& jumping);
		void move(HDC hdc, bool jumping);

		//info related functions
		void updatePos();
		void jump();
		void setSpeed(int x);
		void getOut();
	};
}
