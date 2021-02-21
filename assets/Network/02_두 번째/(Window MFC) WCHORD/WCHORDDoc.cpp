// WCHORDDoc.cpp : CWCHORDDoc 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "WCHORD.h"
#endif

#include "WCHORDDoc.h"

#include "InitializeDlg.h"
#include "LookupDlg.h"
#include "JoinDlg.h"
#include "LeaveDlg.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CWCHORDDoc

IMPLEMENT_DYNCREATE(CWCHORDDoc, CDocument)

BEGIN_MESSAGE_MAP(CWCHORDDoc, CDocument)
	ON_COMMAND(ID_STEP3_LOOKUP, &CWCHORDDoc::OnStep3Lookup)
	ON_COMMAND(ID_STEP3_JOIN, &CWCHORDDoc::OnStep3Join)
	ON_COMMAND(ID_STEP3_LEAVE, &CWCHORDDoc::OnStep3Leave)
END_MESSAGE_MAP()


// CWCHORDDoc 생성/소멸

void CWCHORDDoc::initSetting() {
	CInitializeDlg dlg;
	dlg.setNodeNum(twoN/4);
	dlg.setFileNum(twoN/4);
	if(dlg.DoModal() == IDOK){
		initNode = dlg.getNodeNum();
		initFile = dlg.getFileNum();
	}
	else {
		initNode = 1;
		initFile = 1;
		AfxMessageBox("사용자의 취소로 다음과 같이 세팅합니다.\ninitNode = 1\ninitFile = 1");
	}

	printf("*****************************************************************\n");
	printf("* DHT-Based P2P Protocol (CHORD) Functional Simulator*\n");
	printf("* Ver. 0.3 Aug. 22, 2014*\n");
	printf("* (c) Kim, Tae-Hyong*\n");
	printf("*****************************************************************\n\n");

	printf("Note: This CHORD simulator has 3 steps.\n");
	printf("-----------------------------------------------------------------\n");
	printf("STEP 1: Initialization of CHORD Networks\n");
	printf(" ** Node Initialization Mode : Automatic Mode\n");
	printf(" A. Input the number of initial nodes : %d\n", initNode);
	/**////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	while (selNum < initNode) { //사용자가 만들고자하는 수만큼의 노드를 만들어 내자
		sameExist = 0;
		for (i=0; i < 4; i++) /* Random IP address generation */
			randIPsec[i] = rand()%255+1; //unsigned char randIPsec[4];

		//inet_ntoa() 함수 : 네트워크 바이트 순서의 32비트 값을 Dotted-Decimal Notation의 주소값으로 변환
		//*함수원형 : char *inet_ntoa(struct in_addr addr)
		//*리턴 값 : 성공 시 = 변환된 해당 문자열의 포인터, 실패시 = -1

		curIP.S_un.S_addr = randIPsec[3]*256*256*256 + //struct in_addr curIP;
			randIPsec[2]*256*256 + randIPsec[1]*256 + randIPsec[0];
		curIPstr = inet_ntoa(curIP); //char *curIPstr;
		curID = str_hash(curIPstr); //hash fucntion에 IP xxx.xxx.xxx.xxx를 넣어 ID값을 얻어온다.

		for (i = 0; i < curNodes.nodeNum; i++) //지금 내가 랜덤하게 만든 노드가 충돌하는지 테스트를 위해 기존에 만든 노드들과 비교하자
			if (curID == curNodes.Node[i].ID) {
				sameExist = 1; /* Already Exists */
				break; /* Choose Other */ //충돌 났으니 다시 첨부터 while문을 돌리로 가자 !!
			}

			if (sameExist == 0) { //충돌이 없다면 만든 IP와 ID를 집어 넣어주자
				curNodes.Node[curNodes.nodeNum].ipaddr = curIP;
				curNodes.Node[curNodes.nodeNum].ID = curID;
				selNum++;
				curNodes.nodeNum++;
				printf(" Generated Node %d: %s (ID:%d)\n", selNum,
					curIPstr, curID);
			}
	}

	/* Variable Re-Initialization */
	selNum = 0; //재사용을 위해 사용자가 입력한 initNode 값만큼 증가했지만 0으로 만들기

	printf(" ** File Initialization Mode : Automatic\n");

	printf(" B. Input the number of initial files : %d\n", initFile);
	/**////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	printf(" * In Automatic Mode, File Name will be random within F0~F%d \n", twoN-1);
	curFiles.fileNum = 0;

	while (selNum < initFile) { //사용자가 만들고자하는 수만큼의 파일을 만들어 내자
		sameExist = 0;
		curOwner = curNodes.Node[rand()%curNodes.nodeNum]; //이 파일의 소유자(파일 소유자는 여러 파일을 가져도 되므로 충돌은 신경 안씀)
		curName[0] = 'F'; //파일의 이름은 F로 시작하도록 만든다.
		do {
			randfnum = rand()%twoN;
			itoa(randfnum, curName+1, 10); //randfnum의 숫자를 파일의 이름 F 바로 뒷쪽에 문자로 바꿔 놓고 10진수로 표시하겠다.
			for (i = 0; i < selNum; i++) //selNum은 현재까지 만들어낸 파일의 수이고, 이때까지 만든 파일이랑 이름이 겹치는지 확인하겠다.
				if (strcmp(curFiles.File[selNum].Name, curName) == 0) { //strcmp()함수로 문자열 비교
					sameExist = 1; //충돌났음 do while문의 탈출할수있는 변수이므로 못나가게 해야함
					break; //똑같은 놈있으니 do while문 다시 돌아라
				}
		} while (sameExist);

		curKey = str_hash(curName); //hash 함수에 만들어낸 이름을 넣어 key를 얻자
		for (i = 0; i < curFiles.fileNum; i++) //지금까지 만든 파일을 전부 검사해서 충돌하는 key를 찾아내자
			if (curKey == curFiles.File[i].key) {
				sameExist = 1; /* Already Exists */
				break; /* Choose Other */ //어쩔수 없이 다시 만들자 '제일 처음' while문으로 돌아가자
			}

			if (sameExist == 0) { //충돌이 없다면 만들어낸 소유자, 파일이름, key 다 넣어주자
				curFiles.File[selNum].Owner = curOwner;
				strcpy(curFiles.File[selNum].Name, curName);
				curFiles.File[selNum].key = curKey;
				selNum++;
				curFiles.fileNum++;
				printf(" Generated File %d: %s (Key:%d) Owner:%d\n",
					selNum, curName, curKey, curOwner.ID);
			}
	}

	printf("-----------------------------------------------------------------\n");
	printf("STEP 2: Construct CHORD Tables\n");

	printf(" A. Construction of CHORD Table \n");

	//inet_ntoa() 함수 : 네트워크 바이트 순서의 32비트 값을 Dotted-Decimal Notation의 주소값으로 변환
	//*함수원형 : char *inet_ntoa(struct in_addr addr)
	//*리턴 값 : 성공 시 = 변환된 해당 문자열의 포인터, 실패시 = -1

	for (i=0; i<curNodes.nodeNum; i++) { //이제 STEP 1에서 만들어진 초기 노드들의 Join이 시작된다
		printf(" * Join Node %d : %s (ID: %d) \n", i+1,
			inet_ntoa(curNodes.Node[i].ipaddr), curNodes.Node[i].ID);

		CHORD[i].Self = curNodes.Node[i];
		initialize(curNodes.Node[i], curNodes.Node[0], i); //조인을 누가 도와줘도 상관없지만 가장 먼저 들어온놈은 꾸준히 Join동안 계속 있으므로 처음 온 놈이 도와주게한다.

		stabilize(curNodes.Node[i]); //내가 들어와서 고장난 부분의 succ와 pre를 고쳐주자

		for (j=0; j<=i; j++) //핑거들을 고쳐주자
			fix_finger(curNodes.Node[j]);

		for (j=0; j<=i; j++) {
			//nID2CPos() : 전역 변수 CHORD 배열에 있는 한 노드로부터 nodeID가 배열 몇번째 박혀있는지 좀 알자. 그래야 전역 변수 CHORD 배열의 정보를 알아낼 수 있지!
			printf(" - Finger Table (Node %d): Pre(%d) ",
				curNodes.Node[j].ID,
				CHORD[nID2CPos(curNodes.Node[j].ID)].fingerInfo.Pre.ID); //내 자신의 노드 ID와 pre를 출력
			for (k=0; k<N; k++)
				printf("%d(%d) ", k,
				CHORD[nID2CPos(curNodes.Node[j].ID)].fingerInfo.finger[k].ID); //나의 핑거들을 출력
			printf("\n");
		}
	}

	printf(" B. Addition of File Information in CHORD Table \n");
	printf(" B1. File Own Information \n");

	//CHORD 테이블에 생성 파일 정보 추가
	for (i = 0; i < curFiles.fileNum; i++) {
		//현재 STEP 1.에서 만들어 놓은 파일들의 실소유자에게 넣어주는 일
		tempID = curFiles.File[i].Owner.ID;
		CHORD[nID2CPos(tempID)].ownFile.File[CHORD[nID2CPos(tempID)].ownFile.fileNum++] = curFiles.File[i]; //++연산자를 잘보자. 파일을 집어넣으면서 후에 파일을 넣었다고 연산한다. 1타 2피
		printf(" (%d) Node (ID = %3d) has %5s (key = %3d).\n", i,
			curFiles.File[i].Owner.ID, curFiles.File[i].Name,
			curFiles.File[i].key); //이 노드가 이 파일을 가지고 있다를 출력
	}

	printf(" B2. File Reference Information \n");

	for (i=0; i<curFiles.fileNum; i++) {
		//현재 STEP 1.에서 만들어 놓은 파일들의 레퍼런스를 갖고 있어야하는 노드인 Succesor를 찾아 넣어주는 일 
		tempID = find_successor(curNodes.Node[0].ID,
			curFiles.File[i].key).ID; //key의 succ 노드들을 찾아주자. 여기서 아무 노드를 넣어도 찾아 줄 수 있지만 STEP 1.에서 제일 먼저 가입한 놈을 쓰고 있다.
		CHORD[nID2CPos(tempID)].ownFRef.File[CHORD[nID2CPos(tempID)]
		.ownFRef.fileNum++] = curFiles.File[i]; //++연산자를 잘보자. 파일을 집어넣으면서 후에 파일을 넣었다고 연산한다. 1타 2피
		printf(" (%d) Node (ID = %3d) has reference info. of %5s(key = %3d).\n", i,
			find_successor(curNodes.Node[0].ID, curFiles.File[i].key).ID,
			curFiles.File[i].Name, curFiles.File[i].key); //어떤 노드가 어떤 파일의 레퍼런스 정보를 가지고 있다를 출력
	}
	printf(" * CHORD Network Setup Completed!\n\n");

	printf("-----------------------------------------------------------------\n");
	printf("STEP 3: Handling User Selected Commands: \n");
}

