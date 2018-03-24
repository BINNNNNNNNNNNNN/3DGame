
//*****************************************************************************************
// Desc: D3DUtil.h头文件，用于公共辅助宏的定义
//*****************************************************************************************

#pragma once


#ifndef HR
#define HR(x)    { hr = x; if( FAILED(hr) ) { return hr; } }			//HR()宏，方便执行错误的返回
#endif


#ifndef SAFE_DELETE					
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }    //SAFE_DELETE()宏,指针资源的释放
#endif    

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }	//SAFE_DELETE_ARRAY()宏，指向数组的指针资源的释放
#endif    

#ifndef SAFE_RELEASE			
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }    //SAFE_RELEASE()宏,COM资源的释放
#endif


