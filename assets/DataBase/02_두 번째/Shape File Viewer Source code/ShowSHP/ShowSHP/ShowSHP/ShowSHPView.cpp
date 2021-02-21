
// ShowSHPView.cpp : CShowSHPView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
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
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CShowSHPView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(IDM_SORRY, &CShowSHPView::OnSorry)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CShowSHPView 생성/소멸

CShowSHPView::CShowSHPView()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	curSel = 0;
}

CShowSHPView::~CShowSHPView()
{
}

BOOL CShowSHPView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CShowSHPView 그리기

void CShowSHPView::OnDraw(CDC* pDC)
{
	CShowSHPDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	
	srand(100);

	//파일이 안 열림
	if(pDoc->IsReady () != TRUE){ //if(IsReady()){} 비추

		//OnEraseBkgnd 하지 않으므로 간단히 하얀 바탕만 만듬
		CRect rect;
		GetClientRect(&rect);
		pDC->FillSolidRect(rect, RGB(255, 255, 255));

		//파일이 열리지 않았으므로 더 이상 할 일 없음
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

	//EraseBkgnd 하지 않음 초기에 전부 하얀색으로 셋팅
	buff.FillSolidRect(rect, RGB(255, 255, 255));

	//위치 객체 얻어 오기
	POSITION posN, posPoint, posPart;

	posN = pDoc->listN.GetHeadPosition();
	posPoint = pDoc->listPoint.GetHeadPosition();
	posPart = pDoc->listPart.GetHeadPosition();

	//SHP파일과 DBF파일의 정보들이 각각 같은 인덱스에 놓여 있기에 DBF에서 선택한 정보의 인덱스 curSel과 비교하기 위한 i
	int i = 0;
	while(posN){ //n이 아닌 동안 돌아라
		if(i == curSel){ //DBF에서 선택한 정보의 나라가 맞으면 색칠
			CBrush brush(RGB(rand()%100+ 155, rand()%100 + 155, rand()%100+ 155));
			CBrush *oldBrush = buff.SelectObject(&brush);

			//파일에서 읽어서 그리지 않고 메모리에서 읽어 그리기
			buff.PolyPolygon(pDoc->listPoint.GetNext(posPoint), pDoc->listPart.GetNext(posPart) , pDoc->listN.GetNext(posN));

			buff.SelectObject(oldBrush);
		}
		else{
			//파일에서 읽어서 그리지 않고 메모리에서 읽어 그리기
			buff.PolyPolygon(pDoc->listPoint.GetNext(posPoint), pDoc->listPart.GetNext(posPart) , pDoc->listN.GetNext(posN));
		}
		i++;
	}

	pDC->BitBlt(0, 0, rect.right, rect.bottom, &buff, 0, 0, SRCCOPY);

	buff.SelectObject(oldBmp);

	pDoc->Rewind();
}


// CShowSHPView 인쇄


void CShowSHPView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CShowSHPView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CShowSHPView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CShowSHPView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
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


// CShowSHPView 진단

#ifdef _DEBUG
void CShowSHPView::AssertValid() const
{
	CView::AssertValid();
}

void CShowSHPView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CShowSHPDoc* CShowSHPView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CShowSHPDoc)));
	return (CShowSHPDoc*)m_pDocument;
}
#endif //_DEBUG


// CShowSHPView 메시지 처리기


void CShowSHPView::OnSorry()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CShowSHPDoc* pDoc = GetDocument();
	if(!pDoc->IsDBFReady()){
		AfxMessageBox("리스트를 구성할 항목이 없습니다. 파일을 먼저 열어 주세요.");
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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	//return CView::OnEraseBkgnd(pDC);

	return TRUE;
}
