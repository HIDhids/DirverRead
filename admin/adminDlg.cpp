
// adminDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "admin.h"
#include "adminDlg.h"
#include "afxdialogex.h"
#include"加载卸载驱动.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CadminDlg 对话框



CadminDlg::CadminDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ADMIN_DIALOG, pParent)
	, m_PID(0)
	, m_targetPID(0)//_T("")
	, m_size(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void CadminDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_PID);
	DDX_Text(pDX, IDC_EDIT2, m_targetPID);
	DDX_Text(pDX, IDC_EDIT3, m_size);
}

BEGIN_MESSAGE_MAP(CadminDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CadminDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CadminDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON5, &CadminDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON3, &CadminDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CadminDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IRP_MJ_WRITE, &CadminDlg::OnBnClickedMjWrite)
	ON_BN_CLICKED(IRP_MJ_READ, &CadminDlg::OnBnClickedMjRead)
	ON_BN_CLICKED(IDC_BUTTON7, &CadminDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CadminDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON6, &CadminDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON9, &CadminDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CadminDlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CadminDlg::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON12, &CadminDlg::OnBnClickedButton12)
END_MESSAGE_MAP()


// CadminDlg 消息处理程序

BOOL CadminDlg::OnInitDialog()
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

void CadminDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CadminDlg::OnPaint()
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
HCURSOR CadminDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#include <winioctl.h>
#define 读测试   CTL_CODE(FILE_DEVICE_UNKNOWN, 	0x803, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //控制码测试
#define 写测试   CTL_CODE(FILE_DEVICE_UNKNOWN, 	0x804, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //控制码测试
#define 读写测试   CTL_CODE(FILE_DEVICE_UNKNOWN, 	0x805, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //控制码测试
#define qdname L"\\??\\MyDriver"  //符号链接名
//打开驱动设备对象
static HANDLE DeviceHandle = NULL;
void CadminDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

		// TODO: 在此添加控件通知处理程序代码 //IRP_MJ_CREATE
	DeviceHandle = CreateFileW(
		qdname,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
}



void CadminDlg::OnBnClickedButton2()
{
	CloseHandle(DeviceHandle);
	// TODO: 在此添加控件通知处理程序代码
}


void CadminDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码

	DWORD dwRetSize = 0;//返回字节数

	int inBuf[3] = { 1,2,3 };
	//数组示例
	int OutBuf = 0;//输出缓冲区
	DeviceIoControl(
		DeviceHandle,//CreateFile打开驱动设备 返回的句柄
		读写测试,//控制码 CTL_CODE
		inBuf,//输入缓冲区指针
		sizeof(inBuf),//输入缓冲区大小
		&OutBuf,//返回缓冲区
		sizeof(OutBuf),//返回缓冲区大小
		&dwRetSize, //返回字节数
		NULL);

	//打印返回参数
	CString csStr;
	csStr.Format(L"lkzs:exe R3 读写测试 控制码=%X   输出结果=%d\n", 读写测试, OutBuf);
	OutputDebugStringW(csStr);

}


void CadminDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD dwRetSize = 0;//返回字节数

	char 返回数据[512];
	DWORD 输入缓冲区 = 0;
	DeviceIoControl(
		DeviceHandle,//CreateFile打开驱动设备 返回的句柄
		读测试,//控制码 CTL_CODE
		&输入缓冲区,//输入缓冲区指针
		sizeof(输入缓冲区),//输入缓冲区大小
		返回数据,//返回缓冲区
		sizeof(返回数据),//返回缓冲区大小
		&dwRetSize, //返回字节数
		NULL);

	strcat_s(返回数据, "->EXE R3");
	OutputDebugStringA(返回数据);
}


void CadminDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD dwRetSize = 0;//返回字节数

	char inBuf[] = "DeviceIoControl写测试";
	//数组示例
	DWORD OutBuf[6] = { 0 };//输出缓冲区
	DeviceIoControl(
		DeviceHandle,//CreateFile打开驱动设备 返回的句柄
		写测试,//控制码 CTL_CODE
		inBuf,//输入缓冲区指针
		sizeof(inBuf),//输入缓冲区大小
		&OutBuf,//返回缓冲区
		sizeof(OutBuf),//返回缓冲区大小
		&dwRetSize, //返回字节数
		NULL);

	//打印返回参数
	CString csStr;
	csStr.Format(L"lkzs:exe R3 写测试 控制码=%X\n", 写测试);
	OutputDebugStringW(csStr);
}


