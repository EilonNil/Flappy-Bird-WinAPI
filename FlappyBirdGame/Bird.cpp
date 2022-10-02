#include "Bird.h"
#include <windows.h>
namespace Objects {
	Bird::Bird(HBITMAP& g_hbm, BITMAP& bm, PAINTSTRUCT& ps, HWND& hwnd) : Object::Object(g_hbm, bm, ps, hwnd) 
	{
		this->info = { bm.bmWidth,bm.bmHeight,0,0,0,0 };
		info.y = 200;
		info.x = 40;
		info.dy = 8;
	}

	void Bird::init(HBITMAP& g_hbm, BITMAP& bm, PAINTSTRUCT& ps, HWND& hwnd) 
	{
		this->g_hbm = g_hbm;
		this->bm = bm;
		this->ps = ps;
		this->hwnd = hwnd;
		this->info = { bm.bmWidth,bm.bmHeight,40,200,0,8};
		g_hbmMask = CreateBitmapMask(g_hbm, RGB(255, 0, 100));
	}

	OBJ_INFO Bird::getInfo() 
	{
		return Object::getInfo();
	}

	void Bird::draw(HDC hdc) 
	{
		Object::draw(hdc);
	}

	void Bird::erase(HDC hdc, bool& jumping) 
	{
		Object::erase(hdc, jumping);
	}

	void Bird::updatePos() 
	{
		this->info.y += this->info.dy;
	}

	void Bird::move(HDC hdc, bool jumping) 
	{
		this->erase(hdc, jumping);
		this->updatePos();
		this->draw(hdc);
	}

	void Bird::jump() 
	{
		this->info.y -= 50;
	}

	void Bird::setSpeed(int x) 
	{
		this->info.dy = x;
	}
	
	void Bird::getOut() 
	{ //called at the end of the game to alter the info of the bird so the message box is not sent multiple times
		this->info.y =  100;
		this->info.x = -1000;
	}
}
