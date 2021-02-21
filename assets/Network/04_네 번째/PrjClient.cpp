#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include "resource.h"

#define MAX_CLIENT 4

static SOCKET listen_sock;
fd_set reads, temps;

#define FILE_BUFSIZE	4096			//// ���� ���� ũ��
static char file_path[256];				//// ����ڰ� ���� ������ ������ ������ ���~
static char strFileTitle[100];
static FILE *fp;

static int CorF;	////CHATTING or SENDFILE

#define BUFSIZE     50 // ���� ũ��
#define CHATTING    0  // �޽��� Ÿ��
#define DRAWING     1  // �޽��� Ÿ��
#define SENDFILE	2  //// ���� ���ε� �޼��� Ÿ��
#define WM_DRAWIT   WM_USER+10 // ����� ���� ������ �޽���

// ä�� �޽���
struct CHATMSG
{
	char buf[BUFSIZE+1];
};

// �� �׸��� �޽���
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

// ��ȭ���� �޽��� ó��
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

// ������ �޽��� ó��
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// ������ �Լ�
DWORD WINAPI Server(LPVOID arg);
DWORD WINAPI Client(LPVOID arg);
DWORD WINAPI ReadThread(LPVOID arg);
DWORD WINAPI WriteThread(LPVOID arg);
DWORD WINAPI MulticastWriteThread(LPVOID arg); //// ���������� ��� Ŭ���̾�Ʈ�� ä�� �� ���������� �����ϰ� ���ִ� ������

// ��ƿ��Ƽ �Լ�
void err_display(char *msg); //// ���� �Լ� ���� ���
void err_quit(char *msg);
void DisplayText(char *fmt, ...);
BOOL GetIPAddr(char *name, IN_ADDR *addr);
int recvn(SOCKET s, char *buf, int len, int flags);

// ���� �Լ�
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return -1;

	// �̺�Ʈ ����
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if(hReadEvent == NULL) return -1;
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(hWriteEvent == NULL) return -1;

	bServer = FALSE;
	bStart = FALSE;
	hInst = hInstance;
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

	// �̺�Ʈ ����
	CloseHandle(hReadEvent);
	CloseHandle(hWriteEvent);

	// ���� ����
	WSACleanup();
	return 0;
}

