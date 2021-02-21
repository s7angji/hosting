#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include "resource.h"

#define MAX_CLIENT 4

static SOCKET listen_sock;
fd_set reads, temps;

#define FILE_BUFSIZE	4096			//// 파일 버퍼 크기
static char file_path[256];				//// 사용자가 파일 전송을 지정한 파일의 경로~
static char strFileTitle[100];
static FILE *fp;

static int CorF;	////CHATTING or SENDFILE

#define BUFSIZE     50 // 버퍼 크기
#define CHATTING    0  // 메시지 타입
#define DRAWING     1  // 메시지 타입
#define SENDFILE	2  //// 파일 업로드 메세지 타입
#define WM_DRAWIT   WM_USER+10 // 사용자 정의 윈도우 메시지

// 채팅 메시지
struct CHATMSG
{
	char buf[BUFSIZE+1];
};

// 선 그리기 메시지
struct DRAWMSG
{
	short int x0, y0;
	short int x1, y1;
};

static HWND hEditStatus;
static HWND hButtonSend;
static HWND hButtonSendFile;
static HWND hDrawWnd;

static IN_ADDR ipaddr;
static u_short port;

static HANDLE hServerThread, hClientThread;
static HANDLE hReadEvent, hWriteEvent;

static HINSTANCE hInst;
static BOOL bStart;
static BOOL bServer;

static CHATMSG g_chatmsg;
static SOCKET g_sock;

// 대화상자 메시지 처리
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 윈도우 메시지 처리
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 스레드 함수
DWORD WINAPI Server(LPVOID arg);
DWORD WINAPI Client(LPVOID arg);
DWORD WINAPI ReadThread(LPVOID arg);
DWORD WINAPI WriteThread(LPVOID arg);
DWORD WINAPI MulticastWriteThread(LPVOID arg); //// 서버에서도 모든 클라이언트와 채팅 및 파일전송이 가능하게 해주는 쓰레드

// 유틸리티 함수
void err_display(char *msg); //// 소켓 함수 오류 출력
void err_quit(char *msg);
void DisplayText(char *fmt, ...);
BOOL GetIPAddr(char *name, IN_ADDR *addr);
int recvn(SOCKET s, char *buf, int len, int flags);

// 메인 함수
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return -1;

	// 이벤트 생성
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if(hReadEvent == NULL) return -1;
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(hWriteEvent == NULL) return -1;

	bServer = FALSE;
	bStart = FALSE;
	hInst = hInstance;
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

	// 이벤트 제거
	CloseHandle(hReadEvent);
	CloseHandle(hWriteEvent);

	// 윈속 종료
	WSACleanup();
	return 0;
}

