//-----------------------------------【程序说明】----------------------------------------------
// 3D小游戏
//作者：刘斌  BIN
//时间：2017年8月
//------------------------------------------------------------------------------------------------

//-----------------------------------【宏定义部分】--------------------------------------------
// 描述：辅助宏
//------------------------------------------------------------------------------------------------
#define WINDOW_TITLE	 _T("王者的荣耀")           //窗口标题
#define WINDOW_WIDTH	1366						//窗口宽度
#define WINDOW_HEIGHT	768							//窗口高度


//-----------------------------------【头文件包含部分】---------------------------------------
//	描述：包含程序所依赖的头文件
//------------------------------------------------------------------------------------------------      
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib, "dinput8.lib")     // 使用DirectInput必须包含的库文件，注意这里有8
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib, "winmm.lib") 

//-----------------------------------【头文件包含部分】---------------------------------------
//	描述：包含头文件
//------------------------------------------------------------------------------------------------                                                                                      
#include <d3d9.h>
#include <d3dx9.h>
#include <tchar.h>
#include <time.h> 
#include <vector>
#include "Direct3DInput.h"											//获取输入信息的类
#include "Camera.h"											//虚拟摄像机的类
#include "Terrain.h"										//地形的类
#include "SkyBox.h"											//天空的类
#include "SnowParticle.h"									//雪花粒子的类
#include "AllocateHierarchy.h"								//骨骼动画的类
#include "D3DGUI.h"											//GUI界面的类
#include "D3DUtil.h"
#include "Character.h"
#include "Arrow.h"
#include "Hero.h"
#include "Tower.h"
#include "Soldier.h"
#include "Creeps.h"

// 菜单页面的宏定义
#define GUI_MAIN_SCREEN       1
#define GUI_START_SCREEN      2
#define GUI_LOAD_SCREEN			3
#define GUI_LOADING_SCREEN     4
#define GUI_OPTION_SCREEN    5

// 设置一些GUI中用到的控件ID
#define STATIC_TEXT_ID     1
#define BUTTON_FIRST_ID    2
#define BUTTON_LOAD_ID  3
#define BUTTON_OPTION_ID  4
#define BUTTON_QUIT_ID     5
#define BUTTON_BACK_ID     6
#define BUTTON_LOADING_ID  7
#define BUTTON_STARTGAME_ID 8
#define BUTTON_HERO1_ID    9
#define BUTTON_HERO2_ID   10
#define BUTTON_MUSIC1_ID  11
#define BUTTON_MUSIC2_ID  12

//游戏状态
#define GAME_MENU        1
#define GAME_LOCAL       2


//-----------------------------------【全局变量声明部分】-------------------------------------
//	描述：全局变量的声明
//------------------------------------------------------------------------------------------------

vector <Character*>						g_vCharacter;
vector<Arrow*>                          g_vArrow;
DWORD									g_currentTrack = DWORD(0);
bool									g_isGuiInit = false;
bool									g_is3DInit = false;
int										g_userModal = MONKEY;
int										g_userIndex = 0;

int										g_currentGamestate = GAME_MENU;		//游戏当前状态
LPDIRECT3DDEVICE9                       g_pd3dDevice = NULL;
LPD3DXFONT								g_pTextFPS = NULL;    //字体COM接口
LPD3DXFONT								g_pTextAdaperName = NULL;  // 显卡信息的2D文本
LPD3DXFONT								g_pTextHelper = NULL;  // 帮助信息的2D文本
LPD3DXFONT								g_pTextInfor = NULL;  // 绘制信息的2D文本

float												g_FPS = 0.0f;       //一个浮点型的变量，代表帧速率
wchar_t										g_strFPS[50] = { 0 };    //包含帧速率的字符数组
wchar_t										g_strAdapterName[60] = { 0 };   //包含显卡名称的字符数组

bool												g_LMBDown = false;      // GUI中的鼠标状态信息，鼠标左键是否按下的标识
int												g_MouseX = 0, g_MouseY = 0;      //存储鼠标坐标的两个变量

D3DXMATRIX								g_matWorld;									//世界矩阵
D3DLIGHT9								g_Light;									//全局光照
Direct3DInput*							g_pDInput = NULL;				//Input类的指针
Camera*									g_pCamera = NULL;				//摄像机类的指针
Terrain*								g_pTerrain = NULL;				//地形类的指针
SkyBox*									g_pSkyBox = NULL;								//天空盒类的指针
SnowParticle*							g_pSnowParticles = NULL;				//雪花粒子系统的指针


//创建四个GUI类对象，分别代表四个页面
D3DGUI		                          *g_MainGUI = NULL;//主窗口
D3DGUI		                          *g_StartGUI = NULL; //游戏开始窗口
D3DGUI		                          *g_LoadGUI = NULL; //游戏载入窗口
D3DGUI	                           	  *g_OptionGUI = NULL; //游戏设置窗口
D3DGUI                                *g_LoadingGUI = NULL;//载入服务器窗口



int						               g_MainGUIFontID = -1;						//  GUI中字体对象的ID
int						               g_StartGUIFontID = -1;						//  GUI中字体对象的ID
int						               g_LoadGUIFontID = -1;						//  GUI中字体对象的ID
int                                    g_LoadingGUIFontID = -1;                  //GUI中字体对象的ID
int					               	   g_OptionGUIFontID = -1;					//  GUI中字体对象的ID
int						               g_currentGUI = GUI_MAIN_SCREEN;    //一个当前的GUI标识

int										g_win = -1;				//是否胜利了


//-----------------------------------【全局函数声明部分】-------------------------------------
//	描述：全局函数声明，防止“未声明的标识”系列错误
//------------------------------------------------------------------------------------------------
LRESULT CALLBACK		            WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
HRESULT						        Direct3D_Init(HWND hwnd, HINSTANCE hInstance);
bool								ObjectsForGui_Init();												//GUI渲染资源初始化函数
bool								ObjectsFor3D_Init();												//3D游戏渲染资源初始化函数
void								Direct3D_Render(HWND hwnd, float fTimeDelta);
void								Direct3D_Update(HWND hwnd, float fTimeDelta);
void								Direct3D_GUI_Render(HWND hwnd, float fTimeDelta);					//Direct3D_GUI渲染函数
void								Direct3D_CleanUp();
float								Get_FPS();
void								HelpText_Render(HWND hwnd);
void								GUICallback(int id, int state);

void								judge(Arrow * arrow);												//判断箭撞击物体


