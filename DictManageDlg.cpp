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



using namespace std;
auto dict_count1 = 0;		// 用来保存字典1大小
auto dict_count2 = 0;		// 用来保存字典2大小
auto dict_new = 0;			// 用来保存新字典大小

vector<string> zidian1, zidian2;		//创建string对象，存放字典文件

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <string>


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

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

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

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

void CDictManageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE1, FILE1);
	DDX_Control(pDX, IDC_COUNT1, count1);
	DDX_Control(pDX, IDC_COUNT_NEW, count_new);
	DDX_Control(pDX, IDC_FILE2, FILE2);
	DDX_Control(pDX, IDC_COUNT2, count2);
}

BEGIN_MESSAGE_MAP(CDictManageDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CALCULATE1, &CDictManageDlg::OnBnClickedCalculate1)
	ON_BN_CLICKED(IDC_DE_WEIGHT, &CDictManageDlg::OnBnClickedDeWeight)
	ON_BN_CLICKED(IDC_CALCULATE2, &CDictManageDlg::OnBnClickedCalculate2)
	ON_BN_CLICKED(IDC_MERGE, &CDictManageDlg::OnBnClickedMerge)
	ON_BN_CLICKED(IDC_FILTER, &CDictManageDlg::OnBnClickedFilter)
END_MESSAGE_MAP()


// CDictManageDlg 消息处理程序

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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

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

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDictManageDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// 计算按钮的核心代码
void CDictManageDlg::CalcCore(int button_i)
{
	CString str;
	string buf;						//设置一个读取缓存区
	char* name = 0;
	size_t len = 0;

	switch (button_i)
	{
	case 1:
	{
		zidian1 = {};
		FILE1.GetWindowTextW(str);		//获取编辑框1内容
		ifstream fs(str);		// 打开指定路径文件，并与当前流绑定(创建对象fs，并初始化)
		// 从指定流读取数据到缓存区，存将缓冲区字符存入zidian模板
		while (fs >> buf)
		{
			zidian1.push_back(buf);				//将读取的数据存到字典对象中,//1600W个元素，添加时间大概52秒。
		}
		fs.close();								//关闭字典文件流
		dict_count1 = zidian1.size();					// 获取字典的大小
		str.Format(_T("%d"), dict_count1);				// 将int转PTRSTR类型
		LPCTSTR pStr = LPCTSTR(str);
		count1.SetWindowTextW(pStr);		//设置字典1计数大小
		break;
	}
	case 2:
	{
		zidian2 = {};
		FILE2.GetWindowTextW(str);		//获取编辑框2内容
		ifstream fs(str);		// 打开指定路径文件，并与当前流绑定(创建对象fs，并初始化)
		// 从指定流读取数据到缓存区，存将缓冲区字符存入zidian模板
		while (fs >> buf)
		{
			zidian2.push_back(buf);				//将读取的数据存到字典对象中,//1600W个元素，添加时间大概52秒。
		}
		fs.close();								//关闭字典文件流
		dict_count2 = zidian2.size();					// 获取字典的大小
		str.Format(_T("%d"), dict_count2);				// 将int转PTRSTR类型
		LPCTSTR pStr = LPCTSTR(str);
		count2.SetWindowTextW(pStr);		//设置字典1计数大小
		break;
	}
	default:
		break;
	}
}

// 计算1按钮，被单击事件(计数字典大小)
void CDictManageDlg::OnBnClickedCalculate1()
{
	CalcCore(1);
}

// 计算2按钮，被单击事件(计数字典大小)
void CDictManageDlg::OnBnClickedCalculate2()
{
	CalcCore(2);
}

// 去重复按钮被单击
void CDictManageDlg::OnBnClickedDeWeight()
{
	OnBnClickedCalculate1();
	sort(zidian1.begin(), zidian1.end());						//排序函数
	auto uend = unique(zidian1.begin(), zidian1.end());		//去重函数(不会改变容器大小)
	zidian1.erase(uend, zidian1.end());						 // 删除掉容器重复多余部分
	dict_new = zidian1.size();									// 去重后的大小
	// 如果原文件大小与去重后大小没有变化(没有重复的元素)，就不执行任何操作,
	if (dict_new < dict_count1)
	{
		ofstream zd("c:\\new.txt");
		for (auto& i : zidian1)
		{
			zd << i + "\n";
		}
		zd.close();
	}
	CString str;
	str.Format(_T("%d"), dict_new);				// 将int转PTRSTR类型
	LPCTSTR pStr = LPCTSTR(str);
	count_new.SetWindowTextW(pStr);		//设置字典1计数大小
}

// 合并按钮被单击
void CDictManageDlg::OnBnClickedMerge()
{
	OnBnClickedCalculate1();
	OnBnClickedCalculate2();
	dict_new = 0;
	if (zidian1.empty() && zidian2.empty())
	{
		// 保证两个编辑框里面字典都不会空，才会进行合并字典。
	}
	else
	{
		if (zidian1.size() >= zidian2.size())
		{
			zidian1.insert(zidian1.end(), zidian2.begin(), zidian2.end());
			dict_new = zidian1.size();									// 合并后的大小
			ofstream zd("c:\\new.txt");
			for (auto& i : zidian1)
			{
				zd << i + "\n";
			}
			zd.close();
			CString str;
			str.Format(_T("%d"), dict_new);				// 将int转PTRSTR类型
			LPCTSTR pStr = LPCTSTR(str);
			count_new.SetWindowTextW(pStr);		//设置新字典计数大小
		}
		else
		{
			zidian2.insert(zidian2.end(), zidian1.begin(), zidian1.end());
			dict_new = zidian2.size();
			ofstream zd("c:\\new.txt");
			for (auto& i : zidian2)
			{
				zd << i + "\n";
			}
			zd.close();
			CString str;
			str.Format(_T("%d"), dict_new);
			LPCTSTR pStr = LPCTSTR(str);
			count_new.SetWindowTextW(pStr);
		}
	}
}

// 筛选按钮，过滤出8位以上的密码
void CDictManageDlg::OnBnClickedFilter()
{
	dict_new = 0;
	string buf = {};
	OnBnClickedCalculate1();
	ofstream fs_o("c:\\new.txt");
	
	for (auto& i : zidian1)
	{
		if (!(i.length() < 8))
		{
			fs_o << i + "\n";
			++dict_new;
		}
	}
	fs_o.close();
	CString str;
	str.Format(_T("%d"), dict_new);				// 将int转PTRSTR类型
	LPCTSTR pStr = LPCTSTR(str);
	count_new.SetWindowTextW(pStr);		//设置新字典计数大小

}