// 대화상자 메시지 처리
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//// 파일 경로 관련 변수
	OPENFILENAME OFN;

	static HWND hButtonStartServer;
	static HWND hButtonStartClient;
	static HWND hEditMsg;
	static DWORD ThreadId;
	static char temp[128];

	switch(uMsg){
	case WM_INITDIALOG:
		{
			hButtonStartServer = GetDlgItem(hDlg, IDC_STARTSERVER);
			hButtonStartClient = GetDlgItem(hDlg, IDC_STARTCLIENT);
			hButtonSend = GetDlgItem(hDlg, IDC_SENDMSG);

			hButtonSendFile = GetDlgItem(hDlg, IDC_SENDFILE); //// 파일 전송 버튼의 핸들을 얻어 온다

			hEditMsg = GetDlgItem(hDlg, IDC_MSG);
			hEditStatus = GetDlgItem(hDlg, IDC_STATUS);
			SendMessage(hEditMsg, EM_SETLIMITTEXT, BUFSIZE, 0);

			EnableWindow(hButtonSend, FALSE);
			EnableWindow(hButtonSendFile, FALSE); //// 초기에 서버와 클라이언트 선택 전에는 필요없는 버튼이다.

			SetDlgItemText(hDlg, IDC_ADDR, "127.0.0.1");
			SetDlgItemText(hDlg, IDC_PORT, "9000");
			// 윈도우 클래스 등록
			WNDCLASS wndclass;
			wndclass.cbClsExtra = 0;
			wndclass.cbWndExtra = 0;
			wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
			wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
			wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			wndclass.hInstance = hInst;
			wndclass.lpszClassName = "MyWindowClass";
			wndclass.lpszMenuName = NULL;
			wndclass.style = CS_HREDRAW | CS_VREDRAW;
			wndclass.lpfnWndProc = (WNDPROC)WndProc;
			if(!RegisterClass(&wndclass)) return -1;

			// 자식 윈도우 생성
			hDrawWnd = CreateWindow("MyWindowClass", 
				"그림 그릴 윈도우", WS_CHILD, 
				15, 250, 545, 285,
				hDlg, (HMENU)NULL, hInst, NULL);
			if(hDrawWnd == NULL) return -1;
			ShowWindow(hDrawWnd, SW_SHOW);
			UpdateWindow(hDrawWnd);

			return TRUE;
		}
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDC_STARTSERVER:
			port = GetDlgItemInt(hDlg, IDC_PORT, NULL, FALSE);
			// 서버 스레드 시작
			hServerThread = CreateThread(NULL, 0, 
				Server, NULL, 0, &ThreadId);
			if(hServerThread == NULL){
				MessageBox(hDlg, "서버를 시작할 수 없습니다.""\r\n프로그램을 종료합니다.","실패", MB_ICONERROR);
				EndDialog(hDlg, 0);	
			}
			else{
				MessageBox(hDlg, "서버를 시작합니다.", "성공", MB_ICONINFORMATION);
				CloseHandle(hServerThread);
				EnableWindow(hButtonStartServer, FALSE);
				EnableWindow(hButtonStartClient, FALSE);
				EnableWindow(hButtonSend, TRUE);
				EnableWindow(hButtonSendFile, TRUE); //// 이제 파일 전송이 가능해진다. 버튼을 살리자

				SetFocus(hEditMsg);
			}
			return TRUE;
		case IDC_STARTCLIENT:
			GetDlgItemText(hDlg, IDC_ADDR, temp, 128);
			if(GetIPAddr(temp, &ipaddr) == FALSE){
				MessageBox(hDlg, "잘못된 주소입니다.", "실패", MB_ICONERROR);
				return TRUE;
			}
			port = GetDlgItemInt(hDlg, IDC_PORT, NULL, FALSE);
			// 클라이언트 스레드 시작
			hClientThread = CreateThread(NULL, 0, 
				Client, NULL, 0, &ThreadId);
			if(hClientThread == NULL){
				MessageBox(hDlg, "클라이언트를 시작할 수 없습니다.""\r\n프로그램을 종료합니다.", "실패", MB_ICONERROR);
				EndDialog(hDlg, 0);	
			}
			else{
				EnableWindow(hButtonStartServer, FALSE);
				EnableWindow(hButtonStartClient, FALSE);
				EnableWindow(hButtonSend, TRUE);

				EnableWindow(hButtonSendFile, TRUE); //// 이제 파일 전송이 가능해진다. 버튼을 살리자

				SetFocus(hEditMsg);
				MessageBox(hDlg, "클라이언트를 시작합니다.", "성공", MB_ICONINFORMATION);
			}
			return TRUE;
		case IDC_SENDMSG:
			// 읽기 완료를 기다림
			EnableWindow(hButtonSend, FALSE);
			EnableWindow(hButtonSendFile, FALSE);
			WaitForSingleObject(hReadEvent, INFINITE);
			bStart = FALSE; //// 그리기는 못하게 막음
			CorF = CHATTING; //// WriteThread가 채팅을 처리 할 수 있도록 셋팅
			GetDlgItemText(hDlg, IDC_MSG, g_chatmsg.buf, BUFSIZE+1);
			// 쓰기 완료를 알림
			SetEvent(hWriteEvent);
			// 입력된 텍스트 전체를 선택 표시
			SendMessage(hEditMsg, EM_SETSEL, 0, -1);
			return TRUE;
		case IDC_SENDFILE: //// 추가로 구현해야 할 부분
			EnableWindow(hButtonSend, FALSE);
			EnableWindow(hButtonSendFile, FALSE);
			WaitForSingleObject(hReadEvent, INFINITE);
			CorF = SENDFILE; //// WriteThread가 파일 전송을 처리 할 수 있도록 셋팅

			//// 사용자에게서 업로드 할 파일 경로를 얻어 온다.
			memset(&OFN, 0, sizeof(OPENFILENAME));
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner=hDlg;
			OFN.lpstrFilter="Every File(*.*)\0*.*\0Text File\0*.txt;*.doc\0";
			OFN.lpstrFile=file_path;
			OFN.lpstrFileTitle=strFileTitle;
			OFN.nMaxFile=MAX_PATH;
			OFN.nMaxFileTitle=MAX_PATH;
			if (!GetOpenFileName(&OFN)) {
				EnableWindow(hButtonSend, TRUE);
				EnableWindow(hButtonSendFile, TRUE);
				SetEvent(hReadEvent);
				return FALSE;
			}

			// 파일 열기
			fp = fopen(file_path, "rb");
			if(fp == NULL){
				MessageBox(hDlg, file_path, "파일 열기 실패", MB_ICONERROR);
				perror("파일 입출력 오류");
				return -1;
			}
			bStart = FALSE; //// 그리기는 못하게 막음
			MessageBox(hDlg, file_path, "파일 열기 성공", MB_OK);	

			SetEvent(hWriteEvent);

			return TRUE;
		case IDCANCEL: 
			// 애플리케이션 종료
			if(MessageBox(hDlg, "정말로 종료하시겠습니까?", "확인", MB_YESNO) == IDYES)
			{
				EndDialog(hDlg, 0);
			}
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

// 서버 시작
DWORD WINAPI Server(LPVOID arg)
{
	int retval;

	// socket()
	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sock == INVALID_SOCKET) err_quit("socket()");	
	
	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("bind()");
	
	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if(retval == SOCKET_ERROR) err_quit("listen()");

	HANDLE hThread[MAX_CLIENT + 1]; //// 0 인덱스 사용 안함 fd_set의 소켓들과 대응 되는 인덱스값 마다 만들어질 쓰레드. 0은 listen_sock
	DWORD ThreadId[MAX_CLIENT + 1];
	char temp[128];

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int arrIndex;
	int clntLen;
	SOCKET hClntSock;
	SOCKADDR_IN clntAddr;

	int clientCnt = 0;
	int i;


	int numtotal = 0;
	char filename[256];
	int totalbytes;
	char file_buf[FILE_BUFSIZE];

	CHATMSG chatmsg;
	DRAWMSG drawmsg;
	int type;

	FD_ZERO(&reads);
	FD_SET(listen_sock, &reads);

	TIMEVAL timeout; // the same as struct timeval timeout;
	while(1){
		temps=reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		if(select(0, &temps, 0, 0, &timeout)==SOCKET_ERROR) {
			MessageBox(NULL, "select() : SOCKET_ERROR", "Error", MB_ICONINFORMATION);
			break;
		}

		bServer = TRUE;

		for(arrIndex=0; arrIndex<reads.fd_count; arrIndex++) {
			if(FD_ISSET(reads.fd_array[arrIndex], &temps)) {
				if(reads.fd_array[arrIndex]==listen_sock) { //Connection Request
					if(clientCnt == MAX_CLIENT) {
						MessageBox(NULL, "더 이상 클라이언트 접속을 받아 들일 수 없습니다.", "알림", MB_ICONINFORMATION);

						clntLen = sizeof(clntAddr);
						hClntSock = accept(listen_sock, (SOCKADDR*)&clntAddr, &clntLen);

						// 바로 FIN을 보내서 클라이언트에게 더 이상 접속 불가를 알린다.
						closesocket(hClntSock);

						continue;
					}

					clntLen = sizeof(clntAddr);
					hClntSock = accept(listen_sock, (SOCKADDR*)&clntAddr, &clntLen);

					clientCnt++;

					if(hClntSock == INVALID_SOCKET)
						err_quit("accept()");

					FD_SET(hClntSock, &reads);

					wsprintf(temp, "클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port));
					MessageBox(NULL, temp, "알림", MB_ICONINFORMATION);

					for(i = 0 ; i < reads.fd_count; i++) {
						if(reads.fd_array[i] == hClntSock)
							break;
					}

					// 쓰기 스레드 생성
					hThread[i] = CreateThread(NULL, 0, MulticastWriteThread, (LPVOID)hClntSock, 0, &ThreadId[i]);

					if(hThread[i] == NULL){
						MessageBox(NULL, "스레드를 시작할 수 없습니다.""\r\n프로그램을 종료합니다.", "실패", MB_ICONERROR);
						exit(-1);
					}

					if(clientCnt == 1) {
						EnableWindow(hButtonSend, TRUE);
						EnableWindow(hButtonSendFile, TRUE);
						bStart = TRUE;
					}
				}
				else { //If there are data to send
					retval = recvn(reads.fd_array[arrIndex], (char *)&type, sizeof(type), 0);
					if(retval == 0 || retval == SOCKET_ERROR){
						MessageBox(NULL, "클라이언트가 접속을 끊었습니다", "알림", MB_OK);

						CloseHandle(hThread[arrIndex]);
						//쓰레드를 관리하는 배열들도 빈 인덱스가 없도록 한다.
						for(i = arrIndex; i < clientCnt; i++) {
							hThread[i] = hThread[i + 1];
						}
						clientCnt--;

						if(clientCnt == 0) {
							MessageBox(NULL, "클라이언트가 모두 떠났습니다.", "알림", MB_OK);
							bStart = FALSE;
							EnableWindow(hButtonSend, FALSE);
							EnableWindow(hButtonSendFile, FALSE);
						}

						FD_CLR(reads.fd_array[arrIndex], &reads);
						continue;
					}
					else { // Receive Data ///////////////////////////////////////////////////////////////////////////////////////////////

						if(type == CHATTING){
							retval = recvn(reads.fd_array[arrIndex], chatmsg.buf, sizeof(g_chatmsg.buf), 0);
							if(retval == 0 || retval == SOCKET_ERROR)
								continue;
							DisplayText("[받은 메시지] %s\r\n", chatmsg.buf);

							// 서버에 접속한 모든 클라이언트에게 데이터를 보낸다.
							for(i = 1; i < reads.fd_count; i++) {
								if(i == arrIndex)
									continue;

								// 데이터 보내기
								type = CHATTING;
								retval = send(reads.fd_array[i], (char *)&type, sizeof(type), 0);
								if(retval == SOCKET_ERROR){
									DisplayText("채팅 메세지 보내기 오류\r\n");
									break;
								}
								retval = send(reads.fd_array[i], (char *)&chatmsg.buf, sizeof(g_chatmsg.buf), 0);
								if(retval == SOCKET_ERROR){
									DisplayText("채팅 메세지 보내기 오류\r\n");
									break;
								}
							}
						}
						else if(type == SENDFILE) {
							// 서버에 접속한 모든 클라이언트에게 데이터 타입 SENDFILE을 보낸다.
							for(i = 1; i < reads.fd_count; i++) {
								if(reads.fd_array[i] == reads.fd_array[arrIndex])
									continue;

								type = SENDFILE;
								retval = send(reads.fd_array[i], (char *)&type, sizeof(type), 0);
								if(retval == SOCKET_ERROR){
									break;
								}
							}

							// 파일 이름 받기
							ZeroMemory(filename, 256);
							retval = recvn(reads.fd_array[arrIndex], filename, 256, 0);
							if(retval == SOCKET_ERROR){
								err_display("recv()");
								closesocket(reads.fd_array[arrIndex]);
								break;
							}
							DisplayText("-> 받을 파일 이름: %s\r\n", filename);

							// 서버에 접속한 모든 클라이언트에게 파일 이름을 보낸다.
							for(i = 1; i < reads.fd_count; i++) {
								if(reads.fd_array[i] == reads.fd_array[arrIndex])
									continue;

								// 파일 이름 보내기
								retval = send(reads.fd_array[i], filename, 256, 0);
								if(retval == SOCKET_ERROR) err_quit("send()");
							}

							// 파일 크기 받기
							retval = recvn(reads.fd_array[arrIndex], (char *)&totalbytes, sizeof(totalbytes), 0);
							if(retval == SOCKET_ERROR){
								err_display("recv()");
								closesocket(reads.fd_array[arrIndex]);
								break;
							}
							DisplayText("-> 받을 파일 크기: %d\r\n", totalbytes);

							// 서버에 접속한 모든 클라이언트에게 파일 크기를 보낸다.
							for(i = 1; i < reads.fd_count; i++) {
								if(reads.fd_array[i] == reads.fd_array[arrIndex])
									continue;

								// 파일 크기 보내기
								retval = send(reads.fd_array[i], (char *)&totalbytes, sizeof(totalbytes), 0);
								if(retval == SOCKET_ERROR) err_quit("send()");
							}

							// 파일 열기
							FILE *fp = fopen(filename, "wb");
							if(fp == NULL){
								DisplayText("-> 파일 입출력 오류\r\n");
								perror("파일 입출력 오류");
								closesocket(reads.fd_array[arrIndex]);
								break;
							}

							// 파일 데이터 받기
							int received;
							int left = totalbytes;
							while(left > 0){
								if(left > FILE_BUFSIZE)
									received = recv(reads.fd_array[arrIndex], file_buf, FILE_BUFSIZE, 0);
								else
									received = recv(reads.fd_array[arrIndex], file_buf, left, 0);


								if(received == SOCKET_ERROR){
									DisplayText("-> ReadThread(), recvn() : Error\r\n");
									err_display("recv()");
									break;
								}
								else if(received == 0) 
									break;
								else{
									// 서버에 접속한 모든 클라이언트에게 파일 내용을 보낸다.
									for(i = 1; i < reads.fd_count; i++) {
										if(reads.fd_array[i] == reads.fd_array[arrIndex])
											continue;

										retval = send(reads.fd_array[i], file_buf, received, 0);
										if(retval == SOCKET_ERROR){
											err_display("send()");
											break;
										}
									}

									fwrite(file_buf, 1, received, fp);
									if(ferror(fp)){
										DisplayText("파일 입출력 오류\r\n");
										perror("파일 입출력 오류");
										break;
									}
									left -= received;
								}
							}
							fclose(fp);

							// 전송 결과 출력
							if(left == 0)
								DisplayText("-> 파일 전송 완료!\r\n");
							else
								DisplayText("-> 파일 전송 실패!\r\n");

						}
						else if(type == DRAWING){
							retval = recvn(reads.fd_array[arrIndex], (char *)&drawmsg, sizeof(drawmsg), 0);
							if(retval == 0 || retval == SOCKET_ERROR)
								break;

							// 서버에 접속한 모든 클라이언트에게 직선의 내용을 보낸다.
							for(i = 1; i < reads.fd_count; i++) {
								if(reads.fd_array[i] == reads.fd_array[arrIndex])
									continue;
								type = DRAWING;
								send(reads.fd_array[i], (char *)&type, sizeof(type), 0);
								send(reads.fd_array[i], (char *)&drawmsg, sizeof(drawmsg), 0);
							}

							SendMessage(hDrawWnd, WM_DRAWIT,
								MAKEWPARAM(drawmsg.x0, drawmsg.y0), 
								MAKELPARAM(drawmsg.x1, drawmsg.y1));
						}
					} // Receive Data ///////////////////////////////////////////////////////////////////////////////////////////////
				}
			} //if(FD_ISSET(reads.fd_array[arrIndex], &temps)) end
		} //for(arrIndex=0; arrIndex<reads.fd_count; arrIndex++)  end
	}

	closesocket(listen_sock);

	return 0;
}

// 클라이언트 시작
DWORD WINAPI Client(LPVOID arg)
{
	int retval;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET) err_quit("socket()");	
	
	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr = ipaddr;
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("connect()");

	HANDLE hThread[2];
	DWORD ThreadId[2];

	// 읽기 스레드 생성
	hThread[0] = CreateThread(NULL, 0, ReadThread, (LPVOID)sock, 0, &ThreadId[0]);
	// 쓰기 스레드 생성
	hThread[1] = CreateThread(NULL, 0, WriteThread, (LPVOID)sock, 0, &ThreadId[1]);
	if(hThread[0] == NULL || hThread[1] == NULL){
		MessageBox(NULL, "스레드를 시작할 수 없습니다.""\r\n프로그램을 종료합니다.", "실패", MB_ICONERROR);
		exit(-1);
	}

	bStart = TRUE;
	g_sock = sock;

	// 스레드 종료 대기
	retval = WaitForMultipleObjects(2, hThread, FALSE, INFINITE);
	retval -= WAIT_OBJECT_0;
	if(retval == 0)
		TerminateThread(hThread[1], -1);
	else
		TerminateThread(hThread[0], -1);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);

	bStart = FALSE;

	MessageBox(NULL, "서버가 접속을 끊었습니다", "알림", MB_ICONINFORMATION);
	EnableWindow(hButtonSend, FALSE);
	EnableWindow(hButtonSendFile, FALSE);

	return 0;
}