//-----------------------------------【WinMain( )函数】--------------------------------------
//	描述：Windows应用程序的入口函数，我们的程序从这里开始
//------------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//开始设计一个完整的窗口类
	WNDCLASSEX wndClass = { 0 };				//用WINDCLASSEX定义了一个窗口类，即用wndClass实例化了WINDCLASSEX，用于之后窗口的各项初始化    
	wndClass.cbSize = sizeof(WNDCLASSEX);	//设置结构体的字节数大小
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//设置窗口的样式
	wndClass.lpfnWndProc = WndProc;				//设置指向窗口过程函数的指针
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;				//指定包含窗口过程的程序的实例句柄。
	wndClass.hIcon = (HICON)::LoadImage(NULL, _T("GameMedia\\icon.ico"), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE); //从全局的::LoadImage函数从本地加载自定义ico图标
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);    //指定窗口类的光标句柄。
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);  //为hbrBackground成员指定一个灰色画刷句柄
	wndClass.lpszMenuName = NULL;						//用一个以空终止的字符串，指定菜单资源的名字。
	wndClass.lpszClassName = _T("3DGAME");		//用一个以空终止的字符串，指定窗口类的名字。

	if (!RegisterClassEx(&wndClass))				//设计完窗口后，需要对窗口类进行注册，这样才能创建该类型的窗口
		return -1;

	HWND hwnd = CreateWindow(_T("3DGAME"), WINDOW_TITLE,			//喜闻乐见的创建窗口函数CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);


	//Direct3D资源的初始化，调用失败用messagebox予以显示
	if (!(S_OK == Direct3D_Init(hwnd, hInstance)))
	{
		MessageBox(hwnd, _T("Direct3D初始化失败~！"), _T("错误的消息窗口"), 0); //使用MessageBox函数，创建一个消息窗口 
	}

	if (!ObjectsForGui_Init())
	{
		MessageBox(hwnd, _T("GUI资源初始化失败！"), _T("错误信息窗口"), 0);
	}
	g_isGuiInit = true;


	PlaySound(L"GameMedia\\Assassins Creed Theme 刺客信条.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); //循环播放背景音乐 

	MoveWindow(hwnd, 250, 150, WINDOW_WIDTH, WINDOW_HEIGHT, true);   //调整窗口显示时的位置，窗口左上角位于屏幕坐标（200，10）处
	ShowWindow(hwnd, nShowCmd);    //调用Win32函数ShowWindow来显示窗口
	UpdateWindow(hwnd);  //对窗口进行更新，就像我们买了新房子要装修一样

	g_pDInput = new Direct3DInput();
	g_pDInput->Init(hwnd, hInstance, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); //前台，非独占模式


	//消息循环过程
	MSG msg = { 0 };  //初始化msg
	while (msg.message != WM_QUIT)			//使用while循环
	{
			static float fLastTime = (float)::timeGetTime();
			static float fCurrTime = (float)::timeGetTime();
			static float fTimeDelta = 0.0f;
			fCurrTime = (float)::timeGetTime();
			fTimeDelta = (fCurrTime - fLastTime) / 1000.0f;			//一次循环经历的时间
			fLastTime = fCurrTime;

			switch (g_currentGamestate)
			{
			case GAME_MENU:
			{
				if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))   //查看消息队列，有则发送消息
				{
					TranslateMessage(&msg);					//将虚拟键消息转换为字符消息
					DispatchMessage(&msg);					//发送消息给窗口程序。
				}
				else
				{
					Direct3D_GUI_Render(hwnd, fTimeDelta);
				}
			}
			break;
			case GAME_LOCAL:										//游戏进行时的画面，确保游戏渲染的不会受鼠标不断移动影响，
																	//应该每次循环都进行游戏画面的更新和渲染
			{
				if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))   //查看消息队列，有则发送消息
				{
					TranslateMessage(&msg);					//将虚拟键消息转换为字符消息
					DispatchMessage(&msg);					//发送消息给窗口程序。
				}
				Direct3D_Update(hwnd, fTimeDelta);				//画面的更新
				Direct3D_Render(hwnd, fTimeDelta);				//画面的渲染			
			}
			break;
			default:
			{
				Direct3D_GUI_Render(hwnd, fTimeDelta);
			}
			}
		}

	UnregisterClass(_T("3DGAME"), wndClass.hInstance);
	return 0;

}


//-----------------------------------【Direct3D_Init( )函数】----------------------------------
//	描述：Direct3D初始化函数
//---------------------------------------------------------------------------------------------
HRESULT Direct3D_Init(HWND hwnd, HINSTANCE hInstance)
{

	//--------------------------------------------------------------------------------------
	// 【Direct3D初始化四步曲之一，创接口】：创建Direct3D接口对象, 以便用该Direct3D对象创建Direct3D设备对象
	//--------------------------------------------------------------------------------------
	LPDIRECT3D9  pD3D = NULL; //Direct3D接口对象的创建
	if (NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION))) //初始化Direct3D接口对象，并进行DirectX版本协商
		return E_FAIL;

	//--------------------------------------------------------------------------------------
	// 【Direct3D初始化四步曲之二,取信息】：获取硬件设备信息
	//--------------------------------------------------------------------------------------
	D3DCAPS9 caps; int vp = 0;
	if (FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
	{
		return E_FAIL;
	}
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;   //支持硬件顶点运算，我们就采用硬件顶点运算，妥妥的
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING; //不支持硬件顶点运算，无奈只好采用软件顶点运算

	 //--------------------------------------------------------------------------------------
	 // 【Direct3D初始化四步曲之三，填内容】：填充D3DPRESENT_PARAMETERS结构体
	 //--------------------------------------------------------------------------------------
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = WINDOW_WIDTH;
	d3dpp.BackBufferHeight = WINDOW_HEIGHT;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 2;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.Windowed = true;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	//--------------------------------------------------------------------------------------
	// 【Direct3D初始化四步曲之四，创设备】：创建Direct3D设备接口
	//--------------------------------------------------------------------------------------
	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		hwnd, vp, &d3dpp, &g_pd3dDevice)))
		return E_FAIL;


	//获取显卡信息到g_strAdapterName中，并在显卡名称之前加上“当前显卡型号：”字符串
	wchar_t TempName[60] = L"当前显卡型号：";   //定义一个临时字符串，且方便了把"当前显卡型号："字符串引入我们的目的字符串中
	D3DADAPTER_IDENTIFIER9 Adapter;  //定义一个D3DADAPTER_IDENTIFIER9结构体，用于存储显卡信息
	pD3D->GetAdapterIdentifier(0, 0, &Adapter);//调用GetAdapterIdentifier，获取显卡信息
	int len = MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, NULL, 0);//显卡名称现在已经在Adapter.Description中了，但是其为char类型，我们要将其转为wchar_t类型
	MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, g_strAdapterName, len);//这步操作完成后，g_strAdapterName中就为当前我们的显卡类型名的wchar_t型字符串了
	wcscat_s(TempName, g_strAdapterName);//把当前我们的显卡名加到“当前显卡型号：”字符串后面，结果存在TempName中
	wcscpy_s(g_strAdapterName, TempName);//把TempName中的结果拷贝到全局变量g_strAdapterName中，大功告成~


	SAFE_RELEASE(pD3D) //LPDIRECT3D9接口对象的使命完成，我们将其释放掉

		return S_OK;
}