// ��ȭ���� �޽��� ó��
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//// ���� ��� ���� ����
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

			hButtonSendFile = GetDlgItem(hDlg, IDC_SENDFILE); //// ���� ���� ��ư�� �ڵ��� ��� �´�

			hEditMsg = GetDlgItem(hDlg, IDC_MSG);
			hEditStatus = GetDlgItem(hDlg, IDC_STATUS);
			SendMessage(hEditMsg, EM_SETLIMITTEXT, BUFSIZE, 0);

			EnableWindow(hButtonSend, FALSE);
			EnableWindow(hButtonSendFile, FALSE); //// �ʱ⿡ ������ Ŭ���̾�Ʈ ���� ������ �ʿ���� ��ư�̴�.

			SetDlgItemText(hDlg, IDC_ADDR, "127.0.0.1");
			SetDlgItemText(hDlg, IDC_PORT, "9000");
			// ������ Ŭ���� ���
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

			// �ڽ� ������ ����
			hDrawWnd = CreateWindow("MyWindowClass", 
				"�׸� �׸� ������", WS_CHILD, 
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
			// ���� ������ ����
			hServerThread = CreateThread(NULL, 0, 
				Server, NULL, 0, &ThreadId);
			if(hServerThread == NULL){
				MessageBox(hDlg, "������ ������ �� �����ϴ�.""\r\n���α׷��� �����մϴ�.","����", MB_ICONERROR);
				EndDialog(hDlg, 0);	
			}
			else{
				MessageBox(hDlg, "������ �����մϴ�.", "����", MB_ICONINFORMATION);
				CloseHandle(hServerThread);
				EnableWindow(hButtonStartServer, FALSE);
				EnableWindow(hButtonStartClient, FALSE);
				EnableWindow(hButtonSend, TRUE);
				EnableWindow(hButtonSendFile, TRUE); //// ���� ���� ������ ����������. ��ư�� �츮��

				SetFocus(hEditMsg);
			}
			return TRUE;
		case IDC_STARTCLIENT:
			GetDlgItemText(hDlg, IDC_ADDR, temp, 128);
			if(GetIPAddr(temp, &ipaddr) == FALSE){
				MessageBox(hDlg, "�߸��� �ּ��Դϴ�.", "����", MB_ICONERROR);
				return TRUE;
			}
			port = GetDlgItemInt(hDlg, IDC_PORT, NULL, FALSE);
			// Ŭ���̾�Ʈ ������ ����
			hClientThread = CreateThread(NULL, 0, 
				Client, NULL, 0, &ThreadId);
			if(hClientThread == NULL){
				MessageBox(hDlg, "Ŭ���̾�Ʈ�� ������ �� �����ϴ�.""\r\n���α׷��� �����մϴ�.", "����", MB_ICONERROR);
				EndDialog(hDlg, 0);	
			}
			else{
				EnableWindow(hButtonStartServer, FALSE);
				EnableWindow(hButtonStartClient, FALSE);
				EnableWindow(hButtonSend, TRUE);

				EnableWindow(hButtonSendFile, TRUE); //// ���� ���� ������ ����������. ��ư�� �츮��

				SetFocus(hEditMsg);
				MessageBox(hDlg, "Ŭ���̾�Ʈ�� �����մϴ�.", "����", MB_ICONINFORMATION);
			}
			return TRUE;
		case IDC_SENDMSG:
			// �б� �ϷḦ ��ٸ�
			EnableWindow(hButtonSend, FALSE);
			EnableWindow(hButtonSendFile, FALSE);
			WaitForSingleObject(hReadEvent, INFINITE);
			bStart = FALSE; //// �׸���� ���ϰ� ����
			CorF = CHATTING; //// WriteThread�� ä���� ó�� �� �� �ֵ��� ����
			GetDlgItemText(hDlg, IDC_MSG, g_chatmsg.buf, BUFSIZE+1);
			// ���� �ϷḦ �˸�
			SetEvent(hWriteEvent);
			// �Էµ� �ؽ�Ʈ ��ü�� ���� ǥ��
			SendMessage(hEditMsg, EM_SETSEL, 0, -1);
			return TRUE;
		case IDC_SENDFILE: //// �߰��� �����ؾ� �� �κ�
			EnableWindow(hButtonSend, FALSE);
			EnableWindow(hButtonSendFile, FALSE);
			WaitForSingleObject(hReadEvent, INFINITE);
			CorF = SENDFILE; //// WriteThread�� ���� ������ ó�� �� �� �ֵ��� ����

			//// ����ڿ��Լ� ���ε� �� ���� ��θ� ��� �´�.
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

			// ���� ����
			fp = fopen(file_path, "rb");
			if(fp == NULL){
				MessageBox(hDlg, file_path, "���� ���� ����", MB_ICONERROR);
				perror("���� ����� ����");
				return -1;
			}
			bStart = FALSE; //// �׸���� ���ϰ� ����
			MessageBox(hDlg, file_path, "���� ���� ����", MB_OK);	

			SetEvent(hWriteEvent);

			return TRUE;
		case IDCANCEL: 
			// ���ø����̼� ����
			if(MessageBox(hDlg, "������ �����Ͻðڽ��ϱ�?", "Ȯ��", MB_YESNO) == IDYES)
			{
				EndDialog(hDlg, 0);
			}
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

// ���� ����
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

	HANDLE hThread[MAX_CLIENT + 1]; //// 0 �ε��� ��� ���� fd_set�� ���ϵ�� ���� �Ǵ� �ε����� ���� ������� ������. 0�� listen_sock
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
						MessageBox(NULL, "�� �̻� Ŭ���̾�Ʈ ������ �޾� ���� �� �����ϴ�.", "�˸�", MB_ICONINFORMATION);

						clntLen = sizeof(clntAddr);
						hClntSock = accept(listen_sock, (SOCKADDR*)&clntAddr, &clntLen);

						// �ٷ� FIN�� ������ Ŭ���̾�Ʈ���� �� �̻� ���� �Ұ��� �˸���.
						closesocket(hClntSock);

						continue;
					}

					clntLen = sizeof(clntAddr);
					hClntSock = accept(listen_sock, (SOCKADDR*)&clntAddr, &clntLen);

					clientCnt++;

					if(hClntSock == INVALID_SOCKET)
						err_quit("accept()");

					FD_SET(hClntSock, &reads);

					wsprintf(temp, "Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port));
					MessageBox(NULL, temp, "�˸�", MB_ICONINFORMATION);

					for(i = 0 ; i < reads.fd_count; i++) {
						if(reads.fd_array[i] == hClntSock)
							break;
					}

					// ���� ������ ����
					hThread[i] = CreateThread(NULL, 0, MulticastWriteThread, (LPVOID)hClntSock, 0, &ThreadId[i]);

					if(hThread[i] == NULL){
						MessageBox(NULL, "�����带 ������ �� �����ϴ�.""\r\n���α׷��� �����մϴ�.", "����", MB_ICONERROR);
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
						MessageBox(NULL, "Ŭ���̾�Ʈ�� ������ �������ϴ�", "�˸�", MB_OK);

						CloseHandle(hThread[arrIndex]);
						//�����带 �����ϴ� �迭�鵵 �� �ε����� ������ �Ѵ�.
						for(i = arrIndex; i < clientCnt; i++) {
							hThread[i] = hThread[i + 1];
						}
						clientCnt--;

						if(clientCnt == 0) {
							MessageBox(NULL, "Ŭ���̾�Ʈ�� ��� �������ϴ�.", "�˸�", MB_OK);
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
							DisplayText("[���� �޽���] %s\r\n", chatmsg.buf);

							// ������ ������ ��� Ŭ���̾�Ʈ���� �����͸� ������.
							for(i = 1; i < reads.fd_count; i++) {
								if(i == arrIndex)
									continue;

								// ������ ������
								type = CHATTING;
								retval = send(reads.fd_array[i], (char *)&type, sizeof(type), 0);
								if(retval == SOCKET_ERROR){
									DisplayText("ä�� �޼��� ������ ����\r\n");
									break;
								}
								retval = send(reads.fd_array[i], (char *)&chatmsg.buf, sizeof(g_chatmsg.buf), 0);
								if(retval == SOCKET_ERROR){
									DisplayText("ä�� �޼��� ������ ����\r\n");
									break;
								}
							}
						}
						else if(type == SENDFILE) {
							// ������ ������ ��� Ŭ���̾�Ʈ���� ������ Ÿ�� SENDFILE�� ������.
							for(i = 1; i < reads.fd_count; i++) {
								if(reads.fd_array[i] == reads.fd_array[arrIndex])
									continue;

								type = SENDFILE;
								retval = send(reads.fd_array[i], (char *)&type, sizeof(type), 0);
								if(retval == SOCKET_ERROR){
									break;
								}
							}

							// ���� �̸� �ޱ�
							ZeroMemory(filename, 256);
							retval = recvn(reads.fd_array[arrIndex], filename, 256, 0);
							if(retval == SOCKET_ERROR){
								err_display("recv()");
								closesocket(reads.fd_array[arrIndex]);
								break;
							}
							DisplayText("-> ���� ���� �̸�: %s\r\n", filename);

							// ������ ������ ��� Ŭ���̾�Ʈ���� ���� �̸��� ������.
							for(i = 1; i < reads.fd_count; i++) {
								if(reads.fd_array[i] == reads.fd_array[arrIndex])
									continue;

								// ���� �̸� ������
								retval = send(reads.fd_array[i], filename, 256, 0);
								if(retval == SOCKET_ERROR) err_quit("send()");
							}

							// ���� ũ�� �ޱ�
							retval = recvn(reads.fd_array[arrIndex], (char *)&totalbytes, sizeof(totalbytes), 0);
							if(retval == SOCKET_ERROR){
								err_display("recv()");
								closesocket(reads.fd_array[arrIndex]);
								break;
							}
							DisplayText("-> ���� ���� ũ��: %d\r\n", totalbytes);

							// ������ ������ ��� Ŭ���̾�Ʈ���� ���� ũ�⸦ ������.
							for(i = 1; i < reads.fd_count; i++) {
								if(reads.fd_array[i] == reads.fd_array[arrIndex])
									continue;

								// ���� ũ�� ������
								retval = send(reads.fd_array[i], (char *)&totalbytes, sizeof(totalbytes), 0);
								if(retval == SOCKET_ERROR) err_quit("send()");
							}

							// ���� ����
							FILE *fp = fopen(filename, "wb");
							if(fp == NULL){
								DisplayText("-> ���� ����� ����\r\n");
								perror("���� ����� ����");
								closesocket(reads.fd_array[arrIndex]);
								break;
							}

							// ���� ������ �ޱ�
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
									// ������ ������ ��� Ŭ���̾�Ʈ���� ���� ������ ������.
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
										DisplayText("���� ����� ����\r\n");
										perror("���� ����� ����");
										break;
									}
									left -= received;
								}
							}
							fclose(fp);

							// ���� ��� ���
							if(left == 0)
								DisplayText("-> ���� ���� �Ϸ�!\r\n");
							else
								DisplayText("-> ���� ���� ����!\r\n");

						}
						else if(type == DRAWING){
							retval = recvn(reads.fd_array[arrIndex], (char *)&drawmsg, sizeof(drawmsg), 0);
							if(retval == 0 || retval == SOCKET_ERROR)
								break;

							// ������ ������ ��� Ŭ���̾�Ʈ���� ������ ������ ������.
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

// Ŭ���̾�Ʈ ����
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

	// �б� ������ ����
	hThread[0] = CreateThread(NULL, 0, ReadThread, (LPVOID)sock, 0, &ThreadId[0]);
	// ���� ������ ����
	hThread[1] = CreateThread(NULL, 0, WriteThread, (LPVOID)sock, 0, &ThreadId[1]);
	if(hThread[0] == NULL || hThread[1] == NULL){
		MessageBox(NULL, "�����带 ������ �� �����ϴ�.""\r\n���α׷��� �����մϴ�.", "����", MB_ICONERROR);
		exit(-1);
	}

	bStart = TRUE;
	g_sock = sock;

	// ������ ���� ���
	retval = WaitForMultipleObjects(2, hThread, FALSE, INFINITE);
	retval -= WAIT_OBJECT_0;
	if(retval == 0)
		TerminateThread(hThread[1], -1);
	else
		TerminateThread(hThread[0], -1);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);

	bStart = FALSE;

	MessageBox(NULL, "������ ������ �������ϴ�", "�˸�", MB_ICONINFORMATION);
	EnableWindow(hButtonSend, FALSE);
	EnableWindow(hButtonSendFile, FALSE);

	return 0;
}


