#pragma once

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
};