//-----------------------------------【ObjectsForGui_Init( )函数】---------------------------------
//	描述：渲染资源初始化函数
//-------------------------------------------------------------------------------------------------
bool ObjectsForGui_Init()
{
	//加载音乐资源
	mciSendString(L"open GameMedia\\射箭.wav alias Fight", NULL, 0, NULL);
	mciSendString(L"open GameMedia\\击中.wav alias Hit", NULL, 0, NULL);
	mciSendString(L"open GameMedia\\点击.mp3 alias Click", NULL, 0, NULL);
	mciSendString(L"open GameMedia\\胜利.wav alias Victory", NULL, 0, NULL);
	mciSendString(L"open GameMedia\\失败.wav alias Defeate", NULL, 0, NULL);

	//创建字体
	D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1000, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("Calibri"), &g_pTextFPS);
	D3DXCreateFont(g_pd3dDevice, 20, 0, 1000, 0, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"华文中宋", &g_pTextAdaperName);
	D3DXCreateFont(g_pd3dDevice, 23, 0, 1000, 0, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"微软雅黑", &g_pTextHelper);
	D3DXCreateFont(g_pd3dDevice, 26, 0, 1000, 0, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"黑体", &g_pTextInfor);


	//设置纹理采样参数
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	//--------------------------------------------------【GUI系统相关代码】-------------------------------------------------------

	// 创建一些GUI系统
	g_MainGUI = new D3DGUI(g_pd3dDevice, WINDOW_WIDTH, WINDOW_HEIGHT); //主菜单页面
	g_StartGUI = new D3DGUI(g_pd3dDevice, WINDOW_WIDTH, WINDOW_HEIGHT); //开始新游戏页面
	g_LoadGUI = new D3DGUI(g_pd3dDevice, WINDOW_WIDTH, WINDOW_HEIGHT); //载入游戏页面
	g_OptionGUI = new D3DGUI(g_pd3dDevice, WINDOW_WIDTH, WINDOW_HEIGHT); //设置页面
	g_LoadingGUI = new D3DGUI(g_pd3dDevice, WINDOW_WIDTH, WINDOW_HEIGHT);//载入服务器页面


	// 给五个页面分别添加背景图
	if (!g_MainGUI->AddBackground(L"GameMedia/GameMenu.png")) return false;
	if (!g_StartGUI->AddBackground(L"GameMedia/GameStart.jpg")) return false;
	if (!g_LoadGUI->AddBackground(L"GameMedia/GameLoad.jpg")) return false;
	if (!g_OptionGUI->AddBackground(L"GameMedia/GameOption.jpg")) return false;
	if (!g_LoadingGUI->AddBackground(L"GameMedia/GameLoading.jpg"))return false;

	// 分别给四个页面添加字体
	if (!g_MainGUI->CreateTextFont(L"浪漫雅圆", 28, &g_MainGUIFontID)) return false;
	if (!g_StartGUI->CreateTextFont(L"微软雅黑", 38, &g_StartGUIFontID)) return false;
	if (!g_LoadGUI->CreateTextFont(L"微软雅黑", 38, &g_LoadGUIFontID)) return false;
	if (!g_LoadingGUI->CreateTextFont(L"微软雅黑", 38, &g_LoadingGUIFontID))return false;
	if (!g_OptionGUI->CreateTextFont(L"微软雅黑", 38, &g_OptionGUIFontID)) return false;




	//---------------------------------【主菜单main页面相关的页面布局代码】---------------------------------
	// 添加静态文本到页面中
	if (!g_MainGUI->AddStaticText(STATIC_TEXT_ID, L"Version BIN1.0版",
		1100, 735, D3DCOLOR_XRGB(255, 65, 205), g_MainGUIFontID)) return false;


	// 添加4个按钮，分别是开始游戏，载入进度，选项和退出游戏，每个按钮对应3幅图
	if (!g_MainGUI->AddButton(BUTTON_FIRST_ID, 567, 450, L"GameMedia\\FirstUp.png",
		L"GameMedia\\FirstOver.png", L"GameMedia\\FirstDown.png")) return false;

	if (!g_MainGUI->AddButton(BUTTON_LOAD_ID, 565, 515, L"GameMedia\\onlineUp.png",
		L"GameMedia\\onlineOver.png", L"GameMedia\\onlineDown.png")) return false;

	if (!g_MainGUI->AddButton(BUTTON_OPTION_ID, 565, 580, L"GameMedia\\optionUp.png",
		L"GameMedia\\optionOver.png", L"GameMedia\\optionDown.png")) return false;

	if (!g_MainGUI->AddButton(BUTTON_QUIT_ID, 568, 640, L"GameMedia\\quitUp.png",
		L"GameMedia\\quitOver.png", L"GameMedia\\quitDown.png")) return false;


	//------------------------【单人游戏页面相关的页面布局代码】------------------------
	// 添加按钮到页面中

	if (!g_StartGUI->AddStaticText(STATIC_TEXT_ID, L"请选择你的英雄",
		380, 360, D3DCOLOR_XRGB(253, 25, 55), g_StartGUIFontID)) return false;
	if (!g_StartGUI->AddButton(BUTTON_STARTGAME_ID, 950, 500, L"GameMedia/startUp.png", L"GameMedia/startOver.png",
		L"GameMedia/startDown.png")) return false;
	if (!g_StartGUI->AddButton(BUTTON_BACK_ID, 1000, 570, L"GameMedia/backUp.png", L"GameMedia/backOver.png",
		L"GameMedia/backDown.png")) return false;
	if (!g_StartGUI->AddButton(BUTTON_HERO1_ID, 200, 420, L"GameMedia/孙悟空Up.png", L"GameMedia/孙悟空Over.png",
		L"GameMedia/孙悟空Down.png")) return false;
	if (!g_StartGUI->AddButton(BUTTON_HERO2_ID, 500, 420, L"GameMedia/弓箭手Up.jpg", L"GameMedia/弓箭手Over.jpg",
		L"GameMedia/弓箭手Down.jpg")) return false;

	if (!g_StartGUI->AddStaticText(STATIC_TEXT_ID, L"没头的孙悟空",
		250, 700, D3DCOLOR_XRGB(255, 25, 55), g_StartGUIFontID)) return false;
	if (!g_StartGUI->AddStaticText(STATIC_TEXT_ID, L"弓箭手",
		580, 700, D3DCOLOR_XRGB(2, 25, 255), g_StartGUIFontID)) return false;

	//------------------------【多人游戏load页面相关的页面布局代码】------------------------
	// 添加按钮到页面中
	if (!g_LoadGUI->AddButton(BUTTON_LOADING_ID, 1000, 500, L"GameMedia/connectUp.png", L"GameMedia/connectOver.png",
		L"GameMedia/connectDown.png")) return false;
	if (!g_LoadGUI->AddButton(BUTTON_BACK_ID, 1000, 570, L"GameMedia/backUp.png", L"GameMedia/backOver.png",
		L"GameMedia/backDown.png")) return false;


	//------------------------【多人游戏载入服务器loading页面相关的页面布局代码】------------------------

	//添加静态文本到页面中
	if (!g_LoadingGUI->AddStaticText(STATIC_TEXT_ID, L"正在为您拼命加载服务器（*/∇＼*）......",
		310, 340, D3DCOLOR_XRGB(255, 255, 255), g_LoadingGUIFontID)) return false;
	if (!g_LoadingGUI->AddStaticText(STATIC_TEXT_ID, L"请您耐心等待.......",
		380, 400, D3DCOLOR_XRGB(255, 25, 255), g_LoadingGUIFontID)) return false;

	// 添加按钮到页面中
	if (!g_LoadingGUI->AddButton(BUTTON_BACK_ID, 1000, 550, L"GameMedia/backUp.png", L"GameMedia/backOver.png",
		L"GameMedia/backDown.png")) return false;


	//------------------------【游戏设置option页面相关的页面布局代码】------------------------
	//添加静态文本到页面中
	if (!g_OptionGUI->AddStaticText(STATIC_TEXT_ID, L"游戏设置",
		300, 80, D3DCOLOR_XRGB(255, 155, 0), g_OptionGUIFontID)) return false;
	if (!g_OptionGUI->AddStaticText(STATIC_TEXT_ID, L"操作说明:",
		100, 150, D3DCOLOR_XRGB(173, 255, 255), g_OptionGUIFontID)) return false;
	if (!g_OptionGUI->AddStaticText(STATIC_TEXT_ID, L"上：W",
		250, 200, D3DCOLOR_XRGB(255, 255, 255), g_OptionGUIFontID)) return false;
	if (!g_OptionGUI->AddStaticText(STATIC_TEXT_ID, L"下：S",
		250, 250, D3DCOLOR_XRGB(255, 255, 255), g_OptionGUIFontID)) return false;
	if (!g_OptionGUI->AddStaticText(STATIC_TEXT_ID, L"左：A",
		100, 225, D3DCOLOR_XRGB(255, 255, 255), g_OptionGUIFontID)) return false;
	if (!g_OptionGUI->AddStaticText(STATIC_TEXT_ID, L"右：D",
		400, 225, D3DCOLOR_XRGB(255, 255, 255), g_OptionGUIFontID)) return false;
	if (!g_OptionGUI->AddStaticText(STATIC_TEXT_ID, L"加速：SHIFT",
		210, 300, D3DCOLOR_XRGB(255, 255, 255), g_OptionGUIFontID)) return false;
	if (!g_OptionGUI->AddStaticText(STATIC_TEXT_ID, L"射击：J",
		210, 350, D3DCOLOR_XRGB(255, 255, 255), g_OptionGUIFontID)) return false;
	if (!g_OptionGUI->AddStaticText(STATIC_TEXT_ID, L"请选择你的BGM！",
		250, 400, D3DCOLOR_XRGB(253, 53, 55), g_OptionGUIFontID)) return false;

	// 添加按钮到页面中
	if (!g_OptionGUI->AddButton(BUTTON_BACK_ID, 1000, 600, L"GameMedia/backUp.png", L"GameMedia/backOver.png",
		L"GameMedia/backDown.png")) return false;
	if (!g_OptionGUI->AddButton(BUTTON_MUSIC1_ID, 120, 450, L"GameMedia/Music1Up.png", L"GameMedia/Music1Over.png",
		L"GameMedia/Music1Down.png")) return false;
	if (!g_OptionGUI->AddButton(BUTTON_MUSIC2_ID, 400, 450, L"GameMedia/Music2Up.png", L"GameMedia/Music2Over.png",
		L"GameMedia/Music2Down.png")) return false;

	return true;
}