CWCHORDDoc::CWCHORDDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.
	curNodes.nodeNum = 0;
	curFiles.fileNum = 0;
	for(i = 0; i < twoN; i++) {
		CHORD[i].ownFile.fileNum = 0;
		CHORD[i].ownFRef.fileNum = 0;
	}

	nodeTypeClear(&joinNode);
	isNodesFull = FALSE;

	existChooseLeaveNode = FALSE;
	isNodesEmpty = FALSE;

	Found = 0;
	tempNum = 0; //tempID : 'CHORD 테이블에 생성 파일 정보 추가'에서 사용중인 임시변수

	curName[0] = '\0';
	tempStr[0] = '\0';
	tempIPstr[0] = '\0';
	noFile = 0;
	selNum = 0; //selNum은 노드와 파일 초기화에서 이용, randfnum은 파일 초기화에서 이용(randfnum = rand()%twoN)
	i = 0;
	j = 0;
	k = 0;

	USel = 0;
	USel2 = 0;
	USel3 = 0;
	srand(time(NULL)); //srand(10); test를 위해 우선 난수를 임의로 고정하자

	initSetting();
}

CWCHORDDoc::~CWCHORDDoc()
{
}

BOOL CWCHORDDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CWCHORDDoc serialization

void CWCHORDDoc::Serialize(CArchive& ar)
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
void CWCHORDDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
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
void CWCHORDDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CWCHORDDoc::SetSearchContent(const CString& value)
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

// CWCHORDDoc 진단

#ifdef _DEBUG
void CWCHORDDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWCHORDDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CWCHORDDoc 명령

int CWCHORDDoc::twoPow(int power) //2^power
{
	int i;
	int result = 1;

	if(power >= 0)
		for(i=0; i < power; i++)
			result *= 2;
	else
		result -1;

	return result;
}

int CWCHORDDoc::modMinus(int modN, int minuend, int subtrand) //원형 큐 같이 숫자가 순환할때의 뺄셈
{
	if((minuend - subtrand) >= 0)
		return minuend - subtrand;
	else
		return (modN - subtrand) + minuend;
}

int CWCHORDDoc::modPlus(int modN, int addend1, int addend2) //circular 덧셈
{
	if((addend1 + addend2) < modN)
		return addend1 + addend2;
	else
		return (addend1 + addend2) - modN;
}

