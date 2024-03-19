
// InjectDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Inject.h"
#include "InjectDlg.h"
#include "afxdialogex.h"
#include <tlhelp32.h>
#include <psapi.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CInjectDlg 对话框



CInjectDlg::CInjectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INJECT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);
}

void CInjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_ComboBox_Process);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
	DDX_Control(pDX, IDC_COMBO5, m_ComboBox_Wmode);
	DDX_Control(pDX, IDC_LIST2, m_List);
	DDX_Control(pDX, IDC_COMBO4, m_ComboBox_Inject);
	DDX_Control(pDX, IDC_EDIT2, m_Edit2);
}

BEGIN_MESSAGE_MAP(CInjectDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CInjectDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CInjectDlg::OnBnClickedButton2)

	ON_CBN_SELCHANGE(IDC_COMBO1, &CInjectDlg::OnCbnSelchangeCombo1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST2, &CInjectDlg::OnRclickList2)
	ON_COMMAND(ID_ResMode, &CInjectDlg::OnResmode)
	ON_COMMAND(ID_KillMode, &CInjectDlg::OnKillmode)
	ON_BN_CLICKED(IDC_BUTTON4, &CInjectDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON3, &CInjectDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CInjectDlg 消息处理程序

BOOL CInjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	this->UP_Authority();
	// TODO: 在此添加额外的初始化代码
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_List.InsertColumn(0, L"模块名称", LVCFMT_LEFT, 90);
	m_List.InsertColumn(1, L"模块基址", LVCFMT_LEFT, 73);
	m_List.InsertColumn(2, L"模块大小", LVCFMT_LEFT, 73);
	m_List.InsertColumn(3, L"模块路径", LVCFMT_LEFT, 300);
	this->InitDialogData();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CInjectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CInjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

VOID CInjectDlg::InitDialogData()
{
	this->InitInject();//设置注入方式
	this->InitWinMode();//设置x64/x32
}

VOID CInjectDlg::InitProcessName()
{
	HANDLE c32Ret = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,NULL);
	PROCESSENTRY32 p32 = {sizeof(PROCESSENTRY32)};
	m_ComboBox_Process.SetItemHeight(0, 20);
	m_ComboBox_Process.SetMinVisibleItems(8);
	while (Process32Next(c32Ret, &p32))
	{
		CString temp;
		temp.Format(L"%d.", m_Cp_Index + 1);
		temp += p32.szExeFile;
		m_ComboBox_Process.InsertString(m_Cp_Index, temp);
		IDbuff[m_Cp_Index] = p32.th32ProcessID;
		m_Cp_Index++;
	}
	CloseHandle(c32Ret);
}

VOID CInjectDlg::InitWinMode()
{
	m_ComboBox_Wmode.AddString(L"x86");
	m_ComboBox_Wmode.AddString(L"x64");
	m_ComboBox_Wmode.SetCueBanner(L"x64");
}

VOID CInjectDlg::InitInject()
{
	m_ComboBox_Inject.AddString(L"远线程注入");
	m_ComboBox_Inject.AddString(L"消息钩子注入");
	m_ComboBox_Inject.AddString(L"劫持进程注入");
	m_ComboBox_Inject.AddString(L"UserAPC注入");
	m_ComboBox_Inject.SetCueBanner(L"远线程注入");
}

VOID CInjectDlg::InjectMode(const TCHAR* injectmode)
{
	int flag;
	if (!StrCmpW(injectmode,L"远线程注入"))
	{
		flag = 0;
	}
	else if (!StrCmpW(injectmode, L"UserAPC注入"))
	{
		flag = 1;
	}
	else if (!StrCmpW(injectmode, L"消息钩子注入"))
	{
		flag = 2;
	}
	else
	{
		flag = 3;
	}
	switch (flag)
	{
		case 0://远线程注入
		{
			int index = m_ComboBox_Wmode.GetCurSel();
			CString szbuff;
			m_ComboBox_Wmode.GetLBText(index, szbuff);
			if (szbuff == L"x86")
			{
				this->X86CRT_Inject();
				break;
			}
			this->CRT_Inject();
			break;
		}
		case 1://UserAPC注入
		{
			this->APC_Inject();
			break;
		}
		case 2://消息钩子注入
		{
			this->MAG_Inject();
			break;
		}
		case 3://劫持进程注入
		{
			this->HJP_Inject();
			break;
		}
	default:
		break;
	}
}