//-----------------------------------【ObjectsFor3D_Init( )函数】--------------------------------------
//	描述：渲染资源初始化函数
//--------------------------------------------------------------------------------------------------
bool ObjectsFor3D_Init()
{
	//--------------------------------------------------【游戏相关代码】-------------------------------------------------------

	//创建 初始化地形
	g_pTerrain = new Terrain(g_pd3dDevice);
	g_pTerrain->LoadTerrainFromFile(L"GameMedia\\hightMap.raw", L"GameMedia\\地图.png");//从文件加载高度图和纹理
	g_pTerrain->InitTerrain(200, 200, 100.0f, 10.0f, 1.0f, 1.0f);  //四个值分别是顶点行数，顶点列数，顶点间间距，高度图缩放系数,横，竖用多少张纹理图贴图


	 // 设置光照  
	::ZeroMemory(&g_Light, sizeof(g_Light));
	g_Light.Type = D3DLIGHT_DIRECTIONAL;
	g_Light.Ambient = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);
	g_Light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	g_Light.Specular = D3DXCOLOR(0.9f, 0.9f, 0.9f, 1.0f);
	g_Light.Direction = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	g_pd3dDevice->SetLight(0, &g_Light);
	g_pd3dDevice->LightEnable(0, true);
	g_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, true);

	// 创建并初始化虚拟摄像机
	g_pCamera = new Camera(g_pd3dDevice);
	g_pCamera->SetCameraPosition(&D3DXVECTOR3(0.0f, 300.0f, -800.0f));  //设置摄像机所在的位置
	g_pCamera->SetTargetPosition(&D3DXVECTOR3(0.0f, 400.0f, 0.0f));		//设置目标观察点所在的位置
	g_pCamera->SetViewMatrix();											//设置取景变换矩阵
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 1.0f, 1.0f, 200000.0f);
	g_pCamera->SetProjMatrix(&matProj);

	//创建并初始化天空对象
	g_pSkyBox = new SkyBox(g_pd3dDevice);
	g_pSkyBox->LoadSkyTextureFromFile(L"GameMedia\\SunSetFront2048.png", L"GameMedia\\SunSetBack2048.png", L"GameMedia\\SunSetRight2048.png", L"GameMedia\\SunSetLeft2048.png", L"GameMedia\\SunSetUp2048.png");//从文件加载前、后、左、右、顶面5个面的纹理图
	g_pSkyBox->InitSkyBox(50000);  //设置天空盒的边长
	
	//创建并初始化雪花粒子系统  
	g_pSnowParticles = new SnowParticle(g_pd3dDevice);
	g_pSnowParticles->InitSnowParticle();

	// 创建骨骼动画
	//塔
	ifstream infileForTower("GameMedia\\towerInfo.txt");
	if (!infileForTower)
	{

		MessageBox(NULL, L"towerInfo.txt 文件打开失败", L"文件打开失败", 0);
	}
	for (int i = 0; i < 14; i++)
	{
		Tower * TempChar = new Tower(i);
		g_vCharacter.push_back(TempChar);
		D3DXVECTOR3 tower;
		float towerFacing;
		int team;
		infileForTower >> tower.x >> tower.y >> tower.z >> towerFacing >> team;
		g_vCharacter[g_vCharacter.size() - 1]->id = g_vCharacter.size() - 1;
		g_vCharacter[g_vCharacter.size() - 1]->team = (Team)team;
		g_vCharacter[g_vCharacter.size() - 1]->species = 1;
		g_vCharacter[g_vCharacter.size() - 1]->Init(g_pd3dDevice, g_pDInput, g_pTerrain, g_pCamera, &g_vCharacter);
		g_vCharacter[g_vCharacter.size() - 1]->setControlByUser(false);
		g_vCharacter[g_vCharacter.size() - 1]->setPosition(tower);
		g_vCharacter[g_vCharacter.size() - 1]->setFacing(towerFacing);
	}
	//玩家英雄
	Hero * TempChar = new Hero(g_userModal);
	g_vCharacter.push_back(TempChar);
	g_vCharacter[g_vCharacter.size() - 1]->id = g_vCharacter.size() - 1;
	g_vCharacter[g_vCharacter.size() - 1]->species = 0;
	g_vCharacter[g_vCharacter.size() - 1]->team = Red;
	g_vCharacter[g_vCharacter.size() - 1]->Init(g_pd3dDevice, g_pDInput, g_pTerrain, g_pCamera, &g_vCharacter);
	g_vCharacter[g_vCharacter.size() - 1]->setControlByUser(true);
	g_userIndex = g_vCharacter.size() - 1;
	g_vCharacter[g_vCharacter.size() - 1]->setPosition(D3DXVECTOR3(-7565.6, 0.0, -7190.5));

	//敌军英雄
	TempChar = new Hero(rand() % 2 == 1 ?   MONKEY:ARCHER);
	g_vCharacter.push_back(TempChar);
	g_vCharacter[g_vCharacter.size() - 1]->id = g_vCharacter.size() - 1;
	g_vCharacter[g_vCharacter.size() - 1]->species = 0;
	g_vCharacter[g_vCharacter.size() - 1]->team = Blue;
	g_vCharacter[g_vCharacter.size() - 1]->Init(g_pd3dDevice, g_pDInput, g_pTerrain, g_pCamera, &g_vCharacter);
	g_vCharacter[g_vCharacter.size() - 1]->setControlByUser(false);
	g_vCharacter[g_vCharacter.size() - 1]->setPosition(D3DXVECTOR3(3516.8, 0.0, 3393.6));

	//我方小兵
	for (int i = 0; i < 3; i++)
	{
		Soldier * TempChar = new Soldier(i);
		g_vCharacter.push_back(TempChar);
		g_vCharacter[g_vCharacter.size() - 1]->setControlByUser(false);
		g_vCharacter[g_vCharacter.size() - 1]->id = g_vCharacter.size() - 1;
		g_vCharacter[g_vCharacter.size() - 1]->species = 2;
		g_vCharacter[g_vCharacter.size() - 1]->team = Red;
		g_vCharacter[g_vCharacter.size() - 1]->Init(g_pd3dDevice, g_pDInput, g_pTerrain, g_pCamera, &g_vCharacter);
	}

	for (int i = 0; i < 3; i++)
	{
		Soldier * TempChar = new Soldier(i);
		g_vCharacter.push_back(TempChar);
		g_vCharacter[g_vCharacter.size() - 1]->setControlByUser(false);
		g_vCharacter[g_vCharacter.size() - 1]->id = g_vCharacter.size() - 1;
		g_vCharacter[g_vCharacter.size() - 1]->species = 2;
		g_vCharacter[g_vCharacter.size() - 1]->team = Blue;
		g_vCharacter[g_vCharacter.size() - 1]->Init(g_pd3dDevice, g_pDInput, g_pTerrain, g_pCamera, &g_vCharacter);
	}

	//野怪
	//1
	Creeps * TempChar1 = new Creeps(D3DXVECTOR3(-6095.5, 0.0, -808.3));
	g_vCharacter.push_back(TempChar1);
	g_vCharacter[g_vCharacter.size() - 1]->setControlByUser(false);
	g_vCharacter[g_vCharacter.size() - 1]->id = g_vCharacter.size() - 1;
	g_vCharacter[g_vCharacter.size() - 1]->species = 3;
	g_vCharacter[g_vCharacter.size() - 1]->team = Creep;
	g_vCharacter[g_vCharacter.size() - 1]->Init(g_pd3dDevice, g_pDInput, g_pTerrain, g_pCamera, &g_vCharacter);

	//2
	Creeps * TempChar2 = new Creeps(D3DXVECTOR3(500.8, 0.0, -5888.7));
	g_vCharacter.push_back(TempChar2);
	g_vCharacter[g_vCharacter.size() - 1]->setControlByUser(false);
	g_vCharacter[g_vCharacter.size() - 1]->position = D3DXVECTOR3(5.8, 0.0, -6488.7);
	g_vCharacter[g_vCharacter.size() - 1]->id = g_vCharacter.size() - 1;
	g_vCharacter[g_vCharacter.size() - 1]->species = 3;
	g_vCharacter[g_vCharacter.size() - 1]->team = Creep;
	g_vCharacter[g_vCharacter.size() - 1]->Init(g_pd3dDevice, g_pDInput, g_pTerrain, g_pCamera, &g_vCharacter);

	//3
	Creeps * TempChar3 = new Creeps(D3DXVECTOR3(5799.3, 0.0, 668.9));
	g_vCharacter.push_back(TempChar3);
	g_vCharacter[g_vCharacter.size() - 1]->setControlByUser(false);
	g_vCharacter[g_vCharacter.size() - 1]->id = g_vCharacter.size() - 1;
	g_vCharacter[g_vCharacter.size() - 1]->species = 3;
	g_vCharacter[g_vCharacter.size() - 1]->team = Creep;
	g_vCharacter[g_vCharacter.size() - 1]->Init(g_pd3dDevice, g_pDInput, g_pTerrain, g_pCamera, &g_vCharacter);

	//4
	Creeps * TempChar4 = new Creeps(D3DXVECTOR3(269.2, 0.0, 6481.9));
	g_vCharacter.push_back(TempChar4);
	g_vCharacter[g_vCharacter.size() - 1]->setControlByUser(false);
	g_vCharacter[g_vCharacter.size() - 1]->id = g_vCharacter.size() - 1;
	g_vCharacter[g_vCharacter.size() - 1]->species = 3;
	g_vCharacter[g_vCharacter.size() - 1]->team = Creep;
	g_vCharacter[g_vCharacter.size() - 1]->Init(g_pd3dDevice, g_pDInput, g_pTerrain, g_pCamera, &g_vCharacter);


	return true;


}