DWORD WINAPI ReadThread(LPVOID arg) // ������ �ޱ�
{
	int numtotal = 0;
	char filename[256];
	int totalbytes;
	char file_buf[FILE_BUFSIZE];	////���� ���ۿ� �̿��� ���� 

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
			DisplayText("[���� �޽���] %s\r\n", chatmsg.buf);
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
			// ���� �̸� �ޱ�
			ZeroMemory(filename, 256);
			retval = recvn(sock, filename, 256, 0);
			if(retval == SOCKET_ERROR){
				err_display("recv()");
				closesocket(sock);
				break;
			}
			DisplayText("-> ���� ���� �̸�: %s\r\n", filename);

			// ���� ũ�� �ޱ�
			retval = recvn(sock, (char *)&totalbytes, sizeof(totalbytes), 0);
			if(retval == SOCKET_ERROR){
				err_display("recv()");
				closesocket(sock);
				break;
			}
			DisplayText("-> ���� ���� ũ��: %d\r\n", totalbytes);

			// ���� ����
			FILE *fp = fopen(filename, "wb");
			if(fp == NULL){
				DisplayText("-> ���� ����� ����\r\n");
				perror("���� ����� ����");
				closesocket(sock);
				break;
			}

			// ���� ������ �ޱ�
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
						DisplayText("���� ����� ����\r\n");
						perror("���� ����� ����");
						break;
					}
					left -= received;
				}
			}
			fclose(fp);

			// ���� ��� ���
			if(left == 0)
				DisplayText("-> ���� ���� �Ϸ�!\r\n");
			else
				DisplayText("-> ���� ���� ����!\r\n");

		}
	}

	// closesocket()
	closesocket(sock);

	return 0;
}