BOOL CInjectDlg::UP_Authority()//提权 我目前没看到作用，不知道是我系统和VS权限本来就设置最最高级，导致看不出吗
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		return   FALSE;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	{
		CloseHandle(hToken);
		return false;
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, NULL))
	{
		CloseHandle(hToken);
		return false;
	}
	return true;

}

void CInjectDlg::APC_Inject()//APC注入
{
	INT Temp_Index = m_ComboBox_Process.GetCurSel();
	HANDLE opRet = OpenProcess(PROCESS_ALL_ACCESS, FALSE, IDbuff[Temp_Index]);
	LPVOID dllnameadder = VirtualAllocEx(opRet, NULL, 1024, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	BOOL falg = WriteProcessMemory(opRet, dllnameadder, m_dllpath, (wcslen(m_dllpath) * 2 + 2), NULL);
	PAPCFUNC loadadder = (PAPCFUNC)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW");
	HANDLE c32Ret = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	THREADENTRY32 t32 = {sizeof(THREADENTRY32)};
	while (Thread32Next(c32Ret,&t32))
	{
		if (t32.th32OwnerProcessID == IDbuff[Temp_Index])
		{
			HANDLE otRet = OpenThread(THREAD_ALL_ACCESS, FALSE, t32.th32ThreadID);
			QueueUserAPC(loadadder, otRet, (ULONG_PTR)dllnameadder);
			CloseHandle(otRet);
		}
	}
	CloseHandle(c32Ret);
	CloseHandle(opRet);
}

void CInjectDlg::CRT_Inject()//远线程注入
{
	INT Temp_Index = m_ComboBox_Process.GetCurSel();
	HANDLE opRet = OpenProcess(PROCESS_ALL_ACCESS, FALSE, IDbuff[Temp_Index]);
	PAPCFUNC loadadder = (PAPCFUNC)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW");
	LPVOID dllnameadder = VirtualAllocEx(opRet, NULL, 1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(opRet, dllnameadder, m_dllpath, (wcslen(m_dllpath) * 2 + 2), NULL);
	HANDLE crRet= CreateRemoteThread(opRet, NULL, NULL, (LPTHREAD_START_ROUTINE)loadadder, dllnameadder, NULL, NULL);
	if (crRet == NULL)
	{
		if (GetLastError() == 5)
		{
			MessageBox(L"拒绝访问", L"", MB_OK);
			CloseHandle(opRet);
			CloseHandle(crRet);
			return;
		}
	}
	WaitForSingleObject(crRet, INFINITE);
	VirtualFreeEx(opRet, dllnameadder, 1024, MEM_DECOMMIT);
	CloseHandle(opRet);
	CloseHandle(crRet);
}


typedef void(*PFN_HOOKSTART)(DWORD);

void CInjectDlg::MAG_Inject()//消息钩子注入
{
	/*INT Temp_Index = m_ComboBox_Process.GetCurSel();
	HMODULE lbRet = LoadLibrary(m_dllpath);
	PFN_HOOKSTART HookStart = (PFN_HOOKSTART)GetProcAddress(lbRet, "HookStart");
	HookStart(IDbuff[Temp_Index]);*/
	INT Temp_Index = m_ComboBox_Process.GetCurSel();
	HMODULE lbRet = LoadLibrary(m_dllpath);
	FARPROC address= GetProcAddress(lbRet, "MyGetMsgProc");
	HANDLE c32Ret = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);  //注释的写法也可以，甚至更好，他可以移除挂钩
	THREADENTRY32 t32 = { sizeof(THREADENTRY32) };
	DWORD threadid = 0;
	while (Thread32Next(c32Ret, &t32))
	{
		if (t32.th32OwnerProcessID == IDbuff[Temp_Index])
		{
			threadid = t32.th32ThreadID;
			break;
		}
	}
	HHOOK ret = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)address, lbRet, threadid);
	MessageBox(L"sa");
	UnhookWindowsHookEx(ret);
}