//-----------------------------------【WndProc( )函数】--------------------------------------
//	描述：窗口过程函数WndProc,对窗口消息进行处理
//------------------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (g_currentGamestate == GAME_MENU)			//在菜单状态
	{
		switch (message)
		{
		case WM_LBUTTONDOWN:					//鼠标左键按下
			g_LMBDown = true;
			break;

		case WM_LBUTTONUP:						//鼠标左键弹起
			g_LMBDown = false;
			break;

		case WM_MOUSEMOVE:						//鼠标移动
			g_MouseX = LOWORD(lParam);
			g_MouseY = HIWORD(lParam);
			break;

		case WM_KEYDOWN:						//键盘按下消息
			if (wParam == VK_ESCAPE)			//ESC键
			{
				if (g_currentGUI == GUI_MAIN_SCREEN)
					DestroyWindow(hwnd);		// 销毁窗口, 并发送一条WM_DESTROY消息
				else
				{
					g_currentGUI = GUI_MAIN_SCREEN;
				}
			}
			break;

		case WM_PAINT:							//客户区重绘消息
			Direct3D_GUI_Render(hwnd, 0.0f);		//Direct3D_GUI_Render函数，进行画面的绘制
			ValidateRect(hwnd, NULL);			//更新客户区的显示
			break;

		case WM_DESTROY:						//窗口销毁消息
			Direct3D_CleanUp();					//Direct3D_CleanUp函数，清理COM接口对象
			PostQuitMessage(0);				//向系统表明有个线程有终止请求。用来响应WM_DESTROY消息
			break;

		default:
			return DefWindowProc(hwnd, message, wParam, lParam);		//其他设置为默认
		}
	}
	else if (g_currentGamestate == GAME_LOCAL)
	{
		switch (message)
		{
		case WM_KEYUP:							//键盘按下消息
			if (wParam == VK_ESCAPE)			//ESC键
			{
				g_currentGamestate = GAME_MENU;
				g_currentGUI = GUI_MAIN_SCREEN;
			}
			break;

		case WM_PAINT:							//客户区重绘消息
			Direct3D_Render(hwnd, 0.0f);			//Direct3D_Render函数，进行画面的绘制
			ValidateRect(hwnd, NULL);			//更新客户区的显示
			break;

		case WM_DESTROY:						//窗口销毁消息
			Direct3D_CleanUp();					//Direct3D_CleanUp函数，清理COM接口对象
			PostQuitMessage(0);				//向系统表明有个线程有终止请求。用来响应WM_DESTROY消息
			break;

		default:
			return DefWindowProc(hwnd, message, wParam, lParam);		//其他调用默认
		}
	}

	return 0;
}