// ������ ������
DWORD WINAPI WriteThread(LPVOID arg)
{
	SOCKET sock = (SOCKET)arg;
	int type;
	int retval;

	char filename[256];
	int totalbytes;
	// ���� ������ ���ۿ� ����� ����
	char buf[FILE_BUFSIZE];
	int numread;
	int numtotal = 0;

	// ������ ������ ���
	while(1){
		// ���� �ϷḦ ��ٸ�
		WaitForSingleObject(hWriteEvent, INFINITE);

		switch(CorF) {
		case CHATTING:
			// ���ڿ� ���̰� 0�̸� ������ ����
			if(strlen(g_chatmsg.buf) == 0){
				// ������ ��ư Ȱ��ȭ
				EnableWindow(hButtonSend, TRUE);
				EnableWindow(hButtonSendFile, TRUE);
				// �б� �ϷḦ �˸�
				SetEvent(hReadEvent);
				bStart = TRUE;
				continue;
			}

			// ������ ������
			type = CHATTING;
			retval = send(sock, (char *)&type, sizeof(type), 0);
			if(retval == SOCKET_ERROR){
				break;
			}

			retval = send(sock, (char *)&g_chatmsg.buf, sizeof(g_chatmsg.buf), 0);
			if(retval == SOCKET_ERROR){
				break;
			}
			DisplayText("[���� �޽���] %s\r\n", g_chatmsg.buf);

			bStart = TRUE;
			// ������ ��ư Ȱ��ȭ
			EnableWindow(hButtonSend, TRUE);
			EnableWindow(hButtonSendFile, TRUE);
			// �б� �ϷḦ �˸�
			SetEvent(hReadEvent);
			break;
		case SENDFILE:

			// ������ ������
			type = SENDFILE;
			retval = send(sock, (char *)&type, sizeof(type), 0);
			if(retval == SOCKET_ERROR){
				break;
			}

			// ���� �̸� ������
			ZeroMemory(filename, 256);
			sprintf(filename, strFileTitle);
			retval = send(sock, filename, 256, 0);
			if(retval == SOCKET_ERROR) err_quit("send()");

			// ���� ũ�� ���
			fseek(fp, 0, SEEK_END);
			totalbytes = ftell(fp);

			// ���� ũ�� ������
			retval = send(sock, (char *)&totalbytes, sizeof(totalbytes), 0);
			if(retval == SOCKET_ERROR) err_quit("send()");	


			// ���� ������ ������
			numtotal = 0;
			rewind(fp); // ���� �����͸� ���� ������ �̵�
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
					DisplayText("���� ���� �Ϸ�!: %d ����Ʈ\r\n", numtotal);
					break;
				}
				else {
					DisplayText("���� ����� ����\r\n");
					perror("���� ����� ����");
					break;
				}
			}
			fclose(fp);
			bStart = TRUE;
			// ������ ��ư Ȱ��ȭ
			EnableWindow(hButtonSend, TRUE);
			EnableWindow(hButtonSendFile, TRUE);
			// �ϷḦ �˸�
			SetEvent(hReadEvent);
			break;
		default:
			MessageBox(NULL, "WriteThread() : �� �� ���� ������ Ÿ���Դϴ�.", "Error", MB_ICONERROR);
			break;
		}
	}

	// closesocket()
	closesocket(sock);

	return 0;
}