DWORD WINAPI ReadThread(LPVOID arg) // 데이터 받기
{
	int numtotal = 0;
	char filename[256];
	int totalbytes;
	char file_buf[FILE_BUFSIZE];	////파일 전송에 이용할 버퍼 

	SOCKET sock = (SOCKET)arg;
	CHATMSG chatmsg;
	DRAWMSG drawmsg;
	int type;
	int retval;

	while(1){
		retval = recvn(sock, (char *)&type, sizeof(type), 0);
		if(retval == 0 || retval == SOCKET_ERROR){
			break;
		}

		if(type == CHATTING){
			retval = recvn(sock, chatmsg.buf, sizeof(g_chatmsg.buf), 0);
			if(retval == 0 || retval == SOCKET_ERROR)
				break;
			DisplayText("[받은 메시지] %s\r\n", chatmsg.buf);
		}
		else if(type == DRAWING){
			retval = recvn(sock, (char *)&drawmsg, sizeof(drawmsg), 0);
			if(retval == 0 || retval == SOCKET_ERROR)
				break;
			SendMessage(hDrawWnd, WM_DRAWIT,
				MAKEWPARAM(drawmsg.x0, drawmsg.y0), 
				MAKELPARAM(drawmsg.x1, drawmsg.y1));
		}
		else if(type == SENDFILE) {
			// 파일 이름 받기
			ZeroMemory(filename, 256);
			retval = recvn(sock, filename, 256, 0);
			if(retval == SOCKET_ERROR){
				err_display("recv()");
				closesocket(sock);
				break;
			}
			DisplayText("-> 받을 파일 이름: %s\r\n", filename);

			// 파일 크기 받기
			retval = recvn(sock, (char *)&totalbytes, sizeof(totalbytes), 0);
			if(retval == SOCKET_ERROR){
				err_display("recv()");
				closesocket(sock);
				break;
			}
			DisplayText("-> 받을 파일 크기: %d\r\n", totalbytes);

			// 파일 열기
			FILE *fp = fopen(filename, "wb");
			if(fp == NULL){
				DisplayText("-> 파일 입출력 오류\r\n");
				perror("파일 입출력 오류");
				closesocket(sock);
				break;
			}

			// 파일 데이터 받기
			int received;
			int left = totalbytes;
			while(left > 0){
				if(left > FILE_BUFSIZE)
					received = recv(sock, file_buf, FILE_BUFSIZE, 0);
				else
					received = recv(sock, file_buf, left, 0);

				if(received == SOCKET_ERROR){
					DisplayText("-> ReadThread(), recvn() : Error\r\n");
					err_display("recv()");
					break;
				}
				else if(received == 0) 
					break;
				else{
					fwrite(file_buf, 1, received, fp);
					if(ferror(fp)){
						DisplayText("파일 입출력 오류\r\n");
						perror("파일 입출력 오류");
						break;
					}
					left -= received;
				}
			}
			fclose(fp);

			// 전송 결과 출력
			if(left == 0)
				DisplayText("-> 파일 전송 완료!\r\n");
			else
				DisplayText("-> 파일 전송 실패!\r\n");

		}
	}

	// closesocket()
	closesocket(sock);

	return 0;
}