//-----------------------------------【GUICallback( )函数】---------------------------------------
//	描述：GUI系统的回调函数，填写按钮按下后的相关处理代码
//------------------------------------------------------------------------------------------------
void GUICallback(int id, int state)
{
	if (state == UGP_BUTTON_DOWN)
		mciSendString(L"play Click from 0", NULL, 0, NULL);
	switch (id)
	{
	case BUTTON_FIRST_ID:   //start game开始游戏按钮
		if (state == UGP_BUTTON_DOWN)
		{
			g_currentGUI = GUI_START_SCREEN;
			g_LMBDown = false;
			g_MouseX = 0;
			g_MouseY = 0;
		}
		break;

	case BUTTON_LOAD_ID:  //load game载入游戏按钮
		if (state == UGP_BUTTON_DOWN)
			g_currentGUI = GUI_LOAD_SCREEN;
		break;

	case BUTTON_STARTGAME_ID://start game开始游戏页面中，开始游戏按钮
		if (state == UGP_BUTTON_DOWN)
		{
			if (!g_is3DInit)
			{
				if (!ObjectsFor3D_Init())
				{
					MessageBox(NULL, _T("3D游戏资源初始化失败~！"), _T("错误信息窗口"), 0);
				}
				g_is3DInit = true;
			}
			g_currentGamestate = GAME_LOCAL;
		}

	case BUTTON_LOADING_ID:  //loading服务器按钮
		if (state == UGP_BUTTON_DOWN)
			g_currentGUI = GUI_LOADING_SCREEN;
		break;

	case BUTTON_OPTION_ID: //option设置按钮
		if (state == UGP_BUTTON_DOWN)
			g_currentGUI = GUI_OPTION_SCREEN;
		break;

	case BUTTON_BACK_ID: //back返回按钮
		if (state == UGP_BUTTON_DOWN)
			g_currentGUI = GUI_MAIN_SCREEN;
		break;

	case BUTTON_QUIT_ID://quit退出按钮
		if (state == UGP_BUTTON_DOWN)
			PostQuitMessage(0);
		break;
	case  BUTTON_MUSIC1_ID:
		if (state == UGP_BUTTON_DOWN)
			PlaySound(L"GameMedia\\See You Again - Wizard See,Clone.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);  //循环播放背景音乐
		break;

	case  BUTTON_MUSIC2_ID:
		if (state == UGP_BUTTON_DOWN)
			PlaySound(L"GameMedia\\Free Loop - Daniel Powter.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);   //循环播放背景音乐
		break;

	case BUTTON_HERO1_ID:
		if (state == UGP_BUTTON_DOWN)
			g_userModal = MONKEY;
		break;
	case BUTTON_HERO2_ID:
		if (state == UGP_BUTTON_DOWN)
			g_userModal = ARCHER;
		break;
	}
	g_LMBDown = false;
}


//-----------------------------------【Direct3D_Update( )函数】--------------------------------
//	描述：各种数据的更新
//--------------------------------------------------------------------------------------------------
void Direct3D_Update(HWND hwnd, float fTimeDelta)
{
	//使用DirectInput类读取数据  
	g_pDInput->GetInput();

	// 沿摄像机各分量移动视角  
	if (g_pDInput->IsKeyDown(DIK_A))  g_pCamera->MoveAlongRightVec(-20.0f);
	if (g_pDInput->IsKeyDown(DIK_D))  g_pCamera->MoveAlongRightVec(20.0f);
	if (g_pDInput->IsKeyDown(DIK_W))  g_pCamera->MoveAlongLookVec(20.0f);
	if (g_pDInput->IsKeyDown(DIK_S))  g_pCamera->MoveAlongLookVec(-20.0f);
	if (g_pDInput->IsKeyDown(DIK_R))  g_pCamera->MoveAlongUpVec(10.0f);
	if (g_pDInput->IsKeyDown(DIK_F))  g_pCamera->MoveAlongUpVec(-10.0f);

	//沿摄像机各分量旋转视角  
	if (g_pDInput->IsKeyDown(DIK_LEFT)) g_pCamera->RotationUpVec(-0.003f);
	if (g_pDInput->IsKeyDown(DIK_RIGHT))g_pCamera->RotationUpVec(0.003f);
	if (g_pDInput->IsKeyDown(DIK_UP))	g_pCamera->RotationRightVec(-0.003f);
	if (g_pDInput->IsKeyDown(DIK_DOWN)) g_pCamera->RotationRightVec(0.003f);
	if (g_pDInput->IsKeyDown(DIK_Q))	g_pCamera->RotationLookVec(0.001f);
	if (g_pDInput->IsKeyDown(DIK_E))	g_pCamera->RotationLookVec(-0.001f);

	//鼠标控制右向量和上向量的旋转  
	g_pCamera->RotationUpVec(g_pDInput->MouseDX()* 0.0003f);
	g_pCamera->RotationRightVec(g_pDInput->MouseDY() * 0.0003f);

	//鼠标滚轮控制观察点收缩操作  
	static float fPosZ = 0.0f;
	fPosZ += g_pDInput->MouseDZ()*0.03f;

	if (g_pDInput->IsKeyDown(DIK_C))
	{
		g_vCharacter[g_userIndex]->setControlByUser(false);
		g_userIndex++;
		if (g_userIndex > g_vCharacter.size() - 1)
			g_userIndex = 0;
		g_vCharacter[g_userIndex]->setControlByUser(true);
		Sleep(1000);

	}

	if (!g_pDInput->IsKeyDown(DIK_SPACE))
	{
		D3DXVECTOR3 vCharPos;
		g_vCharacter[g_userIndex]->getPosition(&vCharPos);
		g_pCamera->FollowPosition(&vCharPos);
	}
	// 计算并设置取景变换矩阵
		D3DXMATRIX matView;
	g_pCamera->CalculateViewMatrix(&matView);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	//把正确的世界变换矩阵存到g_matWorld中  
	D3DXMatrixTranslation(&g_matWorld, 0.0f, 0.0f, fPosZ);


	while (ShowCursor(false) >= 0);		//隐藏鼠标光标

	srand((unsigned int)timeGetTime());
	vector<Character * > ::iterator it; //控制更新各种角色
	for (it = g_vCharacter.begin(); it != g_vCharacter.end(); it++)
	{
		(*it)->Control(fTimeDelta);
		(*it)->Update();
	}

	vector <Arrow * >::iterator it1;//判断箭是否击中，并更新箭状态
	for (it1 = g_vArrow.begin(); it1 != g_vArrow.end(); it1++)
	{
		judge(*it1);
		(*it1)->Update(fTimeDelta);
	}

	bool red = false, blue = false;//判断输赢

	for (it = g_vCharacter.begin(); it != g_vCharacter.end(); it++)
	{
		if ((*it)->species == 1 && ((*it)->Type == 0 || (*it)->Type == 13))
		{
			if ((*it)->alive==false)
			{
				if ((*it)->team == Red)
					blue = true;
				else if ((*it)->team == Blue)
					red = true;
			}
		}
	}

	if (red && !blue)
	{
		g_win = 0;
		{
			g_currentGUI = GUI_MAIN_SCREEN;
			ProcessGUI(g_MainGUI, g_LMBDown, g_MouseX,
				g_MouseY, GUICallback);
		}
		mciSendString(L"play Victory from 0", NULL, 0, NULL);
	}
	if (!red && blue)
	{
		g_win = 1;
		{
			g_currentGUI = GUI_MAIN_SCREEN;
			ProcessGUI(g_MainGUI, g_LMBDown, g_MouseX,
			g_MouseY, GUICallback);
		}
		mciSendString(L"play Defeate from 0", NULL, 0, NULL);
	}

}


//-----------------------------------【judge(FireBall * ball)函数】-------------------------------
//	描述：判断火球撞击物体
//--------------------------------------------------------------------------------------------------
void  judge(Arrow * arrow)
{
	if (arrow->isDestroy)
		return;

	bool hit = false;
	for (int i = 0; i < g_vCharacter.size(); i++)
	{
		if (arrow->index == g_vCharacter[i]->id)
			continue;
		if (arrow->m_team == g_vCharacter[i]->team)
			continue;
		if (!g_vCharacter[i]->alive)
			continue;

		if (D3DXVec3Length(&(arrow->position - g_vCharacter[i]->position)) < arrow->ArrowRadius + g_vCharacter[i]->modalRadius)
		{
			hit = true;
			if (g_vCharacter[arrow->index]->atk  > g_vCharacter[i]->def)
				g_vCharacter[i]->hp -= g_vCharacter[arrow->index]->atk - g_vCharacter[i]->def;
			if (g_vCharacter[i]->hp <= 0)
			{
				g_vCharacter[i]->alive = false;
				switch (g_vCharacter[i]->species)
				{
				case 0:
					g_vCharacter[i]->timeForResurrection = g_vCharacter[i]->level * 5.0f;
				case 2:
					g_vCharacter[i]->timeForResurrection = g_vCharacter[i]->level * 4.0f;
				case 3:
					g_vCharacter[i]->timeForResurrection = g_vCharacter[i]->level * 10.0f;
				}
				g_vCharacter[arrow->index]->exp += totalExp[g_vCharacter[i]->level - 1] / 2;
				if (g_vCharacter[arrow->index]->exp > totalExp[g_vCharacter[arrow->index]->level - 1])
				{
					g_vCharacter[arrow->index]->exp -= totalExp[g_vCharacter[arrow->index]->level - 1];
					g_vCharacter[arrow->index]->level += 1;
					if (g_vCharacter[arrow->index]->level > 15)
						g_vCharacter[arrow->index]->level = 15;
				}

			}

		}
		if (hit)
		{
			if (arrow->index == g_userIndex)
				mciSendString(L"play Hit from 0", NULL, 0, NULL);
			arrow->isDestroy = true;
			break;
		}

	}

}

//-----------------------------------【Direct3D_GUI_Render( )函数】-------------------------------
//	描述：使用Direct3D_GUI进行渲染
//--------------------------------------------------------------------------------------------------
void Direct3D_GUI_Render(HWND hwnd, float fTimeDelta)
{
	//清屏操作
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(100, 155, 255), 1.0f, 0);

	//开始绘制
	g_pd3dDevice->BeginScene();

	//正式绘制


	if (g_currentGUI == GUI_MAIN_SCREEN)
	{
		ProcessGUI(g_MainGUI, g_LMBDown, g_MouseX,
			g_MouseY, GUICallback);
	}
	else if (g_currentGUI == GUI_START_SCREEN)
	{
		ProcessGUI(g_StartGUI, g_LMBDown, g_MouseX,
			g_MouseY, GUICallback);
	}
	else if (g_currentGUI == GUI_LOADING_SCREEN)
	{
		ProcessGUI(g_LoadingGUI, g_LMBDown, g_MouseX,
			g_MouseY, GUICallback);
	}
	else if (g_currentGUI == GUI_LOAD_SCREEN)
	{
		ProcessGUI(g_LoadGUI, g_LMBDown, g_MouseX,
			g_MouseY, GUICallback);
	}
	else if (g_currentGUI == GUI_OPTION_SCREEN)
	{
		ProcessGUI(g_OptionGUI, g_LMBDown, g_MouseX,
			g_MouseY, GUICallback);
	}
	else
	{
		ProcessGUI(g_MainGUI, g_LMBDown, g_MouseX,
			g_MouseY, GUICallback);
	}

	//-----------------------------【绘制文字信息】-----------------------------
	//	HelpText_Render(hwnd);//GUI中不需要绘制


	//结束绘制
	g_pd3dDevice->EndScene();

	//显示翻转
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

}

//-----------------------------------【Direct3D_Render( )函数】-------------------------------
//	描述：使用Direct3D进行渲染
//--------------------------------------------------------------------------------------------------
void Direct3D_Render(HWND hwnd, float fTimeDelta)
{
	//清屏操作
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(100, 155, 255), 1.0f, 0);

	//开始绘制
	g_pd3dDevice->BeginScene();

	//正式绘制

	//-----------------------------【绘制地形】-----------------------------
	g_pTerrain->RenderTerrain(&g_matWorld, false);  //渲染地形，且第二个参数设为false，表示不渲染出地形的线框


													//-----------------------------【绘制骨骼动画】------------------------
	vector<Character * > ::iterator it;
	for (it = g_vCharacter.begin(); it != g_vCharacter.end(); it++)
	{
		(*it)->Draw();
	}

	//-----------------------------【绘制火球】------------------------
	vector <Arrow* >::iterator it1;
	for (it1 = g_vArrow.begin(); it1 != g_vArrow.end(); it1++)
	{
		(*it1)->Render();
	}

	//-----------------------------【绘制天空】-----------------------------
	D3DXMATRIX matSky, matTransSky, matRotSky;
	D3DXMatrixTranslation(&matTransSky, 0.0f, -13000.0f, 0.0f);
	D3DXMatrixRotationY(&matRotSky, -0.00002f*timeGetTime());   //旋转天空, 简单模拟云彩运动效果
	matSky = matTransSky*matRotSky;
	g_pSkyBox->RenderSkyBox(&matSky, false);

	//-----------------------------【绘制雪花粒子系统】------------------------
	g_pSnowParticles->UpdateSnowParticle(fTimeDelta);
	g_pSnowParticles->RenderSnowParticle();

	//-----------------------------【绘制文字信息】-----------------------------
	HelpText_Render(hwnd);

	//结束绘制
	g_pd3dDevice->EndScene();

	//显示翻转
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

}

