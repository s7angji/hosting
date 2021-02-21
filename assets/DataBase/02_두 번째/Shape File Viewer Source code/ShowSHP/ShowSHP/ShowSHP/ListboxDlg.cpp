// ListboxDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ShowSHP.h"
#include "ListboxDlg.h"
#include "afxdialogex.h"

// CListboxDlg ��ȭ �����Դϴ�.

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


// CListboxDlg �޽��� ó�����Դϴ�.


BOOL CListboxDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	
	for(int i = 0; i < listNum; i++)
		ctrlList.AddString(listDBF[i]);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CListboxDlg::OnLbnDblclkList1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	curSel = ctrlList.GetCurSel();
	CString str;
	str.Format("%d ��° ��� ����", curSel + 1);
	UpdateData(TRUE);
	AfxGetMainWnd()->SendMessage(WM_PAINT);
	AfxMessageBox(str);
}
