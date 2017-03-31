#pragma once
#include "afxwin.h"
#include <unordered_map>

// CESPropPage.h : CCESPropPage ����ҳ���������


// CCESPropPage : �й�ʵ�ֵ���Ϣ������� CESPropPage.cpp��

class CCESPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CCESPropPage)
	DECLARE_OLECREATE_EX(CCESPropPage)

// ���캯��
public:
	CCESPropPage();

// �Ի�������
	enum { IDD = IDD_PROPPAGE_CES };

// ʵ��
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ��Ϣӳ��
protected:
	DECLARE_MESSAGE_MAP()
private:
	CComboBox m_cbPrimCam;
	CComboBox m_cbSecndCam;

	std::unordered_map<int, CString> _indexToSymbolicLink;
	std::unordered_map<std::wstring, int> _symbolicToIndexLink;
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
private:
	int m_selectedPrimCamIndex;
	int m_selectedSecndCamIndex;
};