void CInjectDlg::HJP_Inject()//劫持进程注入
{
	STARTUPINFOW si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	CreateProcess(NULL, m_exepath, NULL,NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);
	LPVOID address = VirtualAllocEx(pi.hProcess, NULL, 1024, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	BOOL a = WriteProcessMemory(pi.hProcess, address, m_dllpath, (wcslen(m_dllpath) * 2 + 2), NULL);
	PAPCFUNC loadadder = (PAPCFUNC)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW");
	HANDLE crtRet = CreateRemoteThread(pi.hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)loadadder, address, NULL,NULL);
	WaitForSingleObject(crtRet, -1);
	ResumeThread(pi.hThread);
	VirtualFreeEx(pi.hProcess, address, 0, MEM_RELEASE);
	HANDLE c32Ret = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pi.dwProcessId);
	MODULEENTRY32 m32 = { sizeof(MODULEENTRY32) };
	INT L_index = 0;
	while (Module32Next(c32Ret, &m32))
	{
		CString temp;
		m_List.InsertItem(L_index, m32.szModule);
		temp.Format(L"%X", m32.modBaseAddr);
		m_List.SetItemText(L_index, 1, temp);
		temp.Format(L"00%X", m32.modBaseSize);
		m_List.SetItemText(L_index, 2, temp);
		m_List.SetItemText(L_index, 3, m32.szExePath);
		L_index++;
	}
	CloseHandle(c32Ret);
}

void CInjectDlg::X86CRT_Inject()//x86远程线程注入
{
	HANDLE Map_ProcessID = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_EXECUTE_READWRITE, 0, 1000, L"Map_ProcessID");
	HANDLE Map_DllPath = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_EXECUTE_READWRITE, 0, 1000, L"Map_DllPath");//创建了两个映射对象
	LPVOID ProcessID = MapViewOfFile(Map_ProcessID, FILE_MAP_ALL_ACCESS, 0, 0, 1000);
	LPVOID DllPath = MapViewOfFile(Map_DllPath, FILE_MAP_ALL_ACCESS, 0, 0, 1000);//把这个两个对象映射到本地进程上来
	INT Temp_Index = m_ComboBox_Process.GetCurSel();
	memcpy(ProcessID, &IDbuff[Temp_Index], sizeof(IDbuff[Temp_Index]));
	memcpy(DllPath, m_dllpath, (wcslen(m_dllpath) * 2) + 2);
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	CreateProcess(L"./X86inject.exe", NULL, NULL, NULL, NULL, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	Sleep(500);
	UnmapViewOfFile(ProcessID);
	UnmapViewOfFile(DllPath);
	CloseHandle(Map_ProcessID);
	CloseHandle(Map_DllPath);
}


void CInjectDlg::OnBnClickedButton1()//刷新进程
{
	while (m_Cp_Index)
	{
		m_Cp_Index--;
		m_ComboBox_Process.DeleteString(m_Cp_Index);
	}
	m_ComboBox_Process.SetCueBanner(L"");
	this->InitProcessName();
	m_List.DeleteAllItems();
}


void CInjectDlg::OnBnClickedButton2()//选择模块
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"(*.dll)|*.dll|(*.*)|*.*||");
	if (dlg.DoModal() == IDOK)
	{
		CString temp = dlg.GetFileTitle();
		//将文本设置到Edit中间显示，并且属性多行为True
		CRect rect;
		GetDlgItem(IDC_EDIT1)->GetClientRect(&rect);
		::OffsetRect(&rect, 0, 3);
		::SendMessageA(((CComboBox*)GetDlgItem(IDC_EDIT1))->m_hWnd, EM_SETRECT, 0, (LPARAM)&rect);

		SetDlgItemText(IDC_EDIT1, temp);
		memcpy(m_dllpath, dlg.GetPathName(), wcslen(dlg.GetPathName()) * 2);
	}
}

