// JoinDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WCHORD.h"
#include "JoinDlg.h"
#include "afxdialogex.h"


// CJoinDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CJoinDlg, CDialogEx)

CJoinDlg::CJoinDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CJoinDlg::IDD, pParent)
	, addr1(0)
	, addr2(0)
	, addr3(0)
	, addr4(0)
{

}

CJoinDlg::~CJoinDlg()
{
}

void CJoinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, addr1);
	DDV_MinMaxInt(pDX, addr1, 0, 255);
	DDX_Text(pDX, IDC_EDIT2, addr2);
	DDV_MinMaxInt(pDX, addr2, 0, 255);
	DDX_Text(pDX, IDC_EDIT3, addr3);
	DDV_MinMaxInt(pDX, addr3, 0, 255);
	DDX_Text(pDX, IDC_EDIT4, addr4);
	DDV_MinMaxInt(pDX, addr4, 0, 255);
}


BEGIN_MESSAGE_MAP(CJoinDlg, CDialogEx)
END_MESSAGE_MAP()


// CJoinDlg �޽��� ó�����Դϴ�.