int CWCHORDDoc::modIn(int modN, int targNum, int range1, int range2, int leftmode, int rightmode)
//leftmode, rightmode : 0 => range boundary not included
//                      1 => range boundary included
{
	int result = 0;

	if(range1 == range2){ //범위의 크기가 0이고 왼쪽 오른쪽의 부등호가 없을 때 targNum은 범위에 속하지 못한다.
		if((leftmode == 0) || (rightmode == 0))
			return 0;
	}

	if(modPlus(twoN, range1, 1) == range2){ //범위의 크기가 1이지만 왼쪽 오른쪽 부등호가 없어서 targNum은 범위에 속하지 못한다.
		if((leftmode == 0) && (rightmode == 0))
			return 0;
	}

	if(leftmode == 0) //왼쪽의 부등호가 없다면 왼쪽의 부등호가 생길 수 있도록 한칸 전진
		range1 = modPlus(twoN, range1, 1);
	if(rightmode == 0) //오른쪽에 부등호가 없다면 오른쪽의 부등호가 생길 수 있도록 한칸 후진
		range2 = modMinus(twoN, range2, 1);

	if(range1 < range2) { //rage1 =< targNum =< rage2 이니까 당연히 범위안에 들었다.
		if((targNum >= range1) && (targNum <= range2))
			result = 1;
	}
	else if(range1 > range2){ //숫자들이 0~2^m까지이고 circular하게 돌아가므로 이 경우에는 다음과 같이 판단하고 범위안에 들었음을 알 수 있다.
		if(((targNum >= range1) && (targNum < modN))
			|| ((targNum >= 0) && (targNum <= range2))) //range1 <= targNum < modN, or 0 <= targNum <= range2
			result = 1;
	}

	else if((targNum == range1) && (targNum == range2)) //rage1, rage2 가 길이가 한점인데 그 점이 타겟이니깐 당연히 범위 안
		result = 1;

	return result;
}

unsigned CWCHORDDoc::str_hash(char *s) //Simple Hash Function from String to Key
{
	unsigned hashval;
	
	for(hashval = 0; *s != '\0'; s++) //문자열 s의 길이만큼 반복(char는 1Byte이므로 +1씩 진행하고 문자의 끝인 '\0'을 만날때 까지)
		hashval = *s + 31 * hashval;
		//문자 *s에 의존하는 난수적인(random) 값

	return hashval % twoN;
}

void CWCHORDDoc::printNodes(void)
{
	int i;
	printf("	- Currently there are %d nods: \n", curNodes.nodeNum);
	for (i=0; i<curNodes.nodeNum; i++)
		printf("	(%2d) Node %15s (ID=%3d)\n", i+1,
		inet_ntoa(curNodes.Node[i].ipaddr), curNodes.Node[i].ID);
}

void CWCHORDDoc::printFiles(void)
{
	int i, j, k, refID = -1, found = 0;
	printf("	- Currently there are %d files: \n", curFiles.fileNum);
	for (i=0; i<curFiles.fileNum; i++) { //모든 파일을 찾기 위해 모든 파일 수 만큼 돈다.
		for (j=0; j<curNodes.nodeNum; j++) { //각 파일의 레퍼런스를 가진 노드를 찾기 위해 모든 노드의 수만큼 돈다.
			found = 0; //반복문을 벗어나기위한 "찾던 파일의 레퍼런스 노드를 찾았다!"를 checking하는 변수
			for (k=0; k<CHORD[j].ownFRef.fileNum; k++)
				if (curFiles.File[i].key == CHORD[j].ownFRef.File[k].key) //찾던 파일의 레퍼런스를 알고있는 노드를 찾았을 때
				{
					refID = CHORD[j].Self.ID;
					found = 1;
					break;
				}
				if (found) break; //파일의 위치를 알았으니 다음 파일도 찾아야지? 다음 파일로 고고씽
		}
		printf("	(%2d) File %3s (Key=%3d) Owner ID: %3d, Ref Owner ID:%3d\n", i+1, curFiles.File[i].Name,
			curFiles.File[i].key, curFiles.File[i].Owner.ID, refID);
	}
}

/////////////////////////////////////////////////////// 구현해야 할 함수들 //////////////////////////////////////////////////////////////////////
int CWCHORDDoc::randomCurID(void)
{
	int i, curID, collision = 1;

	while(collision) { //충돌이 나면 다시 curID를 만들어 내자
		curID = rand()%twoN;
		for (i = 0; i < curNodes.nodeNum; i++){
			if (curID == curNodes.Node[i].ID) {
				collision = 1;
				break; //충돌 !!
			}
			else if(i == curNodes.nodeNum - 1) { //마지막 노드까지 충돌이 나지 않았으면 멈춰야한다.
				collision = 0;
				break;
			}
		}
	}
	return curID;
}

int CWCHORDDoc::lookup(int nodeID, int fileKey, CWCHORDDoc::nodeType *foundNode, int *refInfo)
{
	int i, Pos;
	int Store = 0;

	Pos = nID2CPos(nodeID); //nodeID가(현재 내가) CHORD 배열의 어느 index에 있는지 알아내자
	if ( Pos < 0) //배열의 범위 밖이다 에러
		return -1; /* Node Error */

	for (i=0; i<CHORD[Pos].ownFile.fileNum; i++) //우선 내가 가지고 있는 파일들을 뒤져 보자 나한테 있으면 끝이니깐
		if (fileKey == CHORD[Pos].ownFile.File[i].key)
			return -2; /* Store the File itself */

	for (i=0; i<CHORD[Pos].ownFRef.fileNum; i++) //내 파일이 없다면 나한테 그 파일의 위치까지 갈 수있는 정보가 있는지 뒤져보자
		if (fileKey == CHORD[Pos].ownFRef.File[i].key) {
			*refInfo = i;
			return -3; /* Store the File Reference itself */
		}
		*foundNode = find_successor(nodeID, fileKey); //fileKey는 아니까 이 파일의 레퍼런스가 저장될 successor를 찾아달라고 함수에게 시키자
		return 0; /* Success */

}

void CWCHORDDoc::join(CWCHORDDoc::nodeType joinNode, CWCHORDDoc::nodeType helpNode, int order)
	/* When there is no help node, helpNode = joinNode */
{
	int i;
	CWCHORDDoc::nodeType mySucc_sPre; //mySucc_sPre = my successor's predecessor

	initialize(joinNode, helpNode, order); //우선 x의 도움으로 나의 succ를 알아 내자
	move_keys(joinNode.ID,
		CHORD[nID2CPos(joinNode.ID)].fingerInfo.finger[0].ID); //찾은 나의 succ로 부터 내가 가져야 할 key를 받아 오자
	stabilize(joinNode); //나의 succ를 바르게하고 succ의 pre를 고치자

	/********************************* 주석 처리 가능 *********************************/
	//stabilize가 한번더 필요한 경우임
	mySucc_sPre = CHORD[nID2CPos(CHORD[nID2CPos(joinNode.ID)].fingerInfo.finger[0].ID)].fingerInfo.Pre;
	if(mySucc_sPre.ID != joinNode.ID) {
		printf(" ★ join : mySucc_sPre.ID != joinNode.ID ★ \n");
		AfxMessageBox(" ★ join : mySucc_sPre.ID != joinNode.ID ★ \n");
		stabilize(mySucc_sPre);
	}
	/**********************************************************************************/

	for (i = 0; i<curNodes.nodeNum; i++) { //모든 노드들의 finger들을 고치자
		fix_finger(curNodes.Node[i]);
	}
}

