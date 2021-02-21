// LookupDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WCHORD.h"
#include "LookupDlg.h"
#include "afxdialogex.h"


// CLookupDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLookupDlg, CDialogEx)

CLookupDlg::CLookupDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLookupDlg::IDD, pParent)
{

}

CLookupDlg::~CLookupDlg()
{
}

void CLookupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO2, m_ctrlSearchNode);
	DDX_Control(pDX, IDC_COMBO3, m_ctrlSearchFile);
}


BEGIN_MESSAGE_MAP(CLookupDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO3, &CLookupDlg::OnCbnSelchangeCombo3)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CLookupDlg::OnCbnSelchangeCombo2)
END_MESSAGE_MAP()


// CLookupDlg 메시지 처리기입니다.


BOOL CLookupDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	int i;
	CString str;

	for(i = 0; i < curNodes.nodeNum; i++) {
		str.Format("%d", curNodes.Node[i].ID);
		m_ctrlSearchNode.AddString(str);
	}
	m_ctrlSearchNode.SetCurSel(0);
	m_ctrlSearchNode.GetLBText(0, selectedNode);
	nodeIndex = 0;

	for(i = 0; i < curFiles.fileNum; i++) {
		str.Format("%d", curFiles.File[i].key);
		m_ctrlSearchFile.AddString(str);
	}
	m_ctrlSearchFile.SetCurSel(0);
	m_ctrlSearchFile.GetLBText(0, selectedFile);
	keyIndex = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CLookupDlg::OnCbnSelchangeCombo2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	nodeIndex = m_ctrlSearchNode.GetCurSel();
	m_ctrlSearchNode.GetLBText(nodeIndex, selectedNode);
}


void CLookupDlg::OnCbnSelchangeCombo3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	keyIndex = m_ctrlSearchFile.GetCurSel();
	m_ctrlSearchFile.GetLBText(keyIndex, selectedFile);
}
