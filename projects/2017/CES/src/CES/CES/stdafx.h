#pragma once

// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的
#define _ATL_NO_AUTOMATIC_NAMESPACE             // 避免类名称冲突

#include <afxctl.h>         // ActiveX 控件的 MFC 支持
#include <afxext.h>         // MFC 扩展
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT


#include <d3d9.h>
#include <initguid.h>
#include <dxva2api.h>
#undef INITGUID
#include <initguid.h>

#include <mfidl.h>
#include <mfapi.h>
#include <Mferror.h>