int CWCHORDDoc::nID2CPos(int nodeID)
{
	int i;

	for(i = 0; i < twoN; i++){
		if(nodeID == CHORD[i].Self.ID)
			return i;
	}

	return -1; //error 못 찾음
}

CWCHORDDoc::nodeType CWCHORDDoc::find_successor(int curNodeID, int IDKey)
{
	CWCHORDDoc::nodeType errorNode;
	int i, Pos, key_sPre; //key_sPre(key's Predecesssor)

	Pos = nID2CPos(curNodeID);
	if(Pos < 0){
		printf("CHORDInfo 배열에 없는 노드를 찾고 있습니다.[find_successor()]\n");
		errorNode.ID = -1;
		return errorNode; //error
	}

	if(curNodeID == IDKey)
		return CHORD[Pos].Self;
	else{
		key_sPre = find_predecessor(curNodeID, IDKey);
		return CHORD[nID2CPos(key_sPre)].fingerInfo.finger[0];
	}
}

int CWCHORDDoc::find_predecessor(int curNodeID, int IDKey)
{
	int oldNodeID;

	while(!modIn(twoN, IDKey, curNodeID, CHORD[nID2CPos(curNodeID)].fingerInfo.finger[0].ID, 0, 1))
	{
		oldNodeID = curNodeID;
		curNodeID = find_closest_predecessor(curNodeID, IDKey);
		
		if(oldNodeID == curNodeID) //또 나야? 그럼 나도 어차피 Predecessor 될 수 있잖아
			break;
	}

	return curNodeID;
}

int CWCHORDDoc::find_closest_predecessor(int curNodeID, int IDKey)
{
	int i;
	
	for(i = N-1; i >= 0; i--){
		if(modIn(twoN, CHORD[nID2CPos(curNodeID)].fingerInfo.finger[0].ID, curNodeID, IDKey, 0, 0))
			return CHORD[nID2CPos(curNodeID)].fingerInfo.finger[0].ID;
	}

	return curNodeID;
}

void CWCHORDDoc::initialize(CWCHORDDoc::nodeType joinNode, CWCHORDDoc::nodeType helpNode, int order)
{
	CHORD[order].Self = joinNode;

	//nID2CPos(joinNode.ID)로 인덱스를 알아낼 수 없다 왜냐하면 아직 CHORD배열에 들어간놈이 아니다. 그래서 order가 필요함
	CHORD[order].fingerInfo.Pre.ID = -1;

	if(nID2CPos(helpNode.ID) < 0)
		CHORD[order].fingerInfo.finger[0] = joinNode;
	else
		CHORD[order].fingerInfo.finger[0] = find_successor(helpNode.ID, joinNode.ID);
}

void CWCHORDDoc::move_keys(int toNodeID, int fromNodeID)
{
	int i, j, tempNum;

	if(toNodeID == fromNodeID) { //이 함수를 진행할 필요가 없다.
		return;
	}

	for(i = 0; i < CHORD[nID2CPos(fromNodeID)].ownFRef.fileNum; i++)
	{
		if(modIn(twoN, toNodeID, CHORD[nID2CPos(fromNodeID)].ownFRef.File[i].key, fromNodeID, 1, 1))
		{
			//move(k to node x)
			tempNum = CHORD[nID2CPos(toNodeID)].ownFRef.fileNum++; ////////////////////////////////////////////////////////////
			CHORD[nID2CPos(toNodeID)].ownFRef.File[tempNum] = CHORD[nID2CPos(fromNodeID)].ownFRef.File[i];

			//from node's refinfo delete
			tempNum = --CHORD[nID2CPos(fromNodeID)].ownFRef.fileNum; //////////////////////////////////////////////////////////
			for(j = i; j < tempNum; j++)
				CHORD[nID2CPos(fromNodeID)].ownFRef.File[j] = CHORD[nID2CPos(fromNodeID)].ownFRef.File[j + 1];
			//마지막 뒷 정리 CHORD[nID2CPos(fromNodeID)].ownFRef.File[j]의 모든걸 0으로
			for(;j < fMax; j++) ///////////////////////////////////////////////////////////////////////////////////////////////
				CHORD[nID2CPos(fromNodeID)].ownFRef.File[j].key = 0;
			i--; //for문의 fileNum의 조건을 건드렸으므로 i도 따라가야지 나머지 파일도 검사한다.
		}
	}
}

void CWCHORDDoc::stabilize(CWCHORDDoc::nodeType Node)
{
	CWCHORDDoc::nodeType mySucc_sPre; //mySucc_sPre = my successor's predecessor
	mySucc_sPre = CHORD[nID2CPos(CHORD[nID2CPos(Node.ID)].fingerInfo.finger[0].ID)].fingerInfo.Pre;

	if(mySucc_sPre.ID < 0){
		printf(" ★ stabilize : 나의 succ의 Pre 정보가 없습니다.. !! ★ \n");
		notify(Node, CHORD[nID2CPos(Node.ID)].fingerInfo.finger[0]);
		return;
		//CHORD[nID2CPos(Node.ID)].fingerInfo.Pre = Node;
	}
	else if(modIn(twoN, mySucc_sPre.ID, Node.ID, CHORD[nID2CPos(Node.ID)].fingerInfo.finger[0].ID, 0, 0))
		CHORD[nID2CPos(Node.ID)].fingerInfo.finger[0] = mySucc_sPre;
	
	else{
		CHORD[nID2CPos(mySucc_sPre.ID)].fingerInfo.finger[0] = Node;
		notify(mySucc_sPre, Node);
	}

	notify(Node, CHORD[nID2CPos(Node.ID)].fingerInfo.finger[0]);
}