void CadminDlg::OnBnClickedMjWrite()
{
	// TODO: 在此添加控件通知处理程序代码
	char 缓冲区[] = "lkzs:R3 ";
	DWORD len = 0;
	BOOL ret = WriteFile(DeviceHandle, 缓冲区, sizeof(缓冲区), &len, 0);

}


void CadminDlg::OnBnClickedMjRead()
{

	// TODO: 在此添加控件通知处理程序代码
	char 输出缓冲区[512] = { 0 };
	DWORD len = 0;//返回实际写入字节数量
	BOOL ret = ReadFile(DeviceHandle, 输出缓冲区, sizeof(输出缓冲区), &len/*返回 实际读取大小*/, 0);
	char buf[256];
	sprintf_s(buf, "ret=%d 输出缓冲区=%s,len=%d  yjx", ret, 输出缓冲区, len);
	OutputDebugStringA(buf);
}
#define 驱动名 "D011_012_NAME"


void CadminDlg::OnBnClickedButton7()
{
	if (LoadDriver(驱动名, "dirHid.sys"))
	{
		MessageBoxA(NULL, "加载成功", "hid", 0);
	}
	else
	{
		MessageBoxA(NULL, "加载失败", "hid", 0);
	}

		// TODO: 在此添加控件通知处理程序代码
}


void CadminDlg::OnBnClickedButton8()
{
	if (UnloadDriver(驱动名))
	{
		MessageBoxA(NULL, "卸载成功", "hid", 0);
	}
	else
	{
		MessageBoxA(NULL, "卸载失败", "hid", 0);
	}
	// TODO: 在此添加控件通知处理程序代码
}

#define IO_添加受保护的PID   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x806, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //控制码测试
#define IO_删除受保护的PID   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x807, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //控制码测试
#define  CTL_IO_通过PID读取数据   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80A, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //控制码测试
#define  CTL_IO_通过PID读取数据2   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80B, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //控制码测试
#define  IO_MOUSE   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80C, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //控制码测试
void CadminDlg::OnBnClickedButton6()
{// TODO: 在此添加控件通知处理程序代码
	//添加保护
	UpdateData(TRUE);//窗口数据更新到变量

	DWORD dwRetSize = 0;//返回字节数

	char buftest[256];
	sprintf_s(buftest, "lkzs:R3层  添加驱动保护");
	OutputDebugStringA(buftest);

	UINT32  传入数据 = m_PID;
	//数组示例
	int OutBuf = 0;
	DeviceIoControl(
		DeviceHandle,//CreateFile打开驱动设备 返回的句柄
		IO_添加受保护的PID,//控制码 CTL_CODE

		&传入数据,//输入缓冲区指针
		sizeof(传入数据),//输入缓冲区大小

		&OutBuf,//输出缓冲区
		sizeof(OutBuf),//返回缓冲区大小

		&dwRetSize, //返回字节数
		NULL);
}


void CadminDlg::OnBnClickedButton9()
{
	// TODO: 在此添加控件通知处理程序代码
	//移出保护

	UpdateData(TRUE);//窗口数据更新到变量

	DWORD dwRetSize = 0;//返回字节数

	char buftest[256];
	sprintf_s(buftest, "lkzs:R3层  移除驱动保护");
	OutputDebugStringA(buftest);
	//int 传入数据[3] = { 3,7,8 };

	UINT32  传入数据 = m_PID;// { 3, 6, 5 };
	//数组示例
	int OutBuf = 0;
	DeviceIoControl(
		DeviceHandle,//CreateFile打开驱动设备 返回的句柄
		IO_删除受保护的PID,//控制码 CTL_CODE

		&传入数据,//输入缓冲区指针
		sizeof(传入数据),//输入缓冲区大小

		&OutBuf,//输出缓冲区
		sizeof(OutBuf),//返回缓冲区大小

		&dwRetSize, //返回字节数
		NULL);
}


