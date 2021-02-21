// LookupDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WCHORD.h"
#include "LookupDlg.h"
#include "afxdialogex.h"


// CLookupDlg ��ȭ �����Դϴ�.

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


// CLookupDlg �޽��� ó�����Դϴ�.


BOOL CLookupDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

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
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CLookupDlg::OnCbnSelchangeCombo2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	nodeIndex = m_ctrlSearchNode.GetCurSel();
	m_ctrlSearchNode.GetLBText(nodeIndex, selectedNode);
}


void CLookupDlg::OnCbnSelchangeCombo3()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	keyIndex = m_ctrlSearchFile.GetCurSel();
	m_ctrlSearchFile.GetLBText(keyIndex, selectedFile);
}
