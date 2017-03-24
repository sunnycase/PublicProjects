// CES.cpp : CCESApp �� DLL ע���ʵ�֡�

#include "stdafx.h"
#include "CES.h"
#include "Cathelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CCESApp theApp;

const GUID CDECL _tlid = { 0x9F3CE81A, 0xB84B, 0x4ADF, { 0x8A, 0x5, 0x2, 0xF0, 0x3A, 0xEC, 0x63, 0xF8 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

const CATID CATID_SafeForScripting =
{ 0x7dd95801,0x9882,0x11cf,{ 0x9f,0xa9,0x00,0xaa,0x00,0x6c,0x42,0xc4 } };
const CATID CATID_SafeForInitializing =
{ 0x7dd95802,0x9882,0x11cf,{ 0x9f,0xa9,0x00,0xaa,0x00,0x6c,0x42,0xc4 } };

const CATID CLSID_SafeItem =
{ 0x360fb522, 0xfe3e, 0x42cc, { 0xb5, 0x6d, 0xca, 0x76, 0x7f, 0x41, 0x99, 0x87 } };


// CCESApp::InitInstance - DLL ��ʼ��

BOOL CCESApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO:  �ڴ�������Լ���ģ���ʼ�����롣
		AfxEnableControlContainer();
	}

	return bInit;
}



// CCESApp::ExitInstance - DLL ��ֹ

int CCESApp::ExitInstance()
{
	// TODO:  �ڴ�������Լ���ģ����ֹ���롣

	return COleControlModule::ExitInstance();
}



// DllRegisterServer - ������ӵ�ϵͳע���

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	if (FAILED(CreateComponentCategory(
		CATID_SafeForScripting,
		L"Controls that are safely scriptable")))
		return ResultFromScode(SELFREG_E_CLASS);

	if (FAILED(CreateComponentCategory(
		CATID_SafeForInitializing,
		L"Controls safely initializable from persistent data")))
		return ResultFromScode(SELFREG_E_CLASS);

	if (FAILED(RegisterCLSIDInCategory(
		CLSID_SafeItem, CATID_SafeForScripting)))
		return ResultFromScode(SELFREG_E_CLASS);

	if (FAILED(RegisterCLSIDInCategory(
		CLSID_SafeItem, CATID_SafeForInitializing)))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}



// DllUnregisterServer - �����ϵͳע������Ƴ�

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}
