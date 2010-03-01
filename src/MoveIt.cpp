#include <windows.h>
#include "wx/wx.h"
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/strconv.h>
#include <wx/log.h>
#include <wx/snglinst.h>
#include <wx/ipc.h>
#include <stdio.h>
#include "WinMover.h"
#include "IniConfig.h"
#include "Vk2Str.h"

using namespace moveit;
#define moveitMAIN_FRAME_ID	 wxID_HIGHEST + 1
namespace {

	const wxChar *APP_IDENTIFIER = wxT("ALIB_MOVE_IT");

	void DisplayHotKey(wxTextCtrl* pTxtInfo, int modifiers, int vk)
	{
		wxString hotKey;
	
		if (!pTxtInfo)
			return;
		if (modifiers & wxMOD_ALT)
		{
			hotKey.append(Vk2Str(VK_MENU));
			hotKey.append(wxT(" "));
		}
		if (modifiers & wxMOD_CONTROL)
		{
			hotKey.append(Vk2Str(VK_CONTROL));
			hotKey.append(wxT(" "));
		}
		if (modifiers & wxMOD_SHIFT)
		{
			hotKey.append(Vk2Str(VK_SHIFT));
			hotKey.append(wxT(" "));
		}
		
		if (vk)
		{
			wxString vkStr = Vk2Str(vk);
			if (vkStr.IsEmpty())
			{
				hotKey.Clear();
				goto Exit0;
			}
			hotKey.append(vkStr);
		}
Exit0:
		pTxtInfo->Clear();
		pTxtInfo->AppendText(hotKey);
	}
}

class MoveItConnection : public wxConnection
{
public:
	virtual bool OnExecute(const wxString& topic, wxChar*data, int size,
			wxIPCFormat format);
};

class MoveItServer : public wxServer
{
public:
	virtual wxConnectionBase* OnAcceptConnection(const wxString &Topic);
};

class MoveItClient : public wxClient
{
public:
	virtual wxConnectionBase* OnMakeConnection()
		{ return new MoveItConnection(); }
};

class MoveItApp : public wxApp
{
public:
	wxSingleInstanceChecker *m_pSingleInst;
	MoveItServer *m_pServer;

	MoveItApp();
	virtual bool OnInit();
	virtual int OnExit();
};

class ShowHotkeyValidator : public wxValidator
{
public:
	int m_vk;
	UINT m_modifiers;
	int m_lastvk;
	int m_keyDownCount;
	int m_keyUpCount;

	ShowHotkeyValidator();
	virtual bool ProcessEvent(wxEvent& e);
	virtual ShowHotkeyValidator* Clone() const;
	void Reset();
private:
	bool ProcessKeyDown(wxKeyEvent& e);
	bool ProcessKeyUp(wxKeyEvent& e);
	bool IsAllKeyUp();
};

class MainFrame : public wxFrame
{
public:
	wxTextCtrl* m_pTxtInfo;
	WinMover m_winMover;
	int m_hotKeyId;
	ShowHotkeyValidator m_hotKeyValidator;
	IniConfig m_config;
	bool m_bNeedSave;

	MainFrame(const wxString& title);
	bool Init();
	bool SetHotKey(int modifiers, int vk);
	void LoadConfig();

	void OnReloadSettings(wxCommandEvent& e);
	void OnKeyPress(wxKeyEvent& e);
	void OnHotKey(wxKeyEvent& e);
	void OnCloseMainFrame(wxCloseEvent& e);
	void OnTextInfoGetFocus(wxFocusEvent& e);
	void OnMenuAbout(wxCommandEvent &e);
	void OnMinimize(wxIconizeEvent &e);
//	DECLARE_EVENT_TABLE();
};

IMPLEMENT_APP(MoveItApp);

wxConnectionBase *MoveItServer::OnAcceptConnection(const wxString &topic)
{
	if (topic == APP_IDENTIFIER)
	{
		return new MoveItConnection();
	}
	return NULL;
}

bool MoveItConnection::OnExecute(const wxString &topic, wxChar *data, int size, wxIPCFormat format)
{
	MainFrame *pMainFrame = wxDynamicCast(wxGetApp().GetTopWindow(), MainFrame);
	if (pMainFrame)
	{
		pMainFrame->Show();
		pMainFrame->Raise();
		pMainFrame->Iconize(false);
	}
	return true;
}

