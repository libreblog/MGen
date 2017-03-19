// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "MGen.h"
#include "MainFrm.h"
#include "EditParamsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER1 1
#define TIMER2 2

// CMainFrame

static UINT WM_GEN_FINISH = RegisterWindowMessage("MGEN_GEN_FINISH_MSG");
static UINT WM_DEBUG_MSG = RegisterWindowMessage("MGEN_DEBUG_MSG");

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_FILE_PRINT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnUpdateFilePrintPreview)
	ON_WM_SETTINGCHANGE()
	ON_COMMAND(ID_BUTTON_OUTPUTWND, &CMainFrame::ShowOutputWnd)
	ON_COMMAND(ID_BUTTON_PARAMS, &CMainFrame::OnButtonParams)
	ON_COMMAND(ID_BUTTON_GEN, &CMainFrame::OnButtonGen)
	ON_COMMAND(ID_BUTTON_PLAY, &CMainFrame::OnButtonPlay)
	ON_COMMAND(ID_BUTTON_SETTINGS, &CMainFrame::OnButtonSettings)
	ON_COMMAND(ID_BUTTON_ALGO, &CMainFrame::OnButtonAlgo)
	ON_COMMAND(ID_CHECK_OUTPUTWND, &CMainFrame::OnCheckOutputwnd)
	ON_UPDATE_COMMAND_UI(ID_CHECK_OUTPUTWND, &CMainFrame::OnUpdateCheckOutputwnd)
	ON_UPDATE_COMMAND_UI(ID_COMBO_ALGO, &CMainFrame::OnUpdateComboAlgo)
	ON_COMMAND(ID_COMBO_ALGO, &CMainFrame::OnComboAlgo)
	ON_REGISTERED_MESSAGE(WM_GEN_FINISH, &CMainFrame::OnGenFinish)
	ON_REGISTERED_MESSAGE(WM_DEBUG_MSG, &CMainFrame::OnDebugMsg)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_GEN, &CMainFrame::OnUpdateButtonGen)
	ON_WM_CLOSE()
	ON_COMMAND(ID_BUTTON_HZOOM_DEC, &CMainFrame::OnButtonHzoomDec)
	ON_COMMAND(ID_BUTTON_HZOOM_INC, &CMainFrame::OnButtonHzoomInc)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_HZOOM_DEC, &CMainFrame::OnUpdateButtonHzoomDec)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_HZOOM_INC, &CMainFrame::OnUpdateButtonHzoomInc)
	ON_UPDATE_COMMAND_UI(ID_COMBO_MIDIOUT, &CMainFrame::OnUpdateComboMidiout)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_EPARAMS, &CMainFrame::OnUpdateButtonEparams)
	ON_COMMAND(ID_BUTTON_EPARAMS, &CMainFrame::OnButtonEparams)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_PLAY, &CMainFrame::OnUpdateButtonPlay)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
}

CMainFrame::~CMainFrame()
{
	if (pGen != 0) delete pGen;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	CMFCRibbonStatusBarPane* pPane = new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE);
	pPane->SetAlmostLargeText(_T("Connecting Connecting Connecting Connecting Connecting Connecting"));
	m_wndStatusBar.AddElement(pPane, strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);

	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	LoadAlgo();

	// Algorithm combo
	CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox,
		m_wndRibbonBar.FindByID(ID_COMBO_ALGO));
	for (int i = 0; i < AlgCount; i++) {
		//pCombo->AddItem(AlgName[i]);
		//TCHAR st[100];
		//_stprintf_s(st, _T("%d"), i);
		//pCombo->AddItem(st);
		pCombo->AddItem(AlgName[i], AlgID[i]);
	}

	// MIDI port
	pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox,	m_wndRibbonBar.FindByID(ID_COMBO_MIDIOUT));
	CString st;
	int default_out = Pm_GetDefaultOutputDeviceID();
	for (int i = 0; i < Pm_CountDevices(); i++) {
		const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
		if (info->output) {
			st.Format("%s, %s [%d]", info->name, info->interf, i);
			if (i == default_out) {
				st += " (default)";
			}
		}
		pCombo->AddItem(st, i);
	}

	WriteLog(0, "Started MGen version 1.1.5");
	AfxInitRichEdit2();

	LoadSettings();

	return 0;
}

