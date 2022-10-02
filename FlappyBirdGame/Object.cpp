#include "Object.h"
#include <windows.h>
#include <iostream>
#include <sstream>


namespace Objects {
	Object::Object(HBITMAP& g_hbm, BITMAP& bm, PAINTSTRUCT& ps, HWND& hwnd) 
	{ //main constructor
		this->g_hbm = g_hbm;
		this->bm = bm;
		this->ps = ps;
		this->hwnd = hwnd;
		this->info = { bm.bmWidth,bm.bmHeight,0,0,0,0 };
		this->g_hbmMask = CreateBitmapMask(g_hbm, RGB(255, 0, 100));
	}

	Object::Object() 
	{ //empty contructor
		this->info = { bm.bmWidth,bm.bmHeight,0,0,0,0 };
		this->g_hbmMask = CreateBitmapMask(g_hbm, RGB(255, 0, 100));
	}

	OBJ_INFO Object::getInfo() 
	{
		return this->info;
	}

	void Object::draw(HDC hdc, int x, int y) 
	{ //draws object in (x,y)
		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, g_hbmMask);

		GetObject(g_hbm, sizeof(bm), &bm);
		info.width = bm.bmWidth;
		info.height = bm.bmHeight;

		BitBlt(hdc, x, y, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCAND); //2 drawings are done with the mask and the original
		// image to use bit manipulation to remove the background of the image
		SelectObject(hdcMem, g_hbm);
		BitBlt(hdc, x, y, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCPAINT);
		info.x = x;
		info.y = y;

		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);
	}

	void Object::draw(HDC hdc) 
	{ //draws object in (info.x,info.y)
		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, g_hbmMask);

		GetObject(g_hbm, sizeof(bm), &bm);
		info.width = bm.bmWidth;
		info.height = bm.bmHeight;

		BitBlt(hdc, info.x, info.y, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCAND);

		SelectObject(hdcMem, g_hbm);
		BitBlt(hdc, info.x, info.y, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCPAINT);

		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);

	}

	void Object::erase(HDC hdc, bool& jumping) 
	{ //erases the object and paints over the erased space with the background color
		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, g_hbm);

		GetObject(g_hbm, sizeof(bm), &bm);

		BitBlt(hdc, info.x, info.y, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCERASE);

		RECT rc;
		GetClientRect(hwnd, &rc);
		rc.left = info.x;
		rc.top = info.y;
		rc.right = info.x + info.width;
		if (info.width == 69 && jumping) { // if the erase is done on the bird (only it has a width of 69) it will also
			rc.bottom = info.y + info.height + 50; //  make sure to delete the extra space caused by the jump
			jumping = false;
		}
		else {
			rc.bottom = info.y + info.height;
		}

		FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 9));

		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);

	}

	void Object::updatePos() 
	{
		this->info.x += this->info.dx;
	}

	void Object::setSpeed(int x) 
	{
		this->info.dx = x;
	}

	bool jumping = false; //irrelevant, just in place of the jumping variable used for the bird
	void Object::move(HDC hdc) 
	{ //moves the object
		this->erase(hdc, jumping);
		this->updatePos();
		this->draw(hdc);
	}

	HBITMAP Object::CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent) 
	{ //mask used to remove image background
		HDC hdcMem, hdcMem2;
		HBITMAP hbmMask;
		BITMAP bm;

		GetObject(hbmColour, sizeof(BITMAP), &bm);
		hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

		hdcMem = CreateCompatibleDC(0);
		hdcMem2 = CreateCompatibleDC(0);

		SelectObject(hdcMem, hbmColour);
		SelectObject(hdcMem2, hbmMask);

		SetBkColor(hdcMem, crTransparent);

		BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

		BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

		DeleteDC(hdcMem);
		DeleteDC(hdcMem2);

		return hbmMask;
	}

	void Object::sendBack(int i, int& score, int& speed) 
	{ //sends the pillars back to the beginning when they reach the end
		int random = std::rand() % 200;
		if (this->info.x < -150) {
			this->info.x = 720;
			if (i < 0) {
				if (score % 3 == 0 && score > 0) {
					speed -= 2 ;
				} //done before the score addition so it does not trigger multiple times accidentally
				score++;
				this->info.y = i + 580;
			}
			else {
				this->info.y = random - 350;
			}
		}
		
	}

	bool Object::isIntersect(OBJ_INFO info) 
	{ //checks for intersection with pillars, ground, sky
		if (info.x < this->info.x + this->info.width && this->info.x < info.x + info.width && info.y < this->info.y + this->info.height)
		{
			return this->info.y < info.y + info.height;
		}
		if (info.y > 400) {
			return true;
		}
		if (info.y < -150) {
			return true;
		}

		return false;
	}

	void Object::dealWithIntersect(int score, Object pillars[], int y) 
	{ //deals with intersection with appropriate response
		if (y > 400) {
			SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0);
		}
		else {
			SendMessage(hwnd, WM_GETTITLEBARINFOEX, 0, 0);
		}

		for (int i = 0; i < 4; i++) {
			pillars[i].info.x = 1000;
			pillars[i].info.y = 1000;
			pillars[i].setSpeed(0);
		}
		const char* msg = "Game Over!\nyour score was: ";
		std::stringstream ss;
		ss << msg << std::to_string(score);
		std::string message = ss.str();
		const char* message1 = message.c_str();
		if (y < -150) {
			SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0);
			msg = "Icarus flew too\nclose to the sun :(\nscore: ";
			std::stringstream ss;
			ss << msg << std::to_string(score);
			std::string message = ss.str();
			message1 = message.c_str();
			MessageBox(hwnd, message1, "GAME OVER!", MB_OK | MB_ICONEXCLAMATION);
		}
		else {
			MessageBox(hwnd, message1, "GAME OVER!", MB_OK | MB_ICONEXCLAMATION);
		}
		PostMessage(hwnd, WM_CLOSE, 0, 0); //closing the game
	}

	bool Object::intersect(OBJ_INFO info, int score, Object pillars[]) 
	{ //called from main to check intersection
		if (isIntersect(info)) {
			dealWithIntersect(score, pillars, info.y);
			return true;
		}
		return false;
	}
}
