// FileStatusDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WCHORD.h"
#include "FileStatusDlg.h"
#include "afxdialogex.h"


// CFileStatusDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CFileStatusDlg, CDialogEx)

CFileStatusDlg::CFileStatusDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFileStatusDlg::IDD, pParent)
{

}

CFileStatusDlg::~CFileStatusDlg()
{
}

void CFileStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_ctrlSelectFile);
}


BEGIN_MESSAGE_MAP(CFileStatusDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CFileStatusDlg::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CFileStatusDlg �޽��� ó�����Դϴ�.


BOOL CFileStatusDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	int i;
	CString str;

	for(i = 0; i < curFiles.fileNum; i++) {
		str.Format("%d", curFiles.File[i].key);
		m_ctrlSelectFile.AddString(str);
	}
	m_ctrlSelectFile.SetCurSel(0);
	m_ctrlSelectFile.GetLBText(0, selectedFile);
	keyIndex = 0;


	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CFileStatusDlg::OnCbnSelchangeCombo1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	keyIndex = m_ctrlSelectFile.GetCurSel();
	m_ctrlSelectFile.GetLBText(keyIndex, selectedFile);
}