//-----------------------------------【HelpText_Render( )函数】-------------------------------
//	描述：显示帮助信息的函数
//--------------------------------------------------------------------------------------------------
void HelpText_Render(HWND hwnd)
{
	//定义一个矩形，用于获取主窗口矩形
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);

	//显示每秒帧数
	formatRect.top = 5;
	int charCount = swprintf_s(g_strFPS, 20, _T("FPS:%0.3f"), Get_FPS());
	g_pTextFPS->DrawText(NULL, g_strFPS, charCount, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_RGBA(0, 239, 136, 255));

	//显示显卡类型名
	g_pTextAdaperName->DrawText(NULL, g_strAdapterName, -1, &formatRect,
		DT_TOP | DT_LEFT, D3DXCOLOR(1.0f, 0.5f, 0.0f, 1.0f));

	//显示摄像机信息
	formatRect.bottom -= 100;
	char info[500];


	formatRect.right -= 50;
	extern  int totalHpForHero[15];
	char cTrack[500];
	if (g_vCharacter.size() > 0)
		sprintf_s(cTrack, "Hp:%d/%d.....\n\
						 转向：...A...,...D.....\n\
						 行走：........W.........\n\
						 %s\n\
						 %s\n\
						 %f",
			g_vCharacter[g_userIndex]->hp,
			totalHpForHero[g_vCharacter[g_userIndex]->level - 1],
			g_vCharacter[g_userIndex]->timeForRun > -1.0f &&  g_vCharacter[g_userIndex]->timeForRun < 1.0f ? "奔跑：LSHIFT + W." : "奔跑：冷却中...........",
			g_vCharacter[g_userIndex]->timeForAttack <= 0.0f ? "攻击：.........J.........." : "攻击：冷却中..........",
			!g_vCharacter[g_userIndex]->alive ? g_vCharacter[g_userIndex]->timeForResurrection : 0.0f
		);

	g_pTextHelper->DrawTextA(NULL, cTrack, -1, &formatRect, DT_BOTTOM | DT_RIGHT, D3DXCOLOR(255.0f, 255.0f, 255.0f, 1.0f));


}




