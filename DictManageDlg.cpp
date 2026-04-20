#include "pch.h"
#include "framework.h"
#include "DictManage.h"
#include "DictManageDlg.h"
#include "afxdialogex.h"
#include <iostream>
#include <fstream>
#include <vector>
#include<set>
#include<string>
#include<time.h>
#include<algorithm>
#include<thread>
#include <atlconv.h>
#include <locale>
#include <codecvt>
#include <mutex>
#include <condition_variable>
#include <filesystem>
#include <windows.h>

std::mutex dict_mutex;
std::condition_variable dict_cv;
bool dict_ready = false;


using namespace std;
size_t dict_count1 = 0;		// 用来保存字典1大小
size_t dict_count2 = 0;		// 用来保存字典2大小
size_t dict_new = 0;			// 用来保存新字典大小
vector<wstring> zidian1, zidian2;		//创建string对象，存放字典文件

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 自定义消息定义
#define WM_FILE_LOAD_FINISHED (WM_USER + 100)
#define WM_UPDATE_COUNT_NEW (WM_USER + 101)

// -----------------自定义功能函数-----------------------------
// 计数处理
static void calc(CDictManageDlg* pDlg, UINT nID, CMFCEditBrowseCtrl* pCtrl) {
	if (pDlg != nullptr && pCtrl != nullptr)
	{
		int button_i = (nID == IDC_FILE1) ? 1 : (nID == IDC_FILE2 ? 2 : 0);
		if (button_i == 0) return;

		CString path;
		pCtrl->GetWindowTextW(path);

		// 捕获 button_i 和 path
		thread([pDlg, button_i, path]() {
			vector<wstring> data;
			wifstream fs(path.GetString());
			fs.imbue(locale(""));
			wstring wbuf;
			while (fs >> wbuf) {
				data.push_back(wbuf);
			}
			fs.close();
			auto* result = new FileLoadResult{ button_i, move(data) };
			::PostMessage(pDlg->GetSafeHwnd(), WM_FILE_LOAD_FINISHED, reinterpret_cast<WPARAM>(result), 0);
			}).detach();
	}
}



// -----------------------------关于CAboutDlg---------------------------------------------

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX){}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

CDictManageDlg::CDictManageDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DICTMANAGE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}





//-----------------------------------------------CDictManageDlg类-----------------------------------------------------------------

void CDictManageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COUNT1, count1);
	DDX_Control(pDX, IDC_COUNT2, count2);
	DDX_Control(pDX, IDC_COUNT_NEW, count_new);
	DDX_Control(pDX, IDC_FILE1, file1);
	DDX_Control(pDX, IDC_FILE2, file2);
}

BEGIN_MESSAGE_MAP(CDictManageDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_DE_WEIGHT, &CDictManageDlg::OnBnClickedDeWeight)
	ON_BN_CLICKED(IDC_MERGE, &CDictManageDlg::OnBnClickedMerge)
	ON_BN_CLICKED(IDC_FILTER, &CDictManageDlg::OnBnClickedFilter)
	ON_MESSAGE(WM_FILE_LOAD_FINISHED, &CDictManageDlg::OnFileLoadFinished)
	ON_MESSAGE(WM_UPDATE_COUNT_NEW, &CDictManageDlg::OnUpdateCountNew)
END_MESSAGE_MAP()


BOOL CDictManageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDictManageDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CDictManageDlg::OnPaint()
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

HCURSOR CDictManageDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CDictManageDlg::OnFileLoadFinished(WPARAM wParam, LPARAM lParam)
{
	FileLoadResult* result = reinterpret_cast<FileLoadResult*>(wParam);
    if (result)
    {
        // 更新对应的计数显示
        if (result->button_i == 1)
        {
			zidian1 = move(result->data);
			dict_count1 = static_cast<long>(zidian1.size());
			CString str;
			str.Format(_T("%zu"), static_cast<unsigned long long>(dict_count1));
			count1.SetWindowTextW(str);
        }
        else if (result->button_i == 2)
        {
			zidian2 = move(result->data);
			dict_count2 = static_cast<unsigned long long>(zidian2.size());
			CString str;
			str.Format(_T("%zu"), static_cast<unsigned long long>(dict_count2));
			count2.SetWindowTextW(str);
        }
		delete result; // 释放内存
    }
    return 0;
}