void CadminDlg::OnBnClickedButton10()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(1); //m_targetPID

	
	DWORD dwRetSize = 0;//返回字节数

	DWORD64 返回数据 = 0;

	//UINT_PTR 地址 = 0x0FFD10000;
	//数组示例
	UINT64 输入缓冲区[4] = { m_PID ,m_targetPID,0,m_size };
	//DWORD64 m_addr = sizeof(输入缓冲区);

	if (!m_size || !m_PID || !m_targetPID)
	{
		::MessageBoxA(0, "检查输入数据是否正确", "过保护读测试", MB_OK);
		return;
	}
	DeviceIoControl(
		DeviceHandle,//CreateFile打开驱动设备 返回的句柄
		CTL_IO_通过PID读取数据,//控制码 CTL_CODE

		&输入缓冲区,//输入缓冲区指针
		8 * 4,//输入缓冲区大小

		&返回数据,//返回缓冲区
		sizeof(返回数据),//返回缓冲区大小

		&dwRetSize, //返回字节数
		NULL);
	char buf[256];
	sprintf_s(buf, "返回数据=%llX ", 返回数据);
	::MessageBoxA(0, buf, "过保护读测试", MB_OK);
}


void CadminDlg::OnBnClickedButton11()
{
	// TODO: 在此添加控件通知处理程序代码

#pragma pack(push)
#pragma pack(8)
	typedef struct TINPUT_BUF
	{
		UINT64 dwPid;//目标进程PID
		PVOID pBase; //目标进程地址
		UINT64 nSize;//要读取的长度

	}TINPUT_BUF;
#pragma pack(pop)
	//结构

	UpdateData(1); //m_targetPID
	if (!m_size || !m_PID || !m_targetPID)
	{
		::MessageBoxA(0, "检查输入数据是否正确", "过保护读测试", MB_OK);
		return;
	}
	DWORD dwRetSize = 0;//返回字节数

	UINT64 返回数据 = 0;

	UINT_PTR 地址 = 0x400000;// 0x0FFD10000;
	//数组示例
	//UINT64 输入缓冲区[4] = { m_targetPID ,地址,0,4 };
	TINPUT_BUF 输入缓冲区 = { m_PID ,(PVOID)m_targetPID,m_size };
	DeviceIoControl(
		DeviceHandle,//CreateFile打开驱动设备 返回的句柄
		CTL_IO_通过PID读取数据2,//控制码 CTL_CODE
		&输入缓冲区,//输入缓冲区指针
		sizeof(TINPUT_BUF),//输入缓冲区大小

		&返回数据,//返回缓冲区 UserBuffer=&返回数据
		sizeof(返回数据),//返回缓冲区大小

		&dwRetSize, //返回字节数
		NULL);
	char buf[256];
	sprintf_s(buf, "读写2 返回数据=%llX", 返回数据);
	::MessageBoxA(0, buf, "过保护读测试", MB_OK);
}

typedef struct _MOUSE_INPUT_DATA
{

	USHORT UnitId;

	USHORT Flags;

	union {
		ULONG Buttons;
		struct {
			USHORT  ButtonFlags;
			USHORT  ButtonData;
		};
	};

	ULONG RawButtons;

	LONG LastX;

	LONG LastY;

	ULONG ExtraInformation;

} MOUSE_INPUT_DATA, * PMOUSE_INPUT_DATA;
void CadminDlg::OnBnClickedButton12()
{
	MOUSE_INPUT_DATA  mid;
	DWORD dwOutput;
	memset(&mid, 0, sizeof(MOUSE_INPUT_DATA));
	mid.Flags = 0;
	switch (mid.Flags)
	{
	case MOUSE_MOVE_RELATIVE:
		mid.LastX = 100;
		mid.LastY = 100;
		break;
	case MOUSE_MOVE_ABSOLUTE:
		mid.LastX = 100 * 0xffff / GetSystemMetrics(SM_CXSCREEN);
		mid.LastY = 100 * 0xffff / GetSystemMetrics(SM_CYSCREEN);
		break;
	default:
		printf("Flags: Parameter error!\n");
		return ;
	}

	BOOL bRet = DeviceIoControl(DeviceHandle, IO_MOUSE, &mid, sizeof(MOUSE_INPUT_DATA), NULL, 0, &dwOutput, NULL);
	if (!bRet)
		printf("Error! please open the simulate kmclass driver!\n");
	return ;
	// TODO: 在此添加控件通知处理程序代码
}