ShowHotkeyValidator::ShowHotkeyValidator()
{
	Reset();	
}

void ShowHotkeyValidator::Reset()
{
	m_vk = 0;
	m_lastvk = 0;
	m_modifiers = 0;
	m_keyDownCount = 0;
	m_keyUpCount = 0;
	DisplayHotKey((wxTextCtrl*)GetWindow(), m_modifiers, m_vk);
}

bool ShowHotkeyValidator::ProcessEvent(wxEvent &e)
{
	if (e.GetEventType() == wxEVT_KEY_DOWN ||
		e.GetEventType() == wxEVT_KEY_UP)
	{
		bool bFinish = false;
		wxKeyEvent* pKeyEvent = (wxKeyEvent*)(&e);
		if (e.GetEventType() == wxEVT_KEY_DOWN)
			bFinish = ProcessKeyDown(*pKeyEvent);
		else
			bFinish = ProcessKeyUp(*pKeyEvent);
		return bFinish;
	}
	else if(e.GetEventType() == wxEVT_CHAR)
		return true;
	return false;	
}

ShowHotkeyValidator* ShowHotkeyValidator::Clone() const
{
	ShowHotkeyValidator* pResult = new ShowHotkeyValidator();
	pResult->m_vk = m_vk;
	pResult->m_modifiers = m_modifiers;
	return pResult;
}

bool ShowHotkeyValidator::ProcessKeyDown(wxKeyEvent& e)
{
	bool bResult = false;

	_tprintf(_T("Before KeyDown: %d, Dn(%d), Up(%d)\n"), e.GetRawKeyCode(), m_keyDownCount, m_keyUpCount);
	
	if (m_keyDownCount == 0 && e.GetRawKeyCode() == VK_TAB)
	{
		WXMSG msg;
		wxTextCtrl *pTxtCtrl = (wxTextCtrl*)GetWindow();
		long txtStyle = pTxtCtrl->GetWindowStyleFlag();
		BOOL retCode = FALSE;

		msg.hwnd = (HWND)(GetWindow()->GetHWND());
		msg.message = WM_KEYDOWN;
		msg.lParam = e.GetRawKeyFlags();
		msg.wParam = e.GetRawKeyCode();
		msg.pt.x = e.GetX();
		msg.pt.y = e.GetY();
		msg.time = e.GetTimestamp();
		pTxtCtrl->SetWindowStyleFlag(txtStyle | wxTE_READONLY);
		retCode = (::IsDialogMessage((HWND)(wxGetApp().GetTopWindow()->GetHWND()), &msg));
		pTxtCtrl->SetWindowStyleFlag(txtStyle);
		goto Exit1;
	}

	if (m_lastvk == e.GetRawKeyCode())
	{
		goto Exit1;
	}
	m_lastvk = e.GetRawKeyCode();
	m_vk = e.GetRawKeyCode();
	if (m_vk == VK_SHIFT ||
		m_vk == VK_CONTROL ||
		m_vk == VK_MENU)
	{
		m_vk = 0;
	}
			
	m_modifiers = e.GetModifiers();
	++m_keyDownCount;
	DisplayHotKey((wxTextCtrl*)GetWindow(), m_modifiers, m_vk);
Exit1:
	bResult = true;
	_tprintf(_T("KeyDown: %d, Dn(%d), Up(%d)\n"), e.GetRawKeyCode(), m_keyDownCount, m_keyUpCount);
	return bResult;
}

bool ShowHotkeyValidator::ProcessKeyUp(wxKeyEvent &e)
{
	++m_keyUpCount;
	if (IsAllKeyUp() || m_keyDownCount == 0)
	{
		m_keyDownCount = 0;
		m_keyUpCount = 0;
	}
	_tprintf(_T("KeyUp: %d, Dn(%d), Up(%d)\n"), e.GetRawKeyCode(), m_keyDownCount, m_keyUpCount);
	return true;
}

bool ShowHotkeyValidator::IsAllKeyUp()
{
	return m_keyDownCount == m_keyUpCount;
}

MoveItApp::MoveItApp()
{
	m_pServer = NULL;
	m_pSingleInst = NULL;
}

