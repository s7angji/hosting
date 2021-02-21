// ListboxDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ShowSHP.h"
#include "ListboxDlg.h"
#include "afxdialogex.h"

// CListboxDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CListboxDlg, CDialogEx)

CListboxDlg::CListboxDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CListboxDlg::IDD, pParent)
	, strList(_T(""))
{
	curSel = 0;
	int recordn = 0;
}

CListboxDlg::~CListboxDlg()
{
}

void CListboxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, ctrlList);
	DDX_LBString(pDX, IDC_LIST1, strList);
}


BEGIN_MESSAGE_MAP(CListboxDlg, CDialogEx)
	ON_LBN_DBLCLK(IDC_LIST1, &CListboxDlg::OnLbnDblclkList1)
END_MESSAGE_MAP()


// CListboxDlg 메시지 처리기입니다.


BOOL CListboxDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	
	for(int i = 0; i < listNum; i++)
		ctrlList.AddString(listDBF[i]);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CListboxDlg::OnLbnDblclkList1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	curSel = ctrlList.GetCurSel();
	CString str;
	str.Format("%d 번째 요소 선택", curSel + 1);
	UpdateData(TRUE);
	AfxGetMainWnd()->SendMessage(WM_PAINT);
	AfxMessageBox(str);
}
