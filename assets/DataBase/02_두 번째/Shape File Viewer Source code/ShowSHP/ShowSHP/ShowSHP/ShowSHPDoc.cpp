
// ShowSHPDoc.cpp : CShowSHPDoc 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "ShowSHP.h"
#endif

#include "ShowSHPDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CShowSHPDoc

IMPLEMENT_DYNCREATE(CShowSHPDoc, CDocument)

BEGIN_MESSAGE_MAP(CShowSHPDoc, CDocument)
END_MESSAGE_MAP()


// CShowSHPDoc 생성/소멸

CShowSHPDoc::CShowSHPDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.
	SHPfp = NULL;
	DBFfp = NULL;

	listNum = 0;

	moveX = 0;
	moveY = 0;
}

CShowSHPDoc::~CShowSHPDoc()
{
}

BOOL CShowSHPDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.
	if(SHPfp != NULL){
		fclose(SHPfp);
		SHPfp = NULL;

		listN.RemoveAll();
		listPart.RemoveAll();
		listPoint.RemoveAll();
	}
	if(DBFfp != NULL){
		fclose(DBFfp);
		DBFfp = NULL;
	}
	listNum = 0;

	moveX = 0;
	moveY = 0;
	return TRUE;
}




// CShowSHPDoc serialization

void CShowSHPDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CShowSHPDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CShowSHPDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CShowSHPDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CShowSHPDoc 진단

#ifdef _DEBUG
void CShowSHPDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CShowSHPDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CShowSHPDoc 명령


void CShowSHPDoc::OnCloseDocument()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(SHPfp != NULL){
		fclose(SHPfp);
		SHPfp = NULL;
	}
	if(DBFfp != NULL){
		fclose(DBFfp);
		DBFfp = NULL;
	}
	CDocument::OnCloseDocument();
}


BOOL CShowSHPDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	//얻은 SHP 파일 경로를 이용하여 DBF 파일도 같이 열어 주기
	CString m_strDBFPath = lpszPathName;
	int tmp = m_strDBFPath.GetLength() - 4;
	m_strDBFPath = m_strDBFPath.Mid(0, tmp) + ".dbf";
	AfxMessageBox(m_strDBFPath);

	DBFfp = fopen(m_strDBFPath, "rb");
	SHPfp = fopen(lpszPathName, "rb");

	if(DBFfp == NULL){
		::AfxMessageBox("Error : File Open");
		return FALSE;
	}
	else if(DBFfp != NULL){
		::AfxMessageBox("DBF File Open Success");
	}

	if(SHPfp == NULL){
		::AfxMessageBox("Error : File Open");
		return FALSE;
	}

	//Shape Type
	int shapeType;
	fseek(SHPfp, 32L, SEEK_SET);
	fread(&shapeType, sizeof(int), 1, SHPfp); //2개 Endian 문제

	if(shapeType != 5){
		::AfxMessageBox("Error : File Type is not Polygon");
		return FALSE;
	}

	//화면 뒤집힌거 잡는 중
	double xmin, ymin, xmax, ymax;
	fread(&xmin, sizeof(int), 1, SHPfp);
	fread(&ymin, sizeof(int), 1, SHPfp);
	fread(&xmax, sizeof(int), 1, SHPfp);
	fread(&ymax, sizeof(int), 1, SHPfp);

	//헤더 파일 건너 뛰기
	fseek(SHPfp, 100L, SEEK_SET);

	//SHP파일 메모리에 저장하기
	int *part = NULL;
	CPoint *point = NULL;

	while(int n = GetNextPolygon(&part, &point)){
		listN.AddTail(n);
		listPart.AddTail(part);
		listPoint.AddTail(point);
	}

	//DBF Type
	char dbfType;

	char record_tag;
	char *record;

		fread(&dbfType, 1, 1, DBFfp);

	if(dbfType != 3){
		fclose(DBFfp);
		::AfxMessageBox("지원 하지 않는 DBF File Version");
		return FALSE;
	}

	//DBF 파일 헤더 읽어오기
	DBF_FILEHEADER fHeader;
	fseek(DBFfp, 0, SEEK_SET);
	fread(&fHeader, sizeof(DBF_FILEHEADER), 1, DBFfp);

	//필드의 개수 구하기
	int nfield = (fHeader.header_length - sizeof(DBF_FILEHEADER))
					/ sizeof(DBF_FILEDDESCRIPTOR);

	//필드의 개수만큼 Field descriptor 생성하기
	DBF_FILEDDESCRIPTOR* pField = new DBF_FILEDDESCRIPTOR[nfield];
	//필드 정보 읽어내기
	fread(pField, sizeof(DBF_FILEDDESCRIPTOR) * nfield, 1, DBFfp);

	//File Header와 Field Descriptor, 0x0D 건너뛰기
	int offset = fHeader.header_length;
	fseek(DBFfp, offset, SEEK_SET);

	//Dialog의 ListBox안에 들어갈 정보인 listDBF의 size 결정
	listNum = fHeader.recordn;
	listDBF.SetSize(listNum);

	for(int j = 0; j < fHeader.recordn; j++){
		fseek(DBFfp, 1, SEEK_CUR); //첫 삭제 바이트 건너뛰기
		for(int i = 0; i < nfield; i++){
			record = new char[pField[i].field_length + 1];
			fread(record, pField[i].field_length, 1, DBFfp);
			record[pField[i].field_length] = '\0'; //NULL 문자 추가 첫 삭제 바이트 건너뛰기
			removeSpace(record, pField[i].field_length); //공백제거
			listDBF[j] += record;
			listDBF[j] += " , ";
			delete[] record;
		}
	}

	return TRUE;
}


int CShowSHPDoc::GetNextPolygon(int **part, CPoint **point)
{
	int numParts;
	int numPoints;

	// 헤더를 건너 뜀
	fseek(SHPfp, sizeof(int)*2, SEEK_CUR);

	//shape타입과 box를 건너 뜀 14page
	fseek(SHPfp, sizeof(int)*1 + sizeof(double)*4, SEEK_CUR);

	//NumParts 를 읽기
	fread(&numParts, sizeof(int), 1, SHPfp);

	//numPoints 를 읽기
	if(fread(&numPoints, sizeof(int), 1, SHPfp) == 0)
		return 0; //view의 while문

	*part = new int[numParts];
	fread(*part, sizeof(int), numParts, SHPfp);

	double *pts =new double[numPoints *2];
	fread(pts, sizeof(double), numPoints* 2, SHPfp);

	*point = new CPoint[numPoints]; //long double 버퍼

	for(int i= 0; i<numPoints; i++){
		//여기서 지도의 움직임 크기 지정
		(*point)[i].x=(long)((pts[i*2] +180 + moveX) *3.5f); //
		(*point)[i].y= (long)((90 -pts[i*2 +1] + moveY) *3.5f); //
	}

	for(int i= 0; i< numParts -1; i++) //시작 , 개수 [0,5] ,[5,5]
		(*part)[i]=(*part)[i+1] - (*part)[i];
	(*part)[numParts -1] = numPoints -(*part)[numParts -1];

	delete[] pts;

	return numParts;
}

void CShowSHPDoc::Rewind()
{
	fseek(SHPfp, 100L, SEEK_SET); //파일 헤더 건너 뛰기
}


void CShowSHPDoc::removeSpace(char *rec, int length){
	for(short int i = length - 1; i > 0; i--){
		if(rec[i] == ' ')
			rec[i] = 0;
		else
			break;
	}
}