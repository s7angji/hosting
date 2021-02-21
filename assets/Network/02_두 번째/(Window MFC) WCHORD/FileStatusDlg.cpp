// FileStatusDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WCHORD.h"
#include "FileStatusDlg.h"
#include "afxdialogex.h"


// CFileStatusDlg 대화 상자입니다.

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


// CFileStatusDlg 메시지 처리기입니다.


BOOL CFileStatusDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
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
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CFileStatusDlg::OnCbnSelchangeCombo1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	keyIndex = m_ctrlSelectFile.GetCurSel();
	m_ctrlSelectFile.GetLBText(keyIndex, selectedFile);
}
