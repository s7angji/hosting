
// WCHORDView.cpp : CWCHORDView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "WCHORD.h"
#endif

#include "WCHORDDoc.h"
#include "WCHORDView.h"

#include "FileStatusDlg.h"
#include "NodeStatusDlg.h"

#define R 320

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWCHORDView

IMPLEMENT_DYNCREATE(CWCHORDView, CView)

BEGIN_MESSAGE_MAP(CWCHORDView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CWCHORDView::OnFilePrintPreview)
	//ON_WM_CONTEXTMENU()
	//ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CWCHORDView 생성/소멸

CWCHORDView::CWCHORDView()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	rect[0].left = 220 - 7;
	rect[0].right = 220 + 7;
	rect[0].top = 120 - 7;
	rect[0].bottom = 120 + 7;
	
	rect[1].left = 255 - 7;
	rect[1].right = 255 + 7;
	rect[1].top = 124 - 7;
	rect[1].bottom = 124 + 7;

	rect[2].left = 286 - 7;
	rect[2].right = 286 + 7;
	rect[2].top = 136 - 7;
	rect[2].bottom = 136 + 7;
	
	rect[3].left = 310 - 7;
	rect[3].right = 310 + 7;
	rect[3].top = 148 - 7;
	rect[3].bottom = 148 + 7;

	rect[4].left = 332 - 7;
	rect[4].right = 332 + 7;
	rect[4].top = 168 - 7;
	rect[4].bottom = 168 + 7;
	
	rect[5].left = 352 - 7;
	rect[5].right = 352 + 7;
	rect[5].top = 190 - 7;
	rect[5].bottom = 190 + 7;

	rect[6].left = 368 - 7;
	rect[6].right = 368 + 7;
	rect[6].top = 216 - 7;
	rect[6].bottom = 216 + 7;
	
	rect[7].left = 376 - 7;
	rect[7].right = 376 + 7;
	rect[7].top = 248 - 7;
	rect[7].bottom = 248 + 7;

	rect[8].left = 380 - 7;
	rect[8].right = 380 + 7;
	rect[8].top = 280 - 7;
	rect[8].bottom = 280 + 7;

	rect[9].left = 376 - 7;
	rect[9].right = 376 + 7;
	rect[9].top = 312 - 7;
	rect[9].bottom = 312 + 7;

	rect[10].left = 368 - 7;
	rect[10].right = 368 + 7;
	rect[10].top = 344 - 7;
	rect[10].bottom = 344 + 7;

	rect[11].left = 352 - 7;
	rect[11].right = 352 + 7;
	rect[11].top = 370 - 7;
	rect[11].bottom = 370 + 7;

	rect[12].left = 332 - 7;
	rect[12].right = 332 + 7;
	rect[12].top = 392 - 7;
	rect[12].bottom = 392 + 7;

	rect[13].left = 310 - 7;
	rect[13].right = 310 + 7;
	rect[13].top = 412 - 7;
	rect[13].bottom = 412 + 7;

	rect[14].left = 286 - 7;
	rect[14].right = 286 + 7;
	rect[14].top = 424 - 7;
	rect[14].bottom = 424 + 7;

	rect[15].left = 255 - 7;
	rect[15].right = 255 + 7;
	rect[15].top = 436 - 7;
	rect[15].bottom = 436 + 7;

	rect[16].left = 220 - 7;
	rect[16].right = 220 + 7;
	rect[16].top = 440 - 7;
	rect[16].bottom = 440 + 7;

	rect[17].left = 185 - 7;
	rect[17].right = 185 + 7;
	rect[17].top = 436 - 7;
	rect[17].bottom = 436 + 7;

	rect[18].left = 154 - 7;
	rect[18].right = 154 + 7;
	rect[18].top = 424 - 7;
	rect[18].bottom = 424 + 7;

	rect[19].left = 130 - 7;
	rect[19].right = 130 + 7;
	rect[19].top = 412 - 7;
	rect[19].bottom = 412 + 7;

	rect[20].left = 108 - 7;
	rect[20].right = 108 + 7;
	rect[20].top = 392 - 7;
	rect[20].bottom = 392 + 7;

	rect[21].left = 88 - 7;
	rect[21].right = 88 + 7;
	rect[21].top = 370 - 7;
	rect[21].bottom = 370 + 7;

	rect[22].left = 72 - 7;
	rect[22].right = 72 + 7;
	rect[22].top = 344 - 7;
	rect[22].bottom = 344 + 7;

	rect[23].left = 64 - 7;
	rect[23].right = 64 + 7;
	rect[23].top = 312 - 7;
	rect[23].bottom = 312 + 7;

	rect[24].left = 60 - 7;
	rect[24].right = 60 + 7;
	rect[24].top = 280 - 7;
	rect[24].bottom = 280 + 7;

	rect[25].left = 64 - 7;
	rect[25].right = 64 + 7;
	rect[25].top = 248 - 7;
	rect[25].bottom = 248 + 7;

	rect[26].left = 72 - 7;
	rect[26].right = 72 + 7;
	rect[26].top = 216 - 7;
	rect[26].bottom = 216 + 7;

	rect[27].left = 88 - 7;
	rect[27].right = 88 + 7;
	rect[27].top = 190 - 7;
	rect[27].bottom = 190 + 7;

	rect[28].left = 108 - 7;
	rect[28].right = 108 + 7;
	rect[28].top = 168 - 7;
	rect[28].bottom = 168 + 7;

	rect[29].left = 130 - 7;
	rect[29].right = 130 + 7;
	rect[29].top = 148 - 7;
	rect[29].bottom = 148 + 7;

	rect[30].left = 154 - 7;
	rect[30].right = 154 + 7;
	rect[30].top = 136 - 7;
	rect[30].bottom = 136 + 7;

	rect[31].left = 185 - 7;
	rect[31].right = 185 + 7;
	rect[31].top = 124 - 7;
	rect[31].bottom = 124 + 7;

}

