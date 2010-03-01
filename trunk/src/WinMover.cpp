#include "WinMover.h"

namespace moveit{
	bool WinMover::Init()
	{
		bool bResult = false;
		BOOL bRet = FALSE;

		bRet = EnumDisplayMonitors(NULL, NULL, &MonitorEnumFunc, (LPARAM)this);
		if (bRet)
			return true;
		return false;
	}

	BOOL CALLBACK WinMover::MonitorEnumFunc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lpreMonitor, LPARAM dwData)
	{
		MONITORINFO info;
		WinMover* pThis = (WinMover*)(dwData);
		MonitorInfo mi;

		info.cbSize = sizeof(info);
		GetMonitorInfo(hMonitor, &info);
		
		mi.workArea = info.rcWork;
		pThis->m_monitors.push_back(mi);
		
		return TRUE;
	}

	bool WinMover::IsWinInMonitor(const RECT &winRect, const RECT &monitorRect)
	{
		bool bIn = !(winRect.left > monitorRect.right ||
			winRect.right < monitorRect.left ||
			winRect.top > monitorRect.bottom ||
			winRect.bottom < monitorRect.top);
		if (bIn)
			return winRect.left != monitorRect.right;
		return false;
	}

	int WinMover::FindWinMonitor(const RECT &winRect)
	{
		for(std::vector<MonitorInfo>::iterator iter = m_monitors.begin();
			iter != m_monitors.end();
			++iter)
		{
			if (IsWinInMonitor(winRect, iter->workArea))
				return static_cast<int>(iter - m_monitors.begin());
		}
		return -1;
	}

	bool WinMover::MoveForegroundWindowToNextMonitor()
	{
		bool bResult = false;
		HWND hwnd = NULL;
		WINDOWINFO winInfo;
		int monitorIdx = -1;
		int nextMonitorIdx = 0;
		int monitorCount = static_cast<int>(m_monitors.size());
		RECT winRect;

		hwnd = GetForegroundWindow();
		winInfo.cbSize = sizeof(winInfo);
		GetWindowInfo(hwnd, &winInfo);
		winRect = winInfo.rcWindow;
		if ((winInfo.dwStyle & WS_MAXIMIZE) > 0)
		{
			WINDOWPLACEMENT winPlacement;
			GetWindowPlacement(hwnd, &winPlacement);
			winRect = winPlacement.rcNormalPosition;
		}
		monitorIdx = FindWinMonitor(winRect);
		if (monitorIdx < 0)
			goto Exit0;
		nextMonitorIdx = (monitorIdx + 1) % monitorCount;
		MoveWinToMonitor(hwnd, winInfo, monitorIdx, nextMonitorIdx);
		bResult = true;
	Exit0:
		return bResult;
	}

	bool WinMover::MoveWinToMonitor(HWND hwnd, const WINDOWINFO& winInfo, int srcMonitorIdx, int dstMonitorIdx)
	{
		bool bResult = false;
		BOOL retCode = FALSE;
		MonitorInfo* pDstMonitorInfo = &m_monitors[dstMonitorIdx];
		MonitorInfo* pSrcMonitorInfo = &m_monitors[srcMonitorIdx];
		int offsetx = winInfo.rcWindow.left - pSrcMonitorInfo->workArea.left;
		int offsety = winInfo.rcWindow.top - pSrcMonitorInfo->workArea.top;
		int width = winInfo.rcWindow.right - winInfo.rcWindow.left + 1;
		int height = winInfo.rcWindow.bottom - winInfo.rcWindow.top + 1;
		int dstleft = pDstMonitorInfo->workArea.right - offsetx - width;
		int dsttop = pDstMonitorInfo->workArea.top + offsety;
		bool bMaximize = ((winInfo.dwStyle & WS_MAXIMIZE) > 0);

		dstleft = max(dstleft, pDstMonitorInfo->workArea.left);
		dsttop = max(dsttop, pDstMonitorInfo->workArea.top);

		if (bMaximize)
		{
			ShowWindow(hwnd, SW_RESTORE);	
		}
		if (dstleft + width > pDstMonitorInfo->workArea.right)
		{
			dstleft = pDstMonitorInfo->workArea.right - width;
			if (dstleft < pDstMonitorInfo->workArea.left)
			{
				dstleft = pDstMonitorInfo->workArea.left;
				width = pDstMonitorInfo->workArea.right - pDstMonitorInfo->workArea.left;
			}
		}
		if (dsttop + height > pDstMonitorInfo->workArea.bottom)
		{
			dsttop = pDstMonitorInfo->workArea.bottom - height;
			if (dsttop < pDstMonitorInfo->workArea.top)
			{
				dsttop = pDstMonitorInfo->workArea.top;
				height = pDstMonitorInfo->workArea.bottom - pDstMonitorInfo->workArea.top;
			}
		}

		retCode = SetWindowPos(hwnd, NULL, dstleft, dsttop, width, height, SWP_NOSIZE);
		if (bMaximize)
				ShowWindow(hwnd, SW_MAXIMIZE);
		if (retCode)
		{
			return true;
		}
		return false;
	}
}