bool MoveItApp::OnInit()
{
	m_pSingleInst = new wxSingleInstanceChecker(APP_IDENTIFIER);
	if (!m_pSingleInst->IsAnotherRunning())
	{
		m_pServer = new MoveItServer();
		m_pServer->Create(APP_IDENTIFIER);
	}
	else
	{
		wxLogNull logNull;
		MoveItClient *pClient = new MoveItClient();
		wxConnectionBase *pConnection = pClient->MakeConnection(wxT("localhost"), 
			APP_IDENTIFIER, APP_IDENTIFIER);

		if (pConnection)
		{
			pConnection->Execute(NULL);
			pConnection->Disconnect();
			delete pConnection;
		}
		delete pClient;
		return false;
	}

	MainFrame* pFrame = new MainFrame(wxT("MoveIt"));
	pFrame->Show();
	return true;
}

int MoveItApp::OnExit()
{
	delete m_pServer;
	return wxApp::OnExit();
}

void MainFrame::OnReloadSettings(wxCommandEvent &e)
{
	ShowHotkeyValidator* pValidator = (ShowHotkeyValidator*)(m_pTxtInfo->GetValidator());
	bool bRetCode = false;

	SetHotKey(pValidator->m_modifiers, pValidator->m_vk);
}

MainFrame::MainFrame(const wxString &title)
{
	bool retCode = false;

	wxBoxSizer *pTopSizer = new wxBoxSizer(wxVERTICAL);
	wxMenuBar *pMenuBar = NULL;
	wxMenu *pMenuHelp = NULL;
	m_bNeedSave = false;

	MainFrame::Create((wxWindow*)NULL, moveitMAIN_FRAME_ID, title,
		wxDefaultPosition, wxDefaultSize,
		(wxDEFAULT_FRAME_STYLE & (~wxMAXIMIZE_BOX)) | wxTAB_TRAVERSAL);
		//(wxDEFAULT_FRAME_STYLE & (~wxMAXIMIZE_BOX)));
	CreateStatusBar();
	pMenuHelp = new wxMenu();
	pMenuHelp->Append(wxID_ABOUT);
	pMenuBar = new wxMenuBar();
	pMenuBar->Append(pMenuHelp, wxT("&Help"));
	SetMenuBar(pMenuBar);
	wxButton* pBtnShow = new wxButton(this, wxID_ANY, wxT("set"));
	m_pTxtInfo = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, 
		wxDefaultSize,
		//wxTE_PROCESS_TAB | wxTE_READONLY,
		wxTE_PROCESS_TAB,
		m_hotKeyValidator
		);

	pTopSizer->Add(m_pTxtInfo, 1, wxEXPAND | wxALL);
	pTopSizer->Add(pBtnShow);

	SetSizer(pTopSizer);
	pTopSizer->Fit(this);
	pTopSizer->SetSizeHints(this);

	m_winMover.Init();

	m_hotKeyId = (int)GlobalAddAtom(_T("ALIB_MOVE_IT"));
	//retCode = RegisterHotKey(m_hotKeyId, wxMOD_CONTROL | wxMOD_ALT, VK_F8);

	//Connect(GetId(), wxEVT_KEY_DOWN, 
	//	(wxObjectEventFunction)
	//	(wxEventFunction)
	//	(wxCharEventFunction)(&MainFrame::OnKeyPress));
	Connect(m_hotKeyId, wxEVT_HOTKEY, 
		(wxObjectEventFunction)
		(wxEventFunction)
		(wxCharEventFunction)(&MainFrame::OnHotKey));
	Connect(GetId(), wxEVT_CLOSE_WINDOW, 
		(wxObjectEventFunction)
		(wxEventFunction)
		(wxCloseEventFunction)(&MainFrame::OnCloseMainFrame));
	Connect(pBtnShow->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
		(wxObjectEventFunction)
		(wxEventFunction)
		(wxCommandEventFunction)(&MainFrame::OnReloadSettings));
	m_pTxtInfo->Connect(m_pTxtInfo->GetId(), wxEVT_SET_FOCUS,
		(wxObjectEventFunction)
		(wxEventFunction)
		(wxFocusEventFunction)(&MainFrame::OnTextInfoGetFocus),
		NULL,
		this);
	Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED,
		(wxObjectEventFunction)
		(wxEventFunction)
		(wxCommandEventFunction)(&MainFrame::OnMenuAbout));
	Connect(GetId(), wxEVT_ICONIZE,
		(wxObjectEventFunction)
		(wxEventFunction)
		(wxIconizeEventFunction)(&MainFrame::OnMinimize));
	LoadConfig();
#ifdef __WXDEBUG__
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
#endif //__WXDEBUG__
}