void CMainFrame::WriteLog(int log, CString st)
{
	COutputList* pOL=0;
	if (log == 0) pOL = &m_wndOutput.m_wndOutputDebug;
	if (log == 1) pOL = &m_wndOutput.m_wndOutputWarn;
	if (log == 2) pOL = &m_wndOutput.m_wndOutputPerf;
	if (log == 3) pOL = &m_wndOutput.m_wndOutputAlgo;
	if (log == 4) pOL = &m_wndOutput.m_wndOutputMidi;
	pOL->AddString(CTime::GetCurrentTime().Format("%H:%M:%S") + " " + st);
	if (pOL->GetCount() > 1000) pOL->DeleteString(0);
	pOL->SetTopIndex(pOL->GetCount() - 1);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	// Create output window
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Output window\n");
		return FALSE; // failed to create
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

}

void CMainFrame::ShowOutputWnd()
{
	if (m_wndOutput.IsVisible()) m_wndOutput.ShowPane(FALSE, FALSE, FALSE);
	else m_wndOutput.ShowPane(TRUE, FALSE, TRUE);
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	m_wndOutput.UpdateFonts();
	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}


void CMainFrame::OnFilePrint()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
	}
}

void CMainFrame::OnFilePrintPreview()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);  // force Print Preview mode closed
	}
}

void CMainFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(IsPrintPreview());
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}


void CMainFrame::OnButtonParams()
{
}


void CMainFrame::OnButtonGen()
{
	if (m_state_gen == 1) {
		if (pGen != 0) {
			pGen->need_exit = 1;
			WriteLog(0, "Sent need_exit to generation thread");
		}
		return;
	}
	/*
	if (m_state_gen == 1) {
		WriteLog(1, "Cannot start generation: generation in progress");
		pGen->need_exit = 1;
		return;
	}
	*/
	if (m_state_gen == 2) {
		WriteLog(0, "Starting generation: Removing previous generator");
		delete pGen;
		m_state_gen = 0;
	}
	pGen = 0;
	int Algo = GetAlgo();
	if (Algo == 101) {
		pGen = new CGenCF1();
	}
	if (Algo == 102) {
		pGen = new CGenCF2();
	}
	if (Algo == 1001) {
		pGen = new CGenRS1();
	}
	if (pGen != 0) {
		WriteLog(0, _T("Started generator: ") + AlgName[Algo]);
		pGen->m_hWnd = m_hWnd;
		pGen->WM_GEN_FINISH = WM_GEN_FINISH;
		pGen->WM_DEBUG_MSG = WM_DEBUG_MSG;
		//pGen->time_started = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		pGen->StopMIDI();
		pGen->StartMIDI(GetMidiI(), 100);
		pGen->time_started = TIME_PROC(TIME_INFO);
		m_GenThread = AfxBeginThread(CMainFrame::GenThread, pGen);
		m_state_gen = 1;
		m_state_play = 0;
		// Start timer
		SetTimer(TIMER1, 100, NULL);
		SetTimer(TIMER2, 1000, NULL);
	}
}

void CMainFrame::OnButtonSettings()
{
	// TODO: Add your command handler code here
}


void CMainFrame::OnButtonAlgo()
{
	//map <string, int> p;
	//p["param1"] = 100;
	//p["param2"] = 200;
	//TCHAR st[100];
	//_stprintf_s(st, _T("%d"), p["param1"] + p["param2"]);
	//WriteLog(0, st);
}


void CMainFrame::OnCheckOutputwnd()
{
	if (m_wndOutput.IsVisible()) m_wndOutput.ShowPane(FALSE, FALSE, FALSE);
	else m_wndOutput.ShowPane(TRUE, FALSE, TRUE);
}

LRESULT CMainFrame::OnGenFinish(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 0) {
		GetActiveView()->Invalidate();
		WriteLog(0, "Generation finished");
		if (m_state_play == 0) ::KillTimer(m_hWnd, TIMER1);
		m_state_gen = 2;
	}
	if (wParam == 1) {
	}
	return 0;
}

LRESULT CMainFrame::OnDebugMsg(WPARAM wParam, LPARAM lParam)
{
	CString* pSt = (CString*)lParam;
	WriteLog(wParam, *pSt);
	delete pSt;
	return LRESULT();
}