void CInjectDlg::OnCbnSelchangeCombo1()
{
	m_List.DeleteAllItems();
	INT Temp_Index = m_ComboBox_Process.GetCurSel();
	HANDLE c32Ret = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, IDbuff[Temp_Index]);
	MODULEENTRY32 m32 = { sizeof(MODULEENTRY32) };
	INT L_index = 0;
	while (Module32Next(c32Ret, &m32))
	{
		CString temp;
		m_List.InsertItem(L_index, m32.szModule);
		temp.Format(L"%X", m32.modBaseAddr);
		m_List.SetItemText(L_index, 1, temp);
		temp.Format(L"00%X", m32.modBaseSize);
		m_List.SetItemText(L_index, 2, temp);
		m_List.SetItemText(L_index, 3, m32.szExePath);
		L_index++;
	}
	CloseHandle(c32Ret);
}


void CInjectDlg::OnRclickList2(NMHDR* pNMHDR, LRESULT* pResult)//右键子菜单
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (m_List.GetFirstSelectedItemPosition() != NULL)
	{
		POINT pt;
		GetCursorPos(&pt);
		HMENU hMenu = LoadMenu(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_MENU1));
		HMENU hSubMenu = GetSubMenu(hMenu, 0);
		TrackPopupMenu(hSubMenu, TPM_CENTERALIGN, pt.x, pt.y, 0, m_hWnd, NULL);
	}
	*pResult = 0;
}


void CInjectDlg::OnResmode()//子菜单刷新模块
{
	this->OnCbnSelchangeCombo1();
}


void CInjectDlg::OnKillmode()//子菜单卸载模块
{
	INT Temp_Index = m_ComboBox_Process.GetCurSel();
	int index = (int)m_List.GetFirstSelectedItemPosition();
	CString temp = m_List.GetItemText(index - 1, 0);
	HANDLE c32Ret = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, IDbuff[Temp_Index]);
	MODULEENTRY32 m32 = { sizeof(MODULEENTRY32) };
	while (Module32Next(c32Ret,&m32))
	{
		if (!StrCmpW(temp.GetString(),m32.szModule))
		{
			HANDLE opRet = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m32.th32ProcessID);
			FARPROC FreeLaddr= GetProcAddress(GetModuleHandle(L"kernel32.dll"),"FreeLibrary");
			HANDLE ctRet = CreateRemoteThread(opRet, NULL, NULL, (LPTHREAD_START_ROUTINE)FreeLaddr, m32.modBaseAddr, 0, 0);
			WaitForSingleObject(ctRet, INFINITE);
			this->OnCbnSelchangeCombo1();
			CloseHandle(ctRet);
			CloseHandle(opRet);
		}
	}
	CloseHandle(c32Ret);
}

void CInjectDlg::OnBnClickedButton4()//注入键
{
	CString tempbuff;
	m_ComboBox_Inject.GetLBText(m_ComboBox_Inject.GetCurSel(), tempbuff);
	this->InjectMode(tempbuff);
}


void CInjectDlg::OnBnClickedButton3()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"(*.exe)|*.exe|(*.*)|*.*||");
	if (dlg.DoModal() == IDOK)
	{
		CString temp = dlg.GetPathName();
		//将文本设置到Edit中间显示，并且属性多行为True
		CRect rect;
		GetDlgItem(IDC_EDIT2)->GetClientRect(&rect);
		::OffsetRect(&rect, 0, 3);
		::SendMessageA(((CComboBox*)GetDlgItem(IDC_EDIT1))->m_hWnd, EM_SETRECT, 0, (LPARAM)&rect);

		SetDlgItemText(IDC_EDIT2, temp);
		memcpy(m_exepath, dlg.GetPathName(), wcslen(dlg.GetPathName()) * 2);
	}
}
