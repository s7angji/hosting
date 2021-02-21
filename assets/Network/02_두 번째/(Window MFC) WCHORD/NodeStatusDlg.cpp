// NodeStatusDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WCHORD.h"
#include "NodeStatusDlg.h"
#include "afxdialogex.h"


// CNodeStatusDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CNodeStatusDlg, CDialogEx)

CNodeStatusDlg::CNodeStatusDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNodeStatusDlg::IDD, pParent)
{

}

CNodeStatusDlg::~CNodeStatusDlg()
{
}

void CNodeStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_ctrlSelectNode);
}


BEGIN_MESSAGE_MAP(CNodeStatusDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CNodeStatusDlg::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CNodeStatusDlg �޽��� ó�����Դϴ�.


BOOL CNodeStatusDlg::OnInitDialog()
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


void CNodeStatusDlg::OnCbnSelchangeCombo1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	nodeIndex = m_ctrlSelectNode.GetCurSel();
	m_ctrlSelectNode.GetLBText(nodeIndex, selectedNode);
}
