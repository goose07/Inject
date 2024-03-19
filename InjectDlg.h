
// InjectDlg.h: 头文件
//

#pragma once


// CInjectDlg 对话框
class CInjectDlg : public CDialogEx
{
// 构造
public:
	CInjectDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INJECT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	INT IDbuff[1000];
	INT m_Cp_Index = 0;
	CComboBox m_ComboBox_Process;
	CComboBox m_ComboBox_Inject;
	CComboBox m_ComboBox_Wmode;
	CListCtrl m_List;
	CEdit m_Edit;
	CEdit m_Edit2;//文件选择
	TCHAR m_dllpath[MAX_PATH] = { 0 };//dll模块路径
	TCHAR m_exepath[MAX_PATH] = { 0 };//文件路径
public:
	VOID InitDialogData();//总调用流程函数
	VOID InitProcessName();//初始化进程名称
	VOID InitWinMode();//注入x64/x86显示
	VOID InitInject();//注入模式显示
	VOID InjectMode(const TCHAR* injectmode);//注入模式选择
	BOOL UP_Authority();
	void APC_Inject();//APC注入
	void CRT_Inject();//远线程注入
	void MAG_Inject();//消息注入
	void HJP_Inject();//劫持进程注入
	void X86CRT_Inject();
	afx_msg void OnBnClickedButton1();//刷新进程
	afx_msg void OnBnClickedButton2();//选择模块
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnRclickList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnResmode();
	afx_msg void OnKillmode();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton3();
};
