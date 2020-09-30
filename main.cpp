#include <windows.h>

#define SPRITE_SPEED 3

HPEN hpBorder;
HBRUSH hbrBlack, hbrWhite;
HANDLE hBitmap;

signed int baseX = 0;
signed int baseY = 0;
signed int speedX = 0;
signed int speedY = 0;
signed int dX = 0;
signed int dY = 0;
unsigned int SPRITE_HEIGHT = 100;
unsigned int SPRITE_WIDTH = 100;
BOOL dragFlag = false;

/* This is where all the input to the window goes to */
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	
	PAINTSTRUCT ps;
	HDC hdc, hCompatibleDC;
	PAINTSTRUCT PaintStruct;
	HANDLE hOldBitmap;
	//RECT r = {0, 0, SPRITE_WIDTH, SPRITE_HEIGHT}; 
	RECT rect;
	unsigned int color;
	
	GetClientRect(hwnd, &rect);
	
	switch(Message) {
		
		/* Upon destruction, tell the main thread to stop */
		case WM_DESTROY: {
			DeleteObject(hBitmap);
			PostQuitMessage(0);
			break;
		}
		case WM_PAINT: {
			
			if (baseX + speedX >= 0 && baseX + speedX <= rect.right - SPRITE_WIDTH) {
				baseX += speedX;
			}
			else {
				if (baseX + speedX < 0) {
					baseX = 0;
					speedX = -speedX;
				}
				else {
					baseX = rect.right - SPRITE_WIDTH;
					speedX = -speedX;
				}
			}
			
			if (baseY + speedY >= 0 && baseY + speedY <= rect.bottom - SPRITE_HEIGHT) {
				baseY += speedY;
			}
			else {
				if (baseY + speedY < 0) {
					baseY = 0;
					speedY = -speedY;
				}
				else {
					baseY = rect.bottom - SPRITE_HEIGHT;
					speedY = -speedY;
				}
			}
			
			hdc = BeginPaint(hwnd, &ps);
			hCompatibleDC = CreateCompatibleDC(hdc);
			hOldBitmap = SelectObject(hCompatibleDC, hBitmap);
			StretchBlt(hdc, baseX, baseY, SPRITE_WIDTH, SPRITE_HEIGHT, hCompatibleDC, 0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, SRCCOPY);
			SelectObject(hCompatibleDC, hOldBitmap);
			DeleteDC(hCompatibleDC);
			
			/*RECT r;
			SelectObject(hdc, hpBorder);
			SelectObject(hdc, hbrWhite);
			SetRect(&r, baseX, baseY, baseX + SPRITE_WIDTH, baseY + SPRITE_HEIGHT);
			Rectangle(hdc, r.left, r.top, r.right, r.bottom);*/
			
			EndPaint(hwnd, &ps);
			UpdateWindow(hwnd);
			return 0;
			break;
		}
		case WM_KEYDOWN: {
			switch (wParam) {
				case VK_LEFT: {
					speedX -= SPRITE_SPEED;
					break;
				}
				case VK_RIGHT: {
					speedX += SPRITE_SPEED;
					break;
				}
				case VK_UP: {
					speedY -= SPRITE_SPEED;
					break;
				}
				case VK_DOWN: {
					speedY += SPRITE_SPEED;
					break;
				}
				case VK_SPACE: {
					speedX = 0;
					speedY = 0;
					break;
				}
			}
			InvalidateRect(hwnd, NULL, TRUE);
			break;
		}
		case WM_MOUSEWHEEL: {
	        if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT)
	        {
	            baseX += GET_WHEEL_DELTA_WPARAM(wParam)/10;
	        }
	        else
	        {
	            baseY -= GET_WHEEL_DELTA_WPARAM(wParam)/10;
	        }
	        InvalidateRect(hwnd, NULL, TRUE);
        	break;
    	}
    	case WM_TIMER: {
    		switch (wParam) {
    			case 1: {
    				InvalidateRect(hwnd, NULL, TRUE);
    				break;
    			}
    		}
    		break;
    	}
    	case WM_LBUTTONDOWN: {
		    if (LOWORD(lParam) > baseX && LOWORD(lParam) < baseX + SPRITE_WIDTH && HIWORD(lParam) > baseY && HIWORD(lParam) < baseY + SPRITE_HEIGHT) {
			    dragFlag = true;
		        speedX = 0;
		        speedY = 0;
	            dX = LOWORD(lParam) - baseX;
	            dY = HIWORD(lParam) - baseY;
	        	break;
	    	}
	    }
	    case WM_MOUSEMOVE: {
	        if (dragFlag) {
	            baseX = LOWORD(lParam) - dX;
	            baseY = HIWORD(lParam) - dY;
	            InvalidateRect(hwnd, NULL, TRUE);
	        }
	        break;
	    }
	    case WM_LBUTTONUP:
	    case WM_NCLBUTTONUP: {
	        dragFlag = false;
	        break;
	    }
		
		/* All other messages (a lot of them) are processed using default procedures */
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

/* The 'main' function of Win32 GUI programs: this is where execution starts */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc; /* A properties struct of our window */
	HWND hwnd; /* A 'HANDLE', hence the H, or a pointer to our window */
	MSG Msg; /* A temporary location for all messages */

	/* zero out the struct and set the stuff we want to modify */
	memset(&wc,0,sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc; /* This is where we will send messages to */
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	
	/* White, COLOR_WINDOW is just a #define for a system color, try Ctrl+Clicking it */
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+2);
	wc.lpszClassName = "WindowClass";
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION); /* Load a standard icon */
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION); /* use the name "A" to use the project icon */

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass","LAB1",WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, /* x */
		CW_USEDEFAULT, /* y */
		800, /* width */
		500, /* height */
		NULL,NULL,hInstance,NULL);

	if(hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	hBitmap = LoadImage(NULL, "123456.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (!hBitmap) {
		MessageBox(NULL, "ÛÀÛ!", "ÛÀÛ!", MB_OK);
		return 0;
	}
	BITMAP Bitmap;

	GetObject(hBitmap, sizeof(BITMAP), &Bitmap);
	SetTimer(hwnd, 1, 16, NULL);
	SPRITE_WIDTH = Bitmap.bmWidth;
	SPRITE_HEIGHT = Bitmap.bmHeight;

	/*
		This is the heart of our program where all input is processed and 
		sent to WndProc. Note that GetMessage blocks code flow until it receives something, so
		this loop will not produce unreasonably high CPU usage
	*/
	while(GetMessage(&Msg, NULL, 0, 0) > 0) { /* If no error is received... */
		TranslateMessage(&Msg); /* Translate key codes to chars if present */
		DispatchMessage(&Msg); /* Send it to WndProc */
	}
	return Msg.wParam;
}
