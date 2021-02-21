
// ShowSHPDoc.h : CShowSHPDoc 클래스의 인터페이스
//


#pragma once


class CShowSHPDoc : public CDocument
{
protected:
	FILE *SHPfp, *DBFfp;
	
	typedef struct Dbf_header3{
		unsigned char		version;
		unsigned char		year;
		unsigned char		month;
		unsigned char		day;
		unsigned long		recordn;
		unsigned short int	header_length; //헤더 전체 크기
		unsigned short int	record_length;
		unsigned char		ver10_reserved[20];
	}DBF_FILEHEADER;

	typedef struct Field_descriptor32{ //32 Byte Field Descriptor
		unsigned char		name[11];
		unsigned char		type;
		unsigned long		field_data_address;
		unsigned char		field_length;
		unsigned char		decimal_count;
		unsigned char		ver10_reserved[14];
	}DBF_FILEDDESCRIPTOR;

	void removeSpace(char *rec, int length);

public:
	BOOL IsReady(){return (SHPfp == NULL)? FALSE : TRUE;};
	bool IsDBFReady(){return (DBFfp == NULL)? FALSE : TRUE;};

	int GetNextPolygon(int **part, CPoint **point); //**사용은 배열을 넘겨줄 때 이용
	void Rewind();

	CStringArray listDBF;

	CList<CPoint *, CPoint *> listPoint;
	CList<int *, int *> listPart;
	CList<int , int &> listN;

	int listNum; //정보의 갯수 == 리스트 박스에 총 Item 갯수
	int moveX;
	int moveY;

protected: // serialization에서만 만들어집니다.
	CShowSHPDoc();
	DECLARE_DYNCREATE(CShowSHPDoc)

// 특성입니다.
public:

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 구현입니다.
public:
	virtual ~CShowSHPDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual void OnCloseDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};