void MainFrame::OnKeyPress(wxKeyEvent &e)
{
	wxString str = wxString::Format(wxT("%d\n"), e.GetRawKeyCode());
	m_pTxtInfo->AppendText(str);
}

void MainFrame::OnHotKey(wxKeyEvent &e)
{
	m_winMover.MoveForegroundWindowToNextMonitor();
}

void MainFrame::OnCloseMainFrame(wxCloseEvent &e)
{
	UnregisterHotKey(m_hotKeyId);
	GlobalDeleteAtom((ATOM)m_hotKeyId);
	if (m_bNeedSave)
		m_config.Save();
	Destroy();
}

void MainFrame::OnTextInfoGetFocus(wxFocusEvent &e)
{
	ShowHotkeyValidator *pValidator = (ShowHotkeyValidator*)(m_pTxtInfo->GetValidator());
	UnregisterHotKey(m_hotKeyId);
	m_pTxtInfo->Clear();
	pValidator->Reset();
	GetStatusBar()->SetStatusText(wxT("Clear"));
}

void MainFrame::LoadConfig()
{
	wxStandardPaths stdPaths;
	wxFileName iniFileName(stdPaths.GetExecutablePath());
	wxString iniFilePath(iniFileName.GetPath());
	iniFilePath.Append(wxFileName::GetPathSeparator());
	iniFilePath.Append(iniFileName.GetName());
	iniFilePath.Append(wxT(".ini"));
	bool bRetCode = false;

	m_config.Init(iniFilePath, wxConvUTF8);

	{
		wxLogNull nullLog;
		bRetCode = m_config.Load();
	}

	if (bRetCode)
	{
		wxString vkStr, modifiersStr;
		long vk, modifiers;
		
		m_config.GetVal(wxT("config"), wxT("vk"), wxT("0"), &vkStr);
		m_config.GetVal(wxT("config"), wxT("modifiers"), wxT("0"), &modifiersStr);
		if (!vkStr.ToLong(&vk))
			vk = 0;
		if (!modifiersStr.ToLong(&modifiers))
			modifiers = 0;
		if (vk != 0)
		{
			if (SetHotKey(modifiers, vk))
				DisplayHotKey(m_pTxtInfo, modifiers, vk);
		}
	}
}

bool MainFrame::SetHotKey(int modifiers, int vk)
{
	bool bRetCode = false;
	wxString vkStr;

	UnregisterHotKey(m_hotKeyId);
	if (vk == 0)
	{
		GetStatusBar()->SetStatusText(wxT("Clear"));
		m_config.SetVal(wxT("config"), wxT("vk"), wxT("0"));
		m_config.SetVal(wxT("config"), wxT("modifiers"), wxT("0"));
		m_bNeedSave = true;
		return false;
	}
	vkStr = Vk2Str(vk);
	if (vkStr.IsEmpty())
	{
		GetStatusBar()->SetStatusText(wxT("Invalid hotkey"));
		return false;
	}
	bRetCode = RegisterHotKey(m_hotKeyId, modifiers, vk);
	if (bRetCode)
	{
		GetStatusBar()->SetStatusText(wxT("Success"));
		m_config.SetVal(wxT("config"), wxT("vk"), wxString::Format(wxT("%d"), vk));
		m_config.SetVal(wxT("config"), wxT("modifiers"), wxString::Format(wxT("%d"), modifiers));
		m_bNeedSave = true;
		return true;
	}
	else
		GetStatusBar()->SetStatusText(wxT("Fail"));
	return false;
}

void MainFrame::OnMenuAbout(wxCommandEvent &e)
{
	wxDialog *dlg = new wxDialog(this, wxID_ANY, wxString(wxT("About")));
	wxBoxSizer* pTopSizer = new wxBoxSizer(wxVERTICAL);
	wxStaticText *pWord = new wxStaticText(dlg, wxID_ANY, wxT("Let's move it."));
	wxStaticText *pName = new wxStaticText(dlg, wxID_ANY, wxT("--Alex"));

	pTopSizer->Add(pWord, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
	pTopSizer->Add(pName, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_RIGHT, 10);
	dlg->SetSizer(pTopSizer);
	pTopSizer->Fit(dlg);
	dlg->Show();
}

void MainFrame::OnMinimize(wxIconizeEvent &e)
{
	if (e.Iconized())
		Hide();
}