void CWCHORDDoc::notify(CWCHORDDoc::nodeType askNode, CWCHORDDoc::nodeType selfNode)
{
	if(CHORD[nID2CPos(selfNode.ID)].fingerInfo.Pre.ID == -1 ///////////////////////////////////////////////
		|| CHORD[nID2CPos(selfNode.ID)].fingerInfo.Pre.ID == selfNode.ID
		|| modIn(twoN, askNode.ID, CHORD[nID2CPos(selfNode.ID)].fingerInfo.Pre.ID, selfNode.ID, 1, 0)) ///////////////왼쪽편을 1로 해보자.. x == (Pre,N) 에서 x가 Pre이면 재귀함수를 끝없이 부르니까. x는 pre일때 N이 자신의 succ가 맞아도 왼편이 0이면 else문만 죽어라 부를 것이다.
	{
		CHORD[nID2CPos(selfNode.ID)].fingerInfo.Pre = askNode;
	}
	else if(askNode.ID == selfNode.ID) { //ex 노드 상황이 22, 4가 남고 [22](Pre4)(22)(22)(22)(22)(22), [4](Pre-1)(22)(22)(22)(22)(22)(22) (BUG(srand(10) : 1, 2, 3, 24, 4, 123, 242, 124)
		printf(" ★ notify : askNode == selfNodeID (ID = %d) 같아서 보정합니다. (selfNode의 Pre = %d) ★\n",
			selfNode.ID, CHORD[nID2CPos(selfNode.ID)].fingerInfo.Pre.ID);
		CHORD[nID2CPos(selfNode.ID)].fingerInfo.finger[0] = CHORD[nID2CPos(selfNode.ID)].fingerInfo.Pre;
		notify(selfNode, CHORD[nID2CPos(selfNode.ID)].fingerInfo.Pre);
	}
	else { //내 predecessor가 너무나 멀쩡하므로 내가 askNode의 succ가 될 수 없다 다시 stabilize를 해라 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		stabilize(askNode);
		printf(" ★ notify : 노드 %d의 succ를 다시 찾아야 합니다.(succ 아닌 노드는 %d) ★ \n", askNode.ID, selfNode.ID);
	}
}

void CWCHORDDoc::fix_finger(CWCHORDDoc::nodeType Node)
{
	int i, j;
	for(i = 1; i < N; i++){ //i = 1부터 시작해도 됨
		j = Node.ID + twoPow(i);
		if(j >= twoN)
			j -= twoN;
		CHORD[nID2CPos(Node.ID)].fingerInfo.finger[i] = find_successor(Node.ID, j);
	}
}

void CWCHORDDoc::nodeTypeClear(CWCHORDDoc::nodeType *node) {
	char *zeroIP = "0";

	node->ID = -1;
	inet_aton(zeroIP, &node->ipaddr);
}

void CWCHORDDoc::fileTypeClear(CWCHORDDoc::fileType *file) {
	char *zeroIP = "0";

	file->key = -1;
	file->Name[0] = '\0';
	file->Owner.ID = -1;
	inet_aton(zeroIP, &file->Owner.ipaddr);
}

void CWCHORDDoc::fileTypeCopy(CWCHORDDoc::fileType *copy, CWCHORDDoc::fileType original) {
	int i;
	copy->key = original.key;
	for(i = 0; i < 32; i++) {
		copy->Name[i] = original.Name[i];
	}
	copy->Owner = original.Owner;
}

void CWCHORDDoc::Remove_that_reference_info(int nID2CPosIndex, int leaveNodeID) { //각 노드 중 하나의 인덱스를 받아 파일의 소유자가 떠나서 더 이상 쓸모 없게 된 reference를 지운다.
	int i= 0, j;
	int curNodeID;
	int found;

	curNodeID = CHORD[nID2CPosIndex].Self.ID;

	//CHORD Update
	do{
		found = 0;

		for(; i < CHORD[nID2CPosIndex].ownFRef.fileNum; i++) {
			if(CHORD[nID2CPosIndex].ownFRef.File[i].Owner.ID == leaveNodeID) { //없어져야할 파일의 인덱스를 찾음
				found = 1;
				break;
			}
		}
		if(i == (CHORD[nID2CPosIndex].ownFRef.fileNum - 1)) { //삭제할 파일이 배열의 마지막 부분에 있었으므로 간단하게 제거한다.
			fileTypeClear(&CHORD[nID2CPosIndex].ownFRef.File[i]);
			CHORD[nID2CPosIndex].ownFRef.fileNum--;
		}
		else if(i < (CHORD[nID2CPosIndex].ownFRef.fileNum - 1)) { //파일을 삭제하면 배열의 마지막 부분이 아니므로 앞쪽으로 땡겨와야한다.
			for(j = i; j < (CHORD[nID2CPosIndex].ownFRef.fileNum - 1); j++) {
				CHORD[nID2CPosIndex].ownFRef.File[j] = CHORD[nID2CPosIndex].ownFRef.File[j + 1];
			}
			CHORD[nID2CPosIndex].ownFRef.fileNum--;
		}

		if(found) //CHORD[nID2CPosIndex].ownFRef.fileNum--로 조작했으므로 i를 조정하자
			i--;
		if(i < (CHORD[nID2CPosIndex].ownFRef.fileNum - 1))
			fileTypeClear(&CHORD[nID2CPosIndex].ownFRef.File[j]); //마지막 부분을 의미없는 값으로 지워주자. 디버그의 편의를 위해서
	}while(i < CHORD[nID2CPosIndex].ownFRef.fileNum);

}

void CWCHORDDoc::Remove_curFiles_Update(int leaveNodeID) //curFiles 에서 제거해야할 내용을 업데이트한다.
{
	int i = 0, j;
	int found;

	do{
		found = 0;

		for(; i < curFiles.fileNum; i++) {
			if(curFiles.File[i].Owner.ID == leaveNodeID) {
				found = 1;
				break;
			}
		}
		if(i == (curFiles.fileNum - 1)) { //삭제할 파일이 배열의 마지막 부분에 있었으므로 간단하게 제거한다.
			fileTypeClear(&curFiles.File[i]);
			curFiles.fileNum--;
		}
		else if(i < (curFiles.fileNum - 1)) { //파일을 삭제하면 배열의 마지막 부분이 아니므로 앞쪽으로 땡겨와야한다.
			for(j = i; j < (curFiles.fileNum - 1); j++) {
				curFiles.File[j] = curFiles.File[j + 1];
			}
			curFiles.fileNum--;
		}

		if(found) //curFiles.fileNum--로 조작했으므로 i를 조정하자
			i--;
		if(i < (curFiles.fileNum - 1))
			fileTypeClear(&curFiles.File[j]); //끝에 의미없는 배열의 내용을 지우자.. 디버그 할때 햇갈리니깐
	}while(i < curFiles.fileNum);
}

