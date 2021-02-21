// LeaveDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WCHORD.h"
#include "LeaveDlg.h"
#include "afxdialogex.h"


// CLeaveDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CLeaveDlg, CDialogEx)

CLeaveDlg::CLeaveDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLeaveDlg::IDD, pParent)
{

}

CLeaveDlg::~CLeaveDlg()
{
}

void CLeaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_ctrlSelectNode);
}


BEGIN_MESSAGE_MAP(CLeaveDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CLeaveDlg::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CLeaveDlg �޽��� ó�����Դϴ�.


void CLeaveDlg::OnCbnSelchangeCombo1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	nodeIndex = m_ctrlSelectNode.GetCurSel();
	m_ctrlSelectNode.GetLBText(nodeIndex, selectedNode);
}


BOOL CLeaveDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	int i;
	CString str;

	for(i = 0; i < curNodes.nodeNum; i++) {
		str.Format("%d", curNodes.Node[i].ID);
		m_ctrlSelectNode.AddString(str);
	}
	m_ctrlSelectNode.SetCurSel(0);
	m_ctrlSelectNode.GetLBText(0, selectedNode);
	nodeIndex = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}