// 데이터 보내기
DWORD WINAPI WriteThread(LPVOID arg)
{
	SOCKET sock = (SOCKET)arg;
	int type;
	int retval;

	char filename[256];
	int totalbytes;
	// 파일 데이터 전송에 사용할 변수
	char buf[FILE_BUFSIZE];
	int numread;
	int numtotal = 0;

	// 서버와 데이터 통신
	while(1){
		// 쓰기 완료를 기다림
		WaitForSingleObject(hWriteEvent, INFINITE);

		switch(CorF) {
		case CHATTING:
			// 문자열 길이가 0이면 보내지 않음
			if(strlen(g_chatmsg.buf) == 0){
				// 보내기 버튼 활성화
				EnableWindow(hButtonSend, TRUE);
				EnableWindow(hButtonSendFile, TRUE);
				// 읽기 완료를 알림
				SetEvent(hReadEvent);
				bStart = TRUE;
				continue;
			}

			// 데이터 보내기
			type = CHATTING;
			retval = send(sock, (char *)&type, sizeof(type), 0);
			if(retval == SOCKET_ERROR){
				break;
			}

			retval = send(sock, (char *)&g_chatmsg.buf, sizeof(g_chatmsg.buf), 0);
			if(retval == SOCKET_ERROR){
				break;
			}
			DisplayText("[보낸 메시지] %s\r\n", g_chatmsg.buf);

			bStart = TRUE;
			// 보내기 버튼 활성화
			EnableWindow(hButtonSend, TRUE);
			EnableWindow(hButtonSendFile, TRUE);
			// 읽기 완료를 알림
			SetEvent(hReadEvent);
			break;
		case SENDFILE:

			// 데이터 보내기
			type = SENDFILE;
			retval = send(sock, (char *)&type, sizeof(type), 0);
			if(retval == SOCKET_ERROR){
				break;
			}

			// 파일 이름 보내기
			ZeroMemory(filename, 256);
			sprintf(filename, strFileTitle);
			retval = send(sock, filename, 256, 0);
			if(retval == SOCKET_ERROR) err_quit("send()");

			// 파일 크기 얻기
			fseek(fp, 0, SEEK_END);
			totalbytes = ftell(fp);

			// 파일 크기 보내기
			retval = send(sock, (char *)&totalbytes, sizeof(totalbytes), 0);
			if(retval == SOCKET_ERROR) err_quit("send()");	


			// 파일 데이터 보내기
			numtotal = 0;
			rewind(fp); // 파일 포인터를 제일 앞으로 이동
			while(1){
				numread = fread(buf, 1, FILE_BUFSIZE, fp);
				if(numread > 0){
					retval = send(sock, buf, numread, 0);
					if(retval == SOCKET_ERROR){
						err_display("send()");
						break;
					}
					numtotal += numread;
				}
				else if(numread == 0 && numtotal == totalbytes){
					DisplayText("파일 전송 완료!: %d 바이트\r\n", numtotal);
					break;
				}
				else {
					DisplayText("파일 입출력 오류\r\n");
					perror("파일 입출력 오류");
					break;
				}
			}
			fclose(fp);
			bStart = TRUE;
			// 보내기 버튼 활성화
			EnableWindow(hButtonSend, TRUE);
			EnableWindow(hButtonSendFile, TRUE);
			// 완료를 알림
			SetEvent(hReadEvent);
			break;
		default:
			MessageBox(NULL, "WriteThread() : 알 수 없는 유형의 타입입니다.", "Error", MB_ICONERROR);
			break;
		}
	}

	// closesocket()
	closesocket(sock);

	return 0;
}