//-----------------------------------【Get_FPS( )函数】------------------------------------------
//	描述：获取帧速率
//--------------------------------------------------------------------------------------------------
float Get_FPS()
{
	static float  fps = 0;
	static int    frameCount = 0;//帧数
	static float  currentTime = 0.0f;//当前时间
	static float  lastTime = 0.0f;//上次时间
	frameCount++;
	currentTime = timeGetTime()*0.001f;

	if (currentTime - lastTime > 1.0f) //将时间控制在1秒钟
	{
		fps = (float)frameCount / (currentTime - lastTime);//计算这1秒钟的FPS值
		lastTime = currentTime;
		frameCount = 0;
	}

	return fps;
}



//-----------------------------------【Direct3D_CleanUp( )函数】--------------------------------
//	描述：对Direct3D的资源进行清理，释放COM接口对象
//---------------------------------------------------------------------------------------------------
void Direct3D_CleanUp()
{
	//释放COM接口对象
	SAFE_RELEASE(g_pd3dDevice);
	SAFE_RELEASE(g_pTextFPS)
		SAFE_RELEASE(g_pd3dDevice)
		SAFE_DELETE(g_MainGUI)
		SAFE_DELETE(g_StartGUI)
		SAFE_DELETE(g_LoadGUI)
		SAFE_DELETE(g_OptionGUI)
		SAFE_DELETE(g_OptionGUI)
}
