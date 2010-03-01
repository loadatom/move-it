#include "Vk2Str.h"
#include <wx/wx.h>
#include <windows.h>
#include <map>

using namespace std;
#define ADD_VK_STR(vk, str)	\
	vkmap.insert(std::make_pair(vk, wxT(str)))

wxString Vk2Str(int vk)
{
	static map<int, wxString> vkmap;
	static bool bInit = false;
	map<int, wxString>::iterator iter;
	wxString str;

	if (!bInit)
	{
		bInit = true;
		ADD_VK_STR(VK_LBUTTON, "LButton");
		ADD_VK_STR(VK_RBUTTON, "RButton");
		ADD_VK_STR(VK_CANCEL, "Cancel");
		ADD_VK_STR(VK_MBUTTON, "MButton");
		ADD_VK_STR(VK_XBUTTON1, "XButton1");
		ADD_VK_STR(VK_XBUTTON2, "XButton2");
		ADD_VK_STR(VK_BACK, "Backspace");
		ADD_VK_STR(VK_TAB, "Tab");
		ADD_VK_STR(VK_CLEAR, "Clear");
		ADD_VK_STR(VK_RETURN, "Return");
		ADD_VK_STR(VK_SHIFT, "Shift");
		ADD_VK_STR(VK_CONTROL, "Ctrl");
		ADD_VK_STR(VK_MENU, "Alt");
		ADD_VK_STR(VK_PAUSE, "Pause");
		ADD_VK_STR(VK_CAPITAL, "Caps");
		/*ADD_VK_STR(VK_KANA, "Kana");
		ADD_VK_STR(VK_HANGUL, "Hanguel");
		ADD_VK_STR(VK_JUNJA, "Junja");
		ADD_VK_STR(VK_FINAL, "Final");
		ADD_VK_STR(VK_HANJA, "Hanja");
		ADD_VK_STR(VK_KANJI, "Kanji");*/
		ADD_VK_STR(VK_ESCAPE, "Esc");
		/*ADD_VK_STR(VK_CONVERT, "IMEConv");
		ADD_VK_STR(VK_NONCONVERT, "IMENonConv");
		ADD_VK_STR(VK_ACCEPT, "IMEAccept");
		ADD_VK_STR(VK_MODECHANGE, "IMEModeChange");*/
		ADD_VK_STR(VK_SPACE, "Space");
		ADD_VK_STR(VK_PRIOR, "PgUp");
		ADD_VK_STR(VK_NEXT, "PgDn");
		ADD_VK_STR(VK_END, "End");
		ADD_VK_STR(VK_HOME, "Home");
		ADD_VK_STR(VK_LEFT, "Left");
		ADD_VK_STR(VK_UP, "Up");
		ADD_VK_STR(VK_RIGHT, "Right");
		ADD_VK_STR(VK_DOWN, "Down");
		ADD_VK_STR(VK_SELECT, "Select");
		ADD_VK_STR(VK_PRINT, "Print");
		ADD_VK_STR(VK_EXECUTE, "Execute");
		ADD_VK_STR(VK_SNAPSHOT, "PrtScn");
		ADD_VK_STR(VK_INSERT, "Ins");
		ADD_VK_STR(VK_DELETE, "Del");
		ADD_VK_STR(VK_HELP, "Help");
		ADD_VK_STR('0', "0");
		ADD_VK_STR('1', "1");
		ADD_VK_STR('2', "2");
		ADD_VK_STR('3', "3");
		ADD_VK_STR('4', "4");
		ADD_VK_STR('5', "5");
		ADD_VK_STR('6', "6");
		ADD_VK_STR('7', "7");
		ADD_VK_STR('8', "8");
		ADD_VK_STR('9', "9");
		ADD_VK_STR('A', "A");
		ADD_VK_STR('B', "B");
		ADD_VK_STR('C', "C");
		ADD_VK_STR('D', "D");
		ADD_VK_STR('E', "E");
		ADD_VK_STR('F', "F");
		ADD_VK_STR('G', "G");
		ADD_VK_STR('H', "H");
		ADD_VK_STR('I', "I");
		ADD_VK_STR('J', "J");
		ADD_VK_STR('K', "K");
		ADD_VK_STR('L', "L");
		ADD_VK_STR('M', "M");
		ADD_VK_STR('N', "N");
		ADD_VK_STR('O', "O");
		ADD_VK_STR('P', "P");
		ADD_VK_STR('Q', "Q");
		ADD_VK_STR('R', "R");
		ADD_VK_STR('S', "S");
		ADD_VK_STR('T', "T");
		ADD_VK_STR('U', "U");
		ADD_VK_STR('V', "V");
		ADD_VK_STR('W', "W");
		ADD_VK_STR('X', "X");
		ADD_VK_STR('Y', "Y");
		ADD_VK_STR('Z', "Z");
		/*ADD_VK_STR(VK_LWIN, "LWin");
		ADD_VK_STR(VK_RWIN, "RWin");*/
		ADD_VK_STR(VK_APPS, "App");
		
		ADD_VK_STR(VK_NUMPAD0, "Num0");
		ADD_VK_STR(VK_NUMPAD1, "Num1");
		ADD_VK_STR(VK_NUMPAD2, "Num2");
		ADD_VK_STR(VK_NUMPAD3, "Num3");
		ADD_VK_STR(VK_NUMPAD4, "Num4");
		ADD_VK_STR(VK_NUMPAD5, "Num5");
		ADD_VK_STR(VK_NUMPAD6, "Num6");
		ADD_VK_STR(VK_NUMPAD7, "Num7");
		ADD_VK_STR(VK_NUMPAD8, "Num8");
		ADD_VK_STR(VK_NUMPAD9, "Num9");
		ADD_VK_STR(VK_MULTIPLY, "*");
		ADD_VK_STR(VK_ADD, "+");
		ADD_VK_STR(VK_SEPARATOR, "\\");
		ADD_VK_STR(VK_SUBTRACT, "-");
		ADD_VK_STR(VK_DECIMAL, ".");
		ADD_VK_STR(VK_DIVIDE, "/");
		ADD_VK_STR(VK_F1, "F1");
		ADD_VK_STR(VK_F2, "F2");
		ADD_VK_STR(VK_F3, "F3");
		ADD_VK_STR(VK_F4, "F4");
		ADD_VK_STR(VK_F5, "F5");
		ADD_VK_STR(VK_F6, "F6");
		ADD_VK_STR(VK_F7, "F7");
		ADD_VK_STR(VK_F8, "F8");
		ADD_VK_STR(VK_F9, "F9");
		ADD_VK_STR(VK_F10, "F10");
		ADD_VK_STR(VK_F11, "F11");
		ADD_VK_STR(VK_F12, "F12");
		ADD_VK_STR(VK_F13, "F13");
		ADD_VK_STR(VK_F14, "F14");
		ADD_VK_STR(VK_F15, "F15");
		ADD_VK_STR(VK_F16, "F16");
		ADD_VK_STR(VK_F17, "F17");
		ADD_VK_STR(VK_F18, "F18");
		ADD_VK_STR(VK_F19, "F19");
		ADD_VK_STR(VK_F20, "F20");
		ADD_VK_STR(VK_F21, "F21");
		ADD_VK_STR(VK_F22, "F22");
		ADD_VK_STR(VK_F23, "F23");
		ADD_VK_STR(VK_F24, "F24");

		ADD_VK_STR(VK_NUMLOCK, "NumLock");
		ADD_VK_STR(VK_SCROLL, "Scroll");

		ADD_VK_STR(VK_OEM_1, ";");
		ADD_VK_STR(VK_OEM_PLUS, "=");
		ADD_VK_STR(VK_OEM_COMMA, ",");
		ADD_VK_STR(VK_OEM_MINUS, "-");
		ADD_VK_STR(VK_OEM_PERIOD, ".");
		ADD_VK_STR(VK_OEM_2, "/");
		ADD_VK_STR(VK_OEM_3, "`");
		ADD_VK_STR(VK_OEM_4, "[");
		ADD_VK_STR(VK_OEM_5, "\\");
		ADD_VK_STR(VK_OEM_6, "]");
		ADD_VK_STR(VK_OEM_7, "'");
		
	}

	iter = vkmap.find(vk);
	if (iter != vkmap.end())
		str = iter->second;
	return str;
}