void CWCHORDDoc::Remove_leaveNode(int leaveNodeID) //CHORD배열과 curNodes 에서 떠난 노드를 제거한다.
{
	int i, j;

	//CHORD delete
	for(i = 0; i < curNodes.nodeNum; i++) {
		if(CHORD[i].Self.ID == leaveNodeID) {
			break;
		}
	}

	if(i == (curNodes.nodeNum - 1)){ //삭제할 노드가 배열의 마지막 부분에 있었으므로 간단하게 제거한다.
		CHORD[i].Self.ID = -1;
	}
	else if(i < (curNodes.nodeNum - 1)){ //노드를 삭제하면 배열의 마지막 부분이 아니므로 앞쪽으로 땡겨와야한다.
		for(j = i; j < (curNodes.nodeNum - 1); j++)
			CHORD[j] = CHORD[j + 1];
	}
	else {
		printf("		삭제할 노드가 CHORD 배열상에 없습니다. Error\n");
		return; //이 함수를 더 돌아도 의미가 없다. 빠져나가자
	}
	if(i < (curNodes.nodeNum - 1)) { //마지막 부분 마무리
		nodeTypeClear(&CHORD[j].Self);
	}
	
	//curNodes delete
	for(i = 0; i < curNodes.nodeNum; i++) {
		if(curNodes.Node[i].ID == leaveNodeID)
			break;
	}
	
	if(i == (curNodes.nodeNum - 1)) { //삭제할 노드가 배열의 마지막 부분에 있었으므로 간단하게 제거한다.
		curNodes.Node[i].ID = -1;
		curNodes.nodeNum--;
	}
	else if(i < (curNodes.nodeNum - 1)) { //노드를 삭제하면 배열의 마지막 부분이 아니므로 앞쪽으로 땡겨와야한다.
		for(j = i; j < (curNodes.nodeNum - 1); j++) {
			curNodes.Node[j] = curNodes.Node[j + 1];
		}
		curNodes.nodeNum--;
	}
	else
		printf("		삭제할 노드가 curNodes 배열상에 없습니다. Error\n");
	if(--i < (curNodes.nodeNum - 1)) { //마지막 부분 마무리, curNodes.nodeNum--;을 조작했으므로 i값 감소시키자
		nodeTypeClear(&curNodes.Node[j]);
	}
}

void CWCHORDDoc::Move_All_Keys(int toNodeID, int fromNodeID) //fromNode의 모든 reference를 toNode에게 맞긴다.
{
	int i;
	if(toNodeID == fromNodeID) //같은 노드가 들어왔으므로 함수는 그냥 나가자
		return;

	toNodeID = nID2CPos(toNodeID);
	fromNodeID = nID2CPos(fromNodeID);
	for(i = 0; i < CHORD[fromNodeID].ownFRef.fileNum ; i++)
		CHORD[toNodeID].ownFRef.File[CHORD[toNodeID].ownFRef.fileNum++] = CHORD[fromNodeID].ownFRef.File[i];
}

//밑에 4개의 함수가 leave()를 보조한다.
//void Remove_curFiles_Update(int leaveNodeID); //curFiles 에서 제거해야할 내용을 업데이트한다.
//void Remove_that_reference_info(int nID2CPosIndex, int leaveNodeID); //각 노드중 하나의 인덱스를 받아 파일의 소유자가 떠나서 더 이상 쓸모 없게 된 reference를 지운다.
//void Remove_leaveNode(int leaveNodeID); //CHORD배열과 curNodes 에서 떠난 노드를 제거한다.
//void Move_All_Keys(int toNodeID, int fromNodeID); //fromNode의 모든 reference를 toNode에게 맞긴다.

void CWCHORDDoc::leave(int leaveNodeID) //내가 떠나면 pre가 틀린 노드와 succ 틀린 노드가 나올 것이다. 찾아보자!
{

	int i, j, k, l;
	CWCHORDDoc::nodeType tmpSucc;
	CWCHORDDoc::nodeType needS; //stabilize가 필요한 노드

	Remove_curFiles_Update(leaveNodeID); //내가 떠나면서 가지고 있는 파일들은 CHORD 상에서 사라진다.
	Move_All_Keys(CHORD[nID2CPos(leaveNodeID)].fingerInfo.finger[0].ID, leaveNodeID); //나의 succ에게 모든 키를 주자
	Remove_leaveNode(leaveNodeID); //정말로 노드는 떠난다.

	if(curNodes.nodeNum == 0) //남아 있는 노드가 없으므로 다음단계 진행은 멈추자
		return; //함수 탈출

	for(i = 0; i < curNodes.nodeNum; i++) { //네트워크 안에 남아 있는 각 노드들을 뒤져보자
		if(CHORD[i].fingerInfo.Pre.ID == leaveNodeID) { //어떤 노드의 predecessor가 떠날 나였다면 null로 만들자. 찾았다!
			nodeTypeClear(&CHORD[i].fingerInfo.Pre);
		}

		for(j = 0; j < N; j++) { //각 모든 finger 테이블을 훑어 볼 차례
			if(CHORD[i].fingerInfo.finger[j].ID == leaveNodeID) { //떠날 나에게서 Pong을 받지 못 할 것이다. 이제 없는 나를 가르키는 finger를 다음 세가지 경우로 finger들을 적절하게 고쳐보자.
				if(j == N - 1) //마지막 finger는 다음 finger가 없으므로 predecessor로 대체한다.
					CHORD[i].fingerInfo.finger[j] = CHORD[i].fingerInfo.Pre;
				else if((0 < j) && (j < (N - 1))) { //중간 finger들은 다음 finger 값으로 하자
					CHORD[i].fingerInfo.finger[j] = CHORD[i].fingerInfo.finger[j + 1];
				}
				else if(j == 0){ //(j == 0)//첫번째 finger는 succ 곧 succ가 잘못된 노드다. 찾았다!
					needS = CHORD[i].Self; //stabilize 함수가 필요한 노드를 찾았다
					
					for(k = 0; k < N; k++) { //떠난 노드를 가르키는 잘못된 모든 핑거들을 나에게서 있는 정보에서 그나마 적절한 succ가 되는 놈을 끌어오자
						tmpSucc = CHORD[i].fingerInfo.finger[k];

						if(tmpSucc.ID != leaveNodeID) //잘못된 모든 핑거들의 그나마 내가 가지고 있는 정보에서 임시 succ가 될 수 있는 노드를 찾았다 break
							break;
					}
					if (k != N) {
						for(l = 0; l < k; l++) {
							CHORD[i].fingerInfo.finger[l] = tmpSucc; //잘못된 핑거에 찾아놓은 임시 succ를 넣자
						}
					}
					//k == N으로 나왔다면 finger들이 싹 다 잘못된 것.. pre로 채워주자!!!
					if(k == N) {
						if(CHORD[i].fingerInfo.Pre.ID == leaveNodeID) {
							printf("		leave() : 심각한 Error 여기 부분은 노드 혼자 존재한다는건데 이미 이 함수 앞부분에서 함수를 나가게 만들었는데 여기로 들어오면 안돼\n");
							return; //error로 그냥 함수 종료
						}
						//밑의 경우의 예제
						//현재 CHORD 상에 20, 22가 있다.
						//22의 pre와 finger들 전부 다 20이다.
						//20이 떠났다... pre와 finger 모두 없다는 것! 나 혼자 있다는거다.
						else if(CHORD[i].fingerInfo.Pre.ID == -1) { //여기서 중요한것은 pre도 없고 succ도 없다는 점이다. 그렇다면 이 네트워크상에는 나 혼자 있는것이다.
							CHORD[i].fingerInfo.Pre.ID = CHORD[i].Self.ID;
							for(l = 0; l < N; l++) //finger들을 싹 다 pre로 바꾸는 작업
								CHORD[i].fingerInfo.finger[l] = CHORD[i].fingerInfo.Pre;
						}
						//밑의 경우의 예제
						//현재 CHORD 상에 5, 6, 25, 28이 있다.
						//6의 pre는 5이고, finger들 전부 다 25이다.
						//25가 떠났다... finger만 모두 없어 졌다는 것! 내가 가진 정보중에 Pre를 써야한다는 소리.
						else {
							for(l = 0; l < N; l++) //finger들을 싹 다 pre로 바꾸는 작업
								CHORD[i].fingerInfo.finger[l] = CHORD[i].fingerInfo.Pre;
						}
					}
				}
				else
					printf("		Leave() : Error \n\n");
			}
		}
		//if(M has reference info of the file owned by N)
		//N이 떠나면서 필요 없어진 정보를 청소하자.
		//	Remove_that_reference_info
		Remove_that_reference_info(i, leaveNodeID);
	}

	stabilize(needS); //succ가 잘못된 노드만 stabilize 해주면 모든게 해결된다.

	for (i = 0; i<curNodes.nodeNum; i++) { //모든 노드들의 finger들을 고치자
		fix_finger(curNodes.Node[i]);
	}
}

