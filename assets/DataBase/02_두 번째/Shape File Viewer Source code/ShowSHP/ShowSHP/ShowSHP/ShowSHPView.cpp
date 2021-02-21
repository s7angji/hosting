
// ShowSHPView.cpp : CShowSHPView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "ShowSHP.h"
#endif

#include "ShowSHPDoc.h"
#include "ShowSHPView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "ListboxDlg.h"
// CShowSHPView

IMPLEMENT_DYNCREATE(CShowSHPView, CView)

BEGIN_MESSAGE_MAP(CShowSHPView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CShowSHPView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(IDM_SORRY, &CShowSHPView::OnSorry)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CShowSHPView ����/�Ҹ�

CShowSHPView::CShowSHPView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	curSel = 0;
}

CShowSHPView::~CShowSHPView()
{
}

BOOL CShowSHPView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CShowSHPView �׸���

void CShowSHPView::OnDraw(CDC* pDC)
{
	CShowSHPDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
	
	srand(100);

	//������ �� ����
	if(pDoc->IsReady () != TRUE){ //if(IsReady()){} ����

		//OnEraseBkgnd ���� �����Ƿ� ������ �Ͼ� ������ ����
		CRect rect;
		GetClientRect(&rect);
		pDC->FillSolidRect(rect, RGB(255, 255, 255));

		//������ ������ �ʾ����Ƿ� �� �̻� �� �� ����
		return;
	}

	//buff == memDC
	CDC buff;
	buff.CreateCompatibleDC(pDC);
	
	CRect rect;
	GetClientRect(&rect);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rect.right, rect.bottom);

	CBitmap *oldBmp = (CBitmap *)buff.SelectObject(&bmp);

	//EraseBkgnd ���� ���� �ʱ⿡ ���� �Ͼ������ ����
	buff.FillSolidRect(rect, RGB(255, 255, 255));

	//��ġ ��ü ��� ����
	POSITION posN, posPoint, posPart;

	posN = pDoc->listN.GetHeadPosition();
	posPoint = pDoc->listPoint.GetHeadPosition();
	posPart = pDoc->listPart.GetHeadPosition();

	//SHP���ϰ� DBF������ �������� ���� ���� �ε����� ���� �ֱ⿡ DBF���� ������ ������ �ε��� curSel�� ���ϱ� ���� i
	int i = 0;
	while(posN){ //n�� �ƴ� ���� ���ƶ�
		if(i == curSel){ //DBF���� ������ ������ ���� ������ ��ĥ
			CBrush brush(RGB(rand()%100+ 155, rand()%100 + 155, rand()%100+ 155));
			CBrush *oldBrush = buff.SelectObject(&brush);

			//���Ͽ��� �о �׸��� �ʰ� �޸𸮿��� �о� �׸���
			buff.PolyPolygon(pDoc->listPoint.GetNext(posPoint), pDoc->listPart.GetNext(posPart) , pDoc->listN.GetNext(posN));

			buff.SelectObject(oldBrush);
		}
		else{
			//���Ͽ��� �о �׸��� �ʰ� �޸𸮿��� �о� �׸���
			buff.PolyPolygon(pDoc->listPoint.GetNext(posPoint), pDoc->listPart.GetNext(posPart) , pDoc->listN.GetNext(posN));
		}
		i++;
	}

	pDC->BitBlt(0, 0, rect.right, rect.bottom, &buff, 0, 0, SRCCOPY);

	buff.SelectObject(oldBmp);

	pDoc->Rewind();
}


// CShowSHPView �μ�


void CShowSHPView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CShowSHPView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CShowSHPView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CShowSHPView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}

void CShowSHPView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CShowSHPView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CShowSHPView ����

#ifdef _DEBUG
void CShowSHPView::AssertValid() const
{
	CView::AssertValid();
}

void CShowSHPView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CShowSHPDoc* CShowSHPView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CShowSHPDoc)));
	return (CShowSHPDoc*)m_pDocument;
}
#endif //_DEBUG


// CShowSHPView �޽��� ó����


void CShowSHPView::OnSorry()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CShowSHPDoc* pDoc = GetDocument();
	if(!pDoc->IsDBFReady()){
		AfxMessageBox("����Ʈ�� ������ �׸��� �����ϴ�. ������ ���� ���� �ּ���.");
		return;
	}

	CListboxDlg dlg;

	dlg.listDBF.Copy(pDoc->listDBF);
	dlg.listNum = pDoc->listNum;

	if(dlg.DoModal() == IDOK){
		curSel = dlg.curSel;
		Invalidate();
	}
}


BOOL CShowSHPView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	//return CView::OnEraseBkgnd(pDC);

	return TRUE;
}
