// InitializeDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WCHORD.h"
#include "InitializeDlg.h"
#include "afxdialogex.h"


// CInitializeDlg 대화 상자입니다.

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


// CInitializeDlg 메시지 처리기입니다.
