#ifndef WIN_MOVER_H
#define WIN_MOVER_H

#include <vector>
#include <windows.h>

namespace moveit {
	struct MonitorInfo
	{
		RECT workArea;
	};

	class WinMover
	{
	public:
		std::vector<MonitorInfo> m_monitors;

		bool Init();
		bool ReInit();
		bool MoveForegroundWindowToNextMonitor();
	private:
		static BOOL CALLBACK MonitorEnumFunc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lpreMonitor, LPARAM dwData);
		bool IsWinInMonitor(const RECT& winRect, const RECT& monitorRect);
		int FindWinMonitor(const RECT& winRect);
		bool MoveWinToMonitor(HWND hwnd, const WINDOWINFO& winInfo, int srcMonitorIdx, int dstMonitorIdx);
	};
}	//moveit
#endif	//WIN_MOVER_H