void CMainFrame::OnUpdateCheckOutputwnd(CCmdUI *pCmdUI)
{
	BOOL bEnable = m_wndOutput.IsVisible();
	pCmdUI->Enable();
	pCmdUI->SetCheck(bEnable);
}


void CMainFrame::OnUpdateComboAlgo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}

int CMainFrame::GetAlgo()
{
	CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox,
		m_wndRibbonBar.FindByID(ID_COMBO_ALGO));
	return pCombo->GetItemData(pCombo->GetCurSel());
}

int CMainFrame::GetMidiI()
{
	CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox,
		m_wndRibbonBar.FindByID(ID_COMBO_MIDIOUT));
	return pCombo->GetItemData(pCombo->GetCurSel());
}

void CMainFrame::LoadAlgo()
{
	ifstream fs;
	fs.open("algorithms.txt");
	CString st, st2, st3, st4, st5;
	char pch[255];
	int pos = 0;
	// Load header
	fs.getline(pch, 255);
	AlgCount = 0;
	while (fs.good()) {
		pos = 0;
		fs.getline(pch, 255);
		st = pch;
		st.Trim();
		if (st.Find("|") != -1) {
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			AlgID[AlgCount] = atoi(st2);
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			AlgFolder[AlgCount] = st2;
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			AlgName[AlgCount] = st2;
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			AlgGroup[AlgCount] = st2;
			AlgCount++;
		}
	}
	fs.close();
}

void CMainFrame::LoadSettings()
{
	ifstream fs;
	fs.open("settings.txt");
	CString st, st2, st3;
	char pch[255];
	int pos = 0;
	while (fs.good()) {
		fs.getline(pch, 255);
		st = pch;
		st.Trim();
		pos = st.Find("=");
		if (pos != -1) {
			st2 = st.Left(pos);
			st3 = st.Mid(pos+1);
			st2.Trim();
			st3.Trim();
			DWORD dwd = atoi(st3);
			if (st2 == "Algorithm") {
				CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox,
					m_wndRibbonBar.FindByID(ID_COMBO_ALGO));
				int id = distance(AlgID, find(begin(AlgID), end(AlgID), dwd));
				pCombo->SelectItem(id);
				WriteLog(1, st3);
			}
		}
	}
	fs.close();
}

void CMainFrame::SaveSettings()
{
	ofstream fs;
	fs.open("settings.txt");
	CString st;
	st.Format("Algorithm = %d\n", GetAlgo());
	fs << st;
	fs.close();
}

void CMainFrame::OnComboAlgo()
{
	WriteLog(1, "Combo");
	SaveSettings();
	LoadSettings();
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	CFrameWndEx::OnTimer(nIDEvent);
	if (nIDEvent == TIMER1)
	{
		GetActiveView()->Invalidate();
		//int play_time = TIME_PROC(TIME_INFO) - pGen->midi_start_time;
		if ((m_state_gen != 1) && (m_state_play == 2) && (TIME_PROC(TIME_INFO) > pGen->midi_sent_t)) {
			OnButtonPlay();
		}
	}
	if ((nIDEvent == TIMER2) && (pGen != 0))
	{
		if (pGen->t_sent < 2) return;
		// Decide if we can play
		if (m_state_play == 0) {
			if (m_state_gen == 2) m_state_play = 1;
			if (m_state_gen == 1) {
				if (!pGen->mutex_output.try_lock_for(chrono::milliseconds(1000))) {
					WriteLog(4, "OnGenFinish mutex timed out: playback not started");
					return;
				}
				double gtime = pGen->stime[pGen->t_sent - 1];
				double ptime = TIME_PROC(TIME_INFO) - pGen->time_started;
				if ((gtime / ptime > 2) || (gtime > 30000) || ((gtime / ptime > 1.2) && (gtime > 5000))) {
					m_state_play = 1;
				}
				else {
					CString st;
					st.Format("Only %.1f s generated after %.1f s. Playback is postponed to avoid buffer underruns", gtime / 1000, ptime / 1000);
					WriteLog(4, st);
				}
				pGen->mutex_output.unlock();
			}
		}
		if (m_state_play == 1) {
			if ((pGen->t_sent > pGen->midi_sent))
				pGen->SendMIDI(pGen->midi_sent, pGen->t_sent);
			if ((pGen->t_sent == pGen->midi_sent) && (m_state_gen == 2)) {
				::KillTimer(m_hWnd, TIMER2);
				m_state_play = 2;
				//GetActiveView()->Invalidate();
			}
		}
	}
}