// 서버가 각 클라이언트에게 데이터 보내기
DWORD WINAPI MulticastWriteThread(LPVOID arg) {
	SOCKET sock = (SOCKET)arg;
	int type;
	int retval;

	char filename[256];
	int totalbytes;
	// 파일 데이터 전송에 사용할 변수
	char buf[FILE_BUFSIZE];
	int numread;
	int numtotal = 0;

	// 서버와 데이터 통신
	while(1){
		// 쓰기 완료를 기다림
		WaitForSingleObject(hWriteEvent, INFINITE);

		switch(CorF) {
		case CHATTING:
			// 문자열 길이가 0이면 보내지 않음
			if(strlen(g_chatmsg.buf) == 0){
				// 보내기 버튼 활성화
				EnableWindow(hButtonSend, TRUE);
				EnableWindow(hButtonSendFile, TRUE);
				// 읽기 완료를 알림
				SetEvent(hReadEvent);
				bStart = TRUE;
				continue;
			}

			int i;
			for(i = 1 ; i < reads.fd_count; i++) {
				// 데이터 보내기
				type = CHATTING;
				retval = send(reads.fd_array[i], (char *)&type, sizeof(type), 0);
				if(retval == SOCKET_ERROR){
					break;
				}

				retval = send(reads.fd_array[i], (char *)&g_chatmsg.buf, sizeof(g_chatmsg.buf), 0);
				if(retval == SOCKET_ERROR){
					break;
				}
			}

			DisplayText("[보낸 메시지] %s\r\n", g_chatmsg.buf);

			bStart = TRUE;
			// 보내기 버튼 활성화
			EnableWindow(hButtonSend, TRUE);
			EnableWindow(hButtonSendFile, TRUE);
			// 읽기 완료를 알림
			SetEvent(hReadEvent);
			break;
		case SENDFILE:

			// 서버에 접속한 모든 클라이언트에게 데이터 타입 SENDFILE을 보낸다.
			for(i = 1; i < reads.fd_count; i++) {
				type = SENDFILE;
				retval = send(reads.fd_array[i], (char *)&type, sizeof(type), 0);
				if(retval == SOCKET_ERROR){
					break;
				}
			}

			// 파일 이름 보내기
			ZeroMemory(filename, 256);
			sprintf(filename, strFileTitle);
			// 서버에 접속한 모든 클라이언트에게 파일 이름을 보낸다.
			for(i = 1; i < reads.fd_count; i++) {
				// 파일 이름 보내기
				retval = send(reads.fd_array[i], filename, 256, 0);
				if(retval == SOCKET_ERROR) err_quit("send()");
			}

			// 파일 크기 얻기
			fseek(fp, 0, SEEK_END);
			totalbytes = ftell(fp);
			// 서버에 접속한 모든 클라이언트에게 파일 크기를 보낸다.
			for(i = 1; i < reads.fd_count; i++) {
				// 파일 크기 보내기
				retval = send(reads.fd_array[i], (char *)&totalbytes, sizeof(totalbytes), 0);
				if(retval == SOCKET_ERROR) err_quit("send()");
			}

			// 파일 데이터 보내기
			numtotal = 0;
			rewind(fp); // 파일 포인터를 제일 앞으로 이동
			while(1){
				numread = fread(buf, 1, FILE_BUFSIZE, fp);
				if(numread > 0){
					// 서버에 접속한 모든 클라이언트에게 파일 내용을 보낸다.
					for(i = 1; i < reads.fd_count; i++) {
						retval = send(reads.fd_array[i], buf, numread, 0);
						if(retval == SOCKET_ERROR){
							err_display("send()");
							break;
						}
					}
					numtotal += numread;
				}
				else if(numread == 0 && numtotal == totalbytes){
					DisplayText("파일 전송 완료!: %d 바이트\r\n", numtotal);
					break;
				}
				else {
					DisplayText("파일 입출력 오류\r\n");
					perror("파일 입출력 오류");
					break;
				}
			}
			fclose(fp);
			bStart = TRUE;
			// 보내기 버튼 활성화
			EnableWindow(hButtonSend, TRUE);
			EnableWindow(hButtonSendFile, TRUE);
			// 완료를 알림
			SetEvent(hReadEvent);
			break;
		default:
			MessageBox(NULL, "WriteThread() : 알 수 없는 유형의 타입입니다.", "Error", MB_ICONERROR);
			break;
		}
	}

	// closesocket()
	closesocket(sock);

	return 0;
}