CWCHORDView::~CWCHORDView()
{
}

BOOL CWCHORDView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CWCHORDView 그리기

void CWCHORDView::OnDraw(CDC* pDC)
{
	CWCHORDDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	pDC->TextOutA(0, 0, "Left Click : Node Status");
	pDC->TextOutA(0, 20, "Right Click : File Status");
	pDC->TextOutA(0, 40, "Menu STEP3 : (1)lookup (2)join (3)leave");

	CRect lRect;
	lRect.top = 120;
	lRect.bottom = lRect.top + R;
	lRect.left = 60;
	lRect.right = lRect.left + R;

	pDC->Ellipse(lRect);

	CString str;
	int i, j;
	for(i = 0; i < pDoc->getCurNodes().nodeNum; i++) {
		pDC->Ellipse(rect[pDoc->getCurNodes().Node[i].ID]);
	}
}


// CWCHORDView 인쇄


void CWCHORDView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CWCHORDView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CWCHORDView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CWCHORDView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CWCHORDView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CWCHORDView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CWCHORDView 진단

#ifdef _DEBUG
void CWCHORDView::AssertValid() const
{
	CView::AssertValid();
}

void CWCHORDView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWCHORDDoc* CWCHORDView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWCHORDDoc)));
	return (CWCHORDDoc*)m_pDocument;
}
#endif //_DEBUG


// CWCHORDView 메시지 처리기


void CWCHORDView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWCHORDDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if(pDoc->getCurNodes().nodeNum == 0) {
		AfxMessageBox("노드의 정보가 없습니다. !!");
		return;
	}
	

	CNodeStatusDlg dlg;
	dlg.curNodes = pDoc->getCurNodes();

	int i;

	CString str;
	CString tmp;

	if(dlg.DoModal() == IDOK) {
		pDoc->printNodes();

		str.Format("Node ID : %d \nNode IP : %s \n\nPre : %d\n", pDoc->getCurNodes().Node[dlg.nodeIndex].ID,
			inet_ntoa(pDoc->getCurNodes().Node[dlg.nodeIndex].ipaddr),
			pDoc->getCHORD(dlg.nodeIndex).fingerInfo.Pre.ID);
		for(i = 0; i < N; i++) {
			tmp.Format("\nFinger[%d] : %d", i, pDoc->getCHORD(dlg.nodeIndex).fingerInfo.finger[i]);
			str += tmp;
		}


		AfxMessageBox(str);
	}
	else {
		printf("사용자가 취소했습니다.\n");
		return;
	}



	CView::OnLButtonDown(nFlags, point);
}


void CWCHORDView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWCHORDDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if(pDoc->getCurFiles().fileNum == 0) {
		AfxMessageBox("파일의 정보가 없습니다. !!");
		return;
	}

	CFileStatusDlg dlg;
	dlg.curFiles = pDoc->getCurFiles();

	CString str;
	CString refID;

	if(dlg.DoModal() == IDOK) {
		pDoc->printFiles();

		str.Format("파일명 : %s(key = %d) \nOwner ID : %d \nRef Owner ID : ",
			pDoc->getCurFiles().File[dlg.keyIndex].Name,
			pDoc->getCurFiles().File[dlg.keyIndex].key,
			pDoc->getCurFiles().File[dlg.keyIndex].Owner);

		refID.Format("%d", pDoc->refOwnerID(pDoc->getCurFiles().File[dlg.keyIndex].key));
		str += refID;
		AfxMessageBox(str);
	}
	else {
		printf("사용자가 취소했습니다.\n");
		return;
	}

	CView::OnRButtonDown(nFlags, point);
}

BOOL CWCHORDView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return CView::OnEraseBkgnd(pDC);
	//return TRUE;
}
