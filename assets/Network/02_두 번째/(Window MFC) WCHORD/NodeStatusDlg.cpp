// NodeStatusDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WCHORD.h"
#include "NodeStatusDlg.h"
#include "afxdialogex.h"


// CNodeStatusDlg 대화 상자입니다.

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


// CNodeStatusDlg 메시지 처리기입니다.


BOOL CNodeStatusDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
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
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CNodeStatusDlg::OnCbnSelchangeCombo1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	nodeIndex = m_ctrlSelectNode.GetCurSel();
	m_ctrlSelectNode.GetLBText(nodeIndex, selectedNode);
}