LRESULT CDictManageDlg::OnUpdateCountNew(WPARAM wParam, LPARAM lParam)
{
	CString str;
	str.Format(_T("%zu"), static_cast<unsigned long long>(wParam));
	count_new.SetWindowTextW(str);
	return 0;
}




// 去重复按钮
void CDictManageDlg::OnBnClickedDeWeight()
{
	thread([this]() {
		sort(zidian1.begin(), zidian1.end());
		auto uend = unique(zidian1.begin(), zidian1.end());		//去重函数(不会改变容器大小)
		zidian1.erase(uend, zidian1.end());						 // 删除掉容器重复多余部分
		
		dict_new = zidian1.size();									// 去重后的大小
	
		// 如果原文件大小与去重后大小没有变化(没有重复的元素)，就不执行任何操作,
		if (dict_new < dict_count1)
		{
			CString path;
			this->file1.GetWindowText(path);
			wofstream zd(path.GetString());
			for (auto& i : zidian1)
			{
				zd << i << L"\n";
			}
			zd.close();
		}
		// 线程内
		::PostMessage(this->GetSafeHwnd(), WM_UPDATE_COUNT_NEW, static_cast<WPARAM>(dict_new), 0);

		}).detach();
}

// 合并按钮
void CDictManageDlg::OnBnClickedMerge()
{
	thread([this]() {
		dict_new = 0;
		if (zidian1.empty() && zidian2.empty())
		{
			// 保证两个编辑框里面字典都不会空，才会进行合并字典。
		}
		else {
			if (zidian1.size() >= zidian2.size())
			{
				zidian1.insert(zidian1.end(), zidian2.begin(), zidian2.end());
				dict_new = zidian1.size();									// 合并后的大小
				CString path1;
				file1.GetWindowTextW(path1);
				wofstream zd(path1.GetString());
				for (auto& i : zidian1)
				{
					zd << i + L"\n";
				}
				zd.close();
				CString pathDelete;
				file2.GetWindowTextW(pathDelete);
				std::error_code ec;
				std::filesystem::remove(std::filesystem::path(pathDelete.GetString()),ec);
			}
			else{
				zidian2.insert(zidian2.end(), zidian1.begin(), zidian1.end());
				dict_new = zidian2.size();
				CString path2;
				file1.GetWindowTextW(path2);
				wofstream zd(path2.GetString());
				for (auto& i : zidian2)
				{
					zd << i + L"\n";
				}
				CString pathDelete1;
				std::error_code ec;
				std::filesystem::remove(std::filesystem::path(path2.GetString()), ec);
				zd.close();
			}
		}	
		// 线程内
		::PostMessage(this->GetSafeHwnd(), WM_UPDATE_COUNT_NEW, static_cast<WPARAM>(dict_new), 0);
	}).detach();
}

// 筛选按钮
void CDictManageDlg::OnBnClickedFilter()
{
	dict_new = 0;
	string buf = {};
	ofstream fs_o("c:\\new.txt");

	for (auto& i : zidian1)
	{
		if (!(i.length() < 8))
		{
			//fs_o << i + "\n";
			++dict_new;
		}
	}
	fs_o.close();
	CString str;
	str.Format(_T("%zu"), static_cast<unsigned long long>(dict_new));				
	LPCTSTR pStr = LPCTSTR(str);
	count_new.SetWindowTextW(pStr);		//设置新字典计数大小

}



// ------------My1EditBrowseCtrl类---------------------------------------------------------------
void My1EditBrowseCtrl::OnBrowse()
{
    CMFCEditBrowseCtrl::OnBrowse(); 
    CDictManageDlg* pDlg = dynamic_cast<CDictManageDlg*>(GetParent());
    UINT nID = GetDlgCtrlID();
    if (pDlg != nullptr)
    {
        // 立即更新计数显示
        CString path;
        GetWindowText(path);
        if (!path.IsEmpty())
        {
            calc(pDlg, nID, this);
        }
    }
}

// ------------My2EditBrowseCtrl类---------------------------------------------------------------
void My2EditBrowseCtrl::OnBrowse()
{
	CMFCEditBrowseCtrl::OnBrowse(); // 控件的默认行为
	CDictManageDlg* pDlg = dynamic_cast<CDictManageDlg*>(GetParent());
	UINT nID = GetDlgCtrlID();
	calc(pDlg, nID, this);
}








