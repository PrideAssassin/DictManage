#pragma once
#include<vector>
#include <string>

#define WM_FILE_LOAD_FINISHED (WM_USER + 100)

class My1EditBrowseCtrl : public CMFCEditBrowseCtrl {

protected:
	virtual void OnBrowse();

};

class My2EditBrowseCtrl : public CMFCEditBrowseCtrl {

protected:
	virtual void OnBrowse();
};

struct FileLoadResult {
    int button_i;
    std::vector<std::wstring> data;
};

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
	CStatic count1;
	CStatic count2;
	CStatic count_new;
	My1EditBrowseCtrl file1;
	My2EditBrowseCtrl file2;
	afx_msg void OnBnClickedDeWeight();
	// afx_msg void OnBnClickedCalculate1();
	// afx_msg void OnBnClickedCalculate2();

	afx_msg void OnBnClickedMerge();
	afx_msg void OnBnClickedFilter();
	LRESULT OnFileLoadFinished(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateCountNew(WPARAM wParam, LPARAM lParam);
	// afx_msg void OnEnChangeFile1();
};



class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