//http://blog.naver.com/jisangryul7/140020071124 참고
int CWCHORDDoc::inet_aton(const char *cp, struct in_addr *ap)
{
	int dots = 0;
	register u_long acc = 0, addr = 0;

	do{
		register char cc = *cp;
		switch(cc) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			acc = acc * 10 + (cc - '0');
			break;
		case '.':
			if(++dots > 3)
				return 0;

		/*Fall through*/
		case '\0':
			if(acc > 255)
				return 0;
			addr = addr << 8 | acc;
			acc = 0;
			break;
		default:
			return 0;
		}
	}while(*cp++);
	/*Store it if requested*/
	if(ap)
		ap->s_addr = htonl(addr);
	return 1;
}

void CWCHORDDoc::OnStep3Lookup()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if(curFiles.fileNum == 0) {
		printf("			현재 찾을 파일이 없습니다!\n");
		AfxMessageBox("현재 찾을 파일이 없습니다!");
		return;
	}

	//사용자에게서 검색할 노드, 검색할 파일을 얻어오기
	CLookupDlg dlg;
	
	dlg.curNodes = curNodes;
	dlg.curFiles = curFiles;

	CString str;
	int nSelected;

	int nodeIndex;
	int keyIndex;

	if(dlg.DoModal() == IDOK) {
		nodeIndex = dlg.nodeIndex;
		keyIndex = dlg.keyIndex;

		printf("\n      (1) File Search\n");
		printNodes();
		printFiles();
		printf("       - Search file %s (Key=%d) at Node %15s (ID=%d)\n", curFiles.File[keyIndex].Name, curFiles.File[keyIndex].key, inet_ntoa(curNodes.Node[nodeIndex].ipaddr), curNodes.Node[nodeIndex].ID);
		lookupResult = lookup(curNodes.Node[nodeIndex].ID, curFiles.File[keyIndex].key, &foundNodeLU, &refInfoLU);

		if(lookupResult == -1)
			printf("       - Node index out of bounds\n");
		else if(lookupResult == -2)
			printf("       - The file %s is stored at Node ID %d itself!\n", curFiles.File[keyIndex].Name, curNodes.Node[nodeIndex].ID);
		else if(lookupResult == -3) {
			printf("       - Reference info of file %s is at Node ID %d itself!\n", curFiles.File[keyIndex].Name, curNodes.Node[nodeIndex].ID);
			printf("       - Actual file %s is stored at Node %15s (ID=%d)\n", curFiles.File[keyIndex].Name, inet_ntoa(CHORD[nID2CPos(curNodes.Node[nodeIndex].ID)].ownFRef.File[refInfoLU].Owner.ipaddr), CHORD[nID2CPos(curNodes.Node[nodeIndex].ID)].ownFRef.File[refInfoLU].Owner.ID);
		}
		else { //lookupResult == 0

			printf("       - The file %s is not stored at Node ID %d.\n", curFiles.File[keyIndex].Name, curNodes.Node[nodeIndex].ID);
			printf("       - Reference info of file %s is not at Node ID %d.\n", curFiles.File[keyIndex].Name, curNodes.Node[nodeIndex].ID);
			printf("       - The Successor of Key %d : Node ID %d.\n", curFiles.File[keyIndex].key, foundNodeLU.ID);
			printf("       - Node ID %d has the reference of the file %s.\n", foundNodeLU.ID, curFiles.File[keyIndex].Name);

			//printf("       - The actual file %s is stored at Node %15s (ID=%d).\n", curFiles.File[keyIndex].Name, inet_ntoa(curFiles.File[keyIndex].Owner.ipaddr), curFiles.File[keyIndex].Owner.ID);
			//다음 바로 밑의 printf() 주석처리 한 것을 이용하면 전역변수에 저장된 파일의 소유자를 바로 찾아 갈 수 있지만.. 이러지말고
			//알고리즘 개념을 이용하여 알고리즘이 찾아준 노드를 방문해서 쫓아 가보는 것으로 하자
			for(i = 0; i < CHORD[nID2CPos(foundNodeLU.ID)].ownFRef.fileNum; i++) {
				if(curFiles.File[keyIndex].key == CHORD[nID2CPos(foundNodeLU.ID)].ownFRef.File[i].key)
					break;
			}
			printf("       - The actual file %s is stored at Node %15s (ID=%d).\n\n\n", curFiles.File[keyIndex].Name, inet_ntoa(CHORD[nID2CPos(foundNodeLU.ID)].ownFRef.File[i].Owner.ipaddr), CHORD[nID2CPos(foundNodeLU.ID)].ownFRef.File[i].Owner.ID);
		}

		AfxMessageBox("결과는 콘솔창에 !!");
	}
	else {
		printf("사용자가 취소했습니다.\n");
		return;
	}
}

