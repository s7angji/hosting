// InitializeDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WCHORD.h"
#include "InitializeDlg.h"
#include "afxdialogex.h"


// CInitializeDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CInitializeDlg, CDialogEx)

CInitializeDlg::CInitializeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CInitializeDlg::IDD, pParent)
	, m_nodeNum(0)
	, m_fileNum(0)
{

}

CInitializeDlg::~CInitializeDlg()
{
}

void CInitializeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, m_nodeNum);
	DDX_Text(pDX, IDC_EDIT3, m_fileNum);
	DDV_MinMaxInt(pDX, m_nodeNum, 1, 8);
	DDV_MinMaxInt(pDX, m_fileNum, 1, 8);
}


BEGIN_MESSAGE_MAP(CInitializeDlg, CDialogEx)
END_MESSAGE_MAP()


// CInitializeDlg �޽��� ó�����Դϴ�.
