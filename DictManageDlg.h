#pragma once

// CDictManageDlg 对话框
class CDictManageDlg : public CDialogEx
{
// 构造
public:
	CDictManageDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DICTMANAGE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CMFCEditBrowseCtrl FILE1;
	CMFCEditBrowseCtrl FILE2;
	CStatic count1;
	CStatic count2;
	CStatic count_new;
	afx_msg void OnBnClickedDeWeight();
	afx_msg void OnBnClickedCalculate1();
	afx_msg void OnBnClickedCalculate2();
	void CalcCore(int button_i);	
	
	afx_msg void OnBnClickedMerge();
	afx_msg void OnBnClickedFilter();
};