void CWCHORDDoc::OnStep3Join()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if(curNodes.nodeNum == twoN)
		isNodesFull = TRUE;
	else
		isNodesFull = FALSE;

	if(curNodes.nodeNum == 0)
		isNodesEmpty = TRUE;
	else
		isNodesEmpty = FALSE;


	if(curNodes.nodeNum == twoN) {
		printf("			더 이상 노드를 배치할 공간이 없습니다!\n");
		AfxMessageBox("더 이상 노드를 배치할 공간이 없습니다!\n");
		isNodesFull = TRUE;
		isNodesEmpty = FALSE;
		return;
	}
	//사용자에게 Join할 노드의 IP Address를 받아온다.
	CJoinDlg dlg;
	CString str;
	do {
		sameExist = 0;

		if(dlg.DoModal() == IDOK) {
			printf("      (2) Node Join\n");
			printNodes();

			str.Format("%d.%d.%d.%d", dlg.addr1, dlg.addr2, dlg.addr3, dlg.addr4);
			char *addrBuf = (LPSTR)(LPCTSTR)str; //join에서 사용자에게 받을 IP address 임시 저장

			printf("      2.1 Enter the IP address of the joining node : %s\n", addrBuf);

			inet_aton(addrBuf, &joinNode.ipaddr);
			curID = str_hash(inet_ntoa(joinNode.ipaddr)); //hash fucntion에 ID값을 얻어온다.

			for (i = 0; i < curNodes.nodeNum; i++){ //이미 있는 노드들의 IP 중복 검사
				if(joinNode.ipaddr.S_un.S_addr == curNodes.Node[i].ipaddr.S_un.S_addr){
					printf("        입력한 IP는 이미 존재합니다.\n        다시 한번 joining Node의 IP address를 입력해주세요.\n");
					sameExist = 1;
					break;
					//사용자가 실수로 동일 IP를 입력했다. 다시 첨으로 돌아가자.
				}
			}

			if(sameExist != 1) {
				for (i = 0; i < curNodes.nodeNum; i++) { //이미 있는 노드들의 ID가 중복되는지 검사
					if (curID == curNodes.Node[i].ID) {
						printf("        입력한 IP의 해쉬함수의 결과인 ID가 이미 존재 합니다.\n");
						printf("        임의로 ID를 정하겠습니다.\n");
						curID = randomCurID();
						break;
					}
				}
			}
		}
		else {//IDCANCEL
			printf("사용자가 취소했습니다.\n");
			return;
		}
	}while(sameExist);

	//본격적으로 Join을 하자
	if(!isNodesFull) { //노드들이 들어 갈 공간이 꽉 차지 않았으면 아직 노드를 넣을 수 있다.
		//ID를 집어 넣어주자
		joinNode.ID = curID;

		//만든 joinNode를 curNodes와 CHORD 배열에 포함시킴
		curNodes.Node[curNodes.nodeNum] = joinNode;
		CHORD[curNodes.nodeNum].Self = joinNode;
		curNodes.nodeNum++;

		printf("        - Node ID of %15s is %d\n", inet_ntoa(joinNode.ipaddr), joinNode.ID);
		printf("      2.2 CHORD Network Connection Update\n");
		printNodes();

		join(joinNode, curNodes.Node[0], nID2CPos(joinNode.ID));

		for (j=0; j<curNodes.nodeNum; j++) {
			//nID2CPos() : 전역 변수 CHORD 배열에 있는 한 노드로부터 nodeID가 배열 몇번째 박혀있는지 좀 알자. 그래야 전역 변수 CHORD 배열의 정보를 알아낼 수 있지!
			printf("        - Finger Table (Node %d): Pre(%d) ",
				curNodes.Node[j].ID,
				CHORD[nID2CPos(curNodes.Node[j].ID)].fingerInfo.Pre.ID); //내 자신의 노드 ID와 pre를 출력
			for (k=0; k<N; k++)
				printf("%d(%d) ", k,
				CHORD[nID2CPos(curNodes.Node[j].ID)].fingerInfo.finger[k].ID); //나의 핑거들을 출력
			printf("\n");
		}

		printFiles();
		printf("Join완료 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
		UpdateAllViews(NULL);
	}
}

void CWCHORDDoc::OnStep3Leave()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if(curNodes.nodeNum == twoN)
		isNodesFull = TRUE;
	else
		isNodesFull = FALSE;

	if(curNodes.nodeNum == 0)
		isNodesEmpty = TRUE;
	else
		isNodesEmpty = FALSE;


	if(curNodes.nodeNum == 0) {
		printf("			더 이상 떠날 노드가 존재하지 않습니다!\n");
		AfxMessageBox("더 이상 떠날 노드가 존재하지 않습니다!\n");
		isNodesFull = FALSE;
		isNodesEmpty = TRUE;
		return;
	}

	CLeaveDlg dlg;

	dlg.curNodes = curNodes;
	if(dlg.DoModal() == IDOK) {

		printNodes();
		printFiles();
		leaveNodeID = _ttoi(dlg.selectedNode);
		printf("      3.1 Enter the node ID of the leaving node : %d\n", leaveNodeID);
	}
	else {//IDCANCEL
		printf("사용자가 취소했습니다.\n");
		return;
	}

	if(!isNodesEmpty) {
		printf("      3.2 CHORD Network Connection Update\n");

		leave(leaveNodeID); //진짜 leave의 핵심적인 부분 !!!!!!!!!!!!!!!!!!!!!

		printNodes();

		for (j=0; j<curNodes.nodeNum; j++) {
			//nID2CPos() : 전역 변수 CHORD 배열에 있는 한 노드로부터 nodeID가 배열 몇번째 박혀있는지 좀 알자. 그래야 전역 변수 CHORD 배열의 정보를 알아낼 수 있지!
			printf("        - Finger Table (Node %d): Pre(%d) ",
				curNodes.Node[j].ID,
				CHORD[nID2CPos(curNodes.Node[j].ID)].fingerInfo.Pre.ID); //내 자신의 노드 ID와 pre를 출력
			for (k=0; k<N; k++)
				printf("%d(%d) ", k,
				CHORD[nID2CPos(curNodes.Node[j].ID)].fingerInfo.finger[k].ID); //나의 핑거들을 출력
			printf("\n");
		}

		printFiles();
		printf("Leave완료 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
		UpdateAllViews(NULL);
	}
}

int CWCHORDDoc::refOwnerID(int key)
{
	int j, k, refID = -1, found = 0;

	for (j=0; j<curNodes.nodeNum; j++) { //각 파일의 레퍼런스를 가진 노드를 찾기 위해 모든 노드의 수만큼 돈다.
		found = 0; //반복문을 벗어나기위한 "찾던 파일의 레퍼런스 노드를 찾았다!"를 checking하는 변수
		for (k=0; k<CHORD[j].ownFRef.fileNum; k++)
			if (key == CHORD[j].ownFRef.File[k].key) //찾던 파일의 레퍼런스를 알고있는 노드를 찾았을 때
			{
				refID = CHORD[j].Self.ID;
				found = 1;
				break;
			}
			if (found) break;
	}

	return refID;
}