// ������ �� Ŭ���̾�Ʈ���� ������ ������
DWORD WINAPI MulticastWriteThread(LPVOID arg) {
	SOCKET sock = (SOCKET)arg;
	int type;
	int retval;

	char filename[256];
	int totalbytes;
	// ���� ������ ���ۿ� ����� ����
	char buf[FILE_BUFSIZE];
	int numread;
	int numtotal = 0;

	// ������ ������ ���
	while(1){
		// ���� �ϷḦ ��ٸ�
		WaitForSingleObject(hWriteEvent, INFINITE);

		switch(CorF) {
		case CHATTING:
			// ���ڿ� ���̰� 0�̸� ������ ����
			if(strlen(g_chatmsg.buf) == 0){
				// ������ ��ư Ȱ��ȭ
				EnableWindow(hButtonSend, TRUE);
				EnableWindow(hButtonSendFile, TRUE);
				// �б� �ϷḦ �˸�
				SetEvent(hReadEvent);
				bStart = TRUE;
				continue;
			}

			int i;
			for(i = 1 ; i < reads.fd_count; i++) {
				// ������ ������
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

			DisplayText("[���� �޽���] %s\r\n", g_chatmsg.buf);

			bStart = TRUE;
			// ������ ��ư Ȱ��ȭ
			EnableWindow(hButtonSend, TRUE);
			EnableWindow(hButtonSendFile, TRUE);
			// �б� �ϷḦ �˸�
			SetEvent(hReadEvent);
			break;
		case SENDFILE:

			// ������ ������ ��� Ŭ���̾�Ʈ���� ������ Ÿ�� SENDFILE�� ������.
			for(i = 1; i < reads.fd_count; i++) {
				type = SENDFILE;
				retval = send(reads.fd_array[i], (char *)&type, sizeof(type), 0);
				if(retval == SOCKET_ERROR){
					break;
				}
			}

			// ���� �̸� ������
			ZeroMemory(filename, 256);
			sprintf(filename, strFileTitle);
			// ������ ������ ��� Ŭ���̾�Ʈ���� ���� �̸��� ������.
			for(i = 1; i < reads.fd_count; i++) {
				// ���� �̸� ������
				retval = send(reads.fd_array[i], filename, 256, 0);
				if(retval == SOCKET_ERROR) err_quit("send()");
			}

			// ���� ũ�� ���
			fseek(fp, 0, SEEK_END);
			totalbytes = ftell(fp);
			// ������ ������ ��� Ŭ���̾�Ʈ���� ���� ũ�⸦ ������.
			for(i = 1; i < reads.fd_count; i++) {
				// ���� ũ�� ������
				retval = send(reads.fd_array[i], (char *)&totalbytes, sizeof(totalbytes), 0);
				if(retval == SOCKET_ERROR) err_quit("send()");
			}

			// ���� ������ ������
			numtotal = 0;
			rewind(fp); // ���� �����͸� ���� ������ �̵�
			while(1){
				numread = fread(buf, 1, FILE_BUFSIZE, fp);
				if(numread > 0){
					// ������ ������ ��� Ŭ���̾�Ʈ���� ���� ������ ������.
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
					DisplayText("���� ���� �Ϸ�!: %d ����Ʈ\r\n", numtotal);
					break;
				}
				else {
					DisplayText("���� ����� ����\r\n");
					perror("���� ����� ����");
					break;
				}
			}
			fclose(fp);
			bStart = TRUE;
			// ������ ��ư Ȱ��ȭ
			EnableWindow(hButtonSend, TRUE);
			EnableWindow(hButtonSendFile, TRUE);
			// �ϷḦ �˸�
			SetEvent(hReadEvent);
			break;
		default:
			MessageBox(NULL, "WriteThread() : �� �� ���� ������ Ÿ���Դϴ�.", "Error", MB_ICONERROR);
			break;
		}
	}

	// closesocket()
	closesocket(sock);

	return 0;
}

// ������ �޽��� ó��
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
		// ȭ���� ������ ��Ʈ�� ����
		cx = GetDeviceCaps(hDC, HORZRES);
		cy = GetDeviceCaps(hDC, VERTRES);
		hBitmap = CreateCompatibleBitmap(hDC, cx, cy);
		// �޸� DC ����
		hDCMem = CreateCompatibleDC(hDC);
		// �޸� DC�� ��Ʈ�� ����
		SelectObject(hDCMem, hBitmap);
		// �޸� DC ȭ���� ������� ĥ��
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

			// ȭ�鿡 �׸���
			SelectObject(hDC, GetStockObject(BLACK_PEN));
			MoveToEx(hDC, x0, y0, NULL);
			LineTo(hDC, x1, y1);
			// �޸� ��Ʈ�ʿ� �׸���
			SelectObject(hDCMem, GetStockObject(BLACK_PEN));
			MoveToEx(hDCMem, x0, y0, NULL);
			LineTo(hDCMem, x1, y1);

			if(bStart){
				if(bServer) {
					// ������ ������ ��� Ŭ���̾�Ʈ���� ������ ������ ������.
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
				else { // �� �׸��� ���� ������
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

		// ȭ�鿡 �׸���
		SelectObject(hDC, GetStockObject(BLACK_PEN));
		MoveToEx(hDC, LOWORD(wParam), HIWORD(wParam), NULL);
		LineTo(hDC, LOWORD(lParam), HIWORD(lParam));
		// �޸� ��Ʈ�ʿ� �׸���
		SelectObject(hDCMem, GetStockObject(BLACK_PEN));
		MoveToEx(hDCMem, LOWORD(wParam), HIWORD(wParam), NULL);
		LineTo(hDCMem, LOWORD(lParam), HIWORD(lParam));

		ReleaseDC(hWnd, hDC);
		return 0;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		// �޸� ��Ʈ�ʿ� ����� �׸��� ȭ�鿡 �����ϱ�
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

// ���� �Լ� ���� ��� �� ����
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

// ����Ʈ ��Ʈ�ѿ� ���ڿ� ���
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

// ������ �̸� -> IP �ּ�
BOOL GetIPAddr(char *name, IN_ADDR *addr)
{
	HOSTENT *ptr = gethostbyname(name);
	if(ptr == NULL)
		return FALSE;
	memcpy(addr, ptr->h_addr, ptr->h_length);
	return TRUE;
}

// ����� ���� ������ ���� �Լ�
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

// ���� �Լ� ���� ���
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