#include "Application.h"

int _stdcall WinMain(
	_In_	 HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_	 LPSTR	   lpCmdLine,
	_In_	 int	   nCmdShow)
{
	return Application{}.Run();
}