UINT CMainFrame::GenThread(LPVOID pParam)
{
	CGenTemplate* pGen = (CGenTemplate*)pParam;

	if (pGen == NULL) return 1;   // if Object is not valid  

	//::PostMessage(pGen->m_hWnd, WM_DEBUG_MSG, 0, (LPARAM)new CString("Thread started"));
	pGen->Generate();
	//Sleep(2000);
	::PostMessage(pGen->m_hWnd, WM_GEN_FINISH, 0, 0);
	//pGen->time_stopped = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	pGen->time_stopped = TIME_PROC(TIME_INFO);

	//::PostMessage(pGen->m_hWnd, WM_DEBUG_MSG, 0, (LPARAM)new CString("Thread stopped"));
	return 0;   // thread completed successfully 
}

void CMainFrame::OnClose()
{
	if (m_state_gen == 1) {
		OnButtonGen();
		WaitForSingleObject(m_GenThread->m_hThread, 10000);
		delete pGen;
		pGen = 0;
	}
	if (pGen != 0) {
		delete pGen;
		pGen = 0;
	}
	Pm_Terminate();

	CFrameWndEx::OnClose();
}


void CMainFrame::OnButtonHzoomDec()
{
	zoom_x = (int)(zoom_x*0.8);
	if (zoom_x < MIN_HZOOM) zoom_x = MIN_HZOOM;
	CString st;
	st.Format("New zoom %d", zoom_x);
	WriteLog(2, st);
	GetActiveView()->Invalidate();
}


void CMainFrame::OnButtonHzoomInc()
{
	zoom_x = (int)(zoom_x*1.2);
	if (zoom_x > MAX_HZOOM) zoom_x = MAX_HZOOM;
	CString st;
	st.Format("New zoom %d", zoom_x);
	WriteLog(2, st);
	GetActiveView()->Invalidate();
}


void CMainFrame::OnUpdateButtonHzoomDec(CCmdUI *pCmdUI)
{
	BOOL bEnable = zoom_x > MIN_HZOOM;
	pCmdUI->Enable(bEnable);
}


void CMainFrame::OnUpdateButtonHzoomInc(CCmdUI *pCmdUI)
{
	BOOL bEnable = zoom_x < MAX_HZOOM;
	pCmdUI->Enable(bEnable);
}


void CMainFrame::OnUpdateComboMidiout(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}


void CMainFrame::OnUpdateButtonEparams(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetAlgo() != 0);
}


void CMainFrame::OnButtonEparams()
{
	CEditParamsDlg dlg;
	dlg.DoModal();
}


void CMainFrame::OnUpdateButtonPlay(CCmdUI *pCmdUI)
{
	if ((m_state_gen == 1) && (m_state_play == 0)) pCmdUI->Enable(0);
	else if (m_state_gen != 0) pCmdUI->Enable(1);
	if (m_state_play > 0) pCmdUI->SetText("Stop Play");
	else pCmdUI->SetText("Play");
}

void CMainFrame::OnUpdateButtonGen(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	if (m_state_gen == 1) pCmdUI->SetText("Stop Gen");
	else pCmdUI->SetText("Generate");
}

void CMainFrame::OnButtonPlay()
{
	if (m_state_play > 0) {
		m_state_play = 0;
		if (pGen != 0) {
			::KillTimer(m_hWnd, TIMER2);
			if (m_state_gen == 2) ::KillTimer(m_hWnd, TIMER1);
			pGen->StopMIDI();
		}
		WriteLog(4, "Stopped playback");
	}
	else if (m_state_gen == 2) {
		pGen->StopMIDI();
		pGen->StartMIDI(GetMidiI(), 100);
		m_state_play = 1;
		// Start timer
		pGen->SendMIDI(pGen->midi_sent, pGen->t_sent);
		SetTimer(TIMER1, 100, NULL);
		SetTimer(TIMER2, 1000, NULL);
	}
}