// 윈도우 메시지 처리
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT ps;
	RECT rect;
	int cx, cy;
	static HDC hDCMem;
	static HBITMAP hBitmap;
	static int x0, y0;
	static int x1, y1;
	static BOOL bDrawing = FALSE;
	DRAWMSG drawmsg;
	int type = DRAWING;

	switch(uMsg){
	case WM_CREATE:
		hDC = GetDC(hWnd);
		// 화면을 저장할 비트맵 생성
		cx = GetDeviceCaps(hDC, HORZRES);
		cy = GetDeviceCaps(hDC, VERTRES);
		hBitmap = CreateCompatibleBitmap(hDC, cx, cy);
		// 메모리 DC 생성
		hDCMem = CreateCompatibleDC(hDC);
		// 메모리 DC에 비트맵 선택
		SelectObject(hDCMem, hBitmap);
		// 메모리 DC 화면을 흰색으로 칠함
		SelectObject(hDCMem, GetStockObject(WHITE_BRUSH));
		SelectObject(hDCMem, GetStockObject(WHITE_PEN));
		Rectangle(hDCMem, 0, 0, cx, cy);
		ReleaseDC(hWnd, hDC);
		return 0;
	case WM_LBUTTONDOWN:
		x0 = LOWORD(lParam);
		y0 = HIWORD(lParam);
		bDrawing = TRUE;
		return 0;

	case WM_MOUSEMOVE:
		if(bDrawing){
			hDC = GetDC(hWnd);
			x1 = LOWORD(lParam);
			y1 = HIWORD(lParam);

			// 화면에 그리기
			SelectObject(hDC, GetStockObject(BLACK_PEN));
			MoveToEx(hDC, x0, y0, NULL);
			LineTo(hDC, x1, y1);
			// 메모리 비트맵에 그리기
			SelectObject(hDCMem, GetStockObject(BLACK_PEN));
			MoveToEx(hDCMem, x0, y0, NULL);
			LineTo(hDCMem, x1, y1);

			if(bStart){
				if(bServer) {
					// 서버에 접속한 모든 클라이언트에게 직선의 내용을 보낸다.
					int i;
					for(i = 1; i < reads.fd_count; i++) {
						type = DRAWING;
						send(reads.fd_array[i], (char *)&type, sizeof(type), 0);
						drawmsg.x0 = x0;
						drawmsg.y0 = y0;
						drawmsg.x1 = x1;
						drawmsg.y1 = y1;
						send(reads.fd_array[i], (char *)&drawmsg, sizeof(drawmsg), 0);
					}
				}
				else { // 선 그리기 정보 보내기
					type = DRAWING;
					send(g_sock, (char *)&type, sizeof(type), 0);
					drawmsg.x0 = x0;
					drawmsg.y0 = y0;
					drawmsg.x1 = x1;
					drawmsg.y1 = y1;
					send(g_sock, (char *)&drawmsg, sizeof(drawmsg), 0);
				}
			}
			x0 = x1;
			y0 = y1;
			ReleaseDC(hWnd, hDC);
		}
		return 0;
	case WM_LBUTTONUP:
		bDrawing = FALSE;
		return 0;
	case WM_DRAWIT:
		hDC = GetDC(hWnd);

		// 화면에 그리기
		SelectObject(hDC, GetStockObject(BLACK_PEN));
		MoveToEx(hDC, LOWORD(wParam), HIWORD(wParam), NULL);
		LineTo(hDC, LOWORD(lParam), HIWORD(lParam));
		// 메모리 비트맵에 그리기
		SelectObject(hDCMem, GetStockObject(BLACK_PEN));
		MoveToEx(hDCMem, LOWORD(wParam), HIWORD(wParam), NULL);
		LineTo(hDCMem, LOWORD(lParam), HIWORD(lParam));

		ReleaseDC(hWnd, hDC);
		return 0;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		// 메모리 비트맵에 저장된 그림을 화면에 복사하기
		GetClientRect(hWnd, &rect);
		BitBlt(hDC, 0, 0, rect.right - rect.left,
			rect.bottom - rect.top, hDCMem, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		DeleteObject(hBitmap);
		DeleteDC(hDCMem);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}

// 에디트 컨트롤에 문자열 출력
void DisplayText(char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	char cbuf[BUFSIZE+50];
	vsprintf(cbuf, fmt, arg);

	int nLength = GetWindowTextLength(hEditStatus);
	SendMessage(hEditStatus, EM_SETSEL, nLength, nLength);
	SendMessage(hEditStatus, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	va_end(arg);
}

// 도메인 이름 -> IP 주소
BOOL GetIPAddr(char *name, IN_ADDR *addr)
{
	HOSTENT *ptr = gethostbyname(name);
	if(ptr == NULL)
		return FALSE;
	memcpy(addr, ptr->h_addr, ptr->h_length);
	return TRUE;
}

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while(left > 0){
		received = recv(s, ptr, left, flags);
		if(received == SOCKET_ERROR) 
			return SOCKET_ERROR;
		else if(received == 0) 
			break;
		left -= received;
		ptr += received;
	}
	return (len - left);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, msg, "Error", MB_ICONERROR);
	printf("[%s] %s", msg, (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
}