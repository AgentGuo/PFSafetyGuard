#include<windows.h>
#include<stdio.h>
#include <stdlib.h>
//#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")  //���� ws2_32.dll
//#include <wchar.h>
#define PAGE_SIZE	4096
using namespace std;
void headCreateAndDestory();
void writeFileString();
void readFileString();
void regCreateAndSetValue();
void regOpenAndDelValue();
void showMenu();
void headRepeatedRelease();
void modifyExProgram();
void selfReplication();
void modifyStartupRegistry();
void openAnotherFolder();
void recvData();
void sendData();
void memoryOperation();
int main() {
	int op = 0;
	//MessageBoxA(NULL, "Can not open the file", "Playwav", MB_OK);
	while (1) {
		showMenu();
		scanf("%d", &op);
		switch (op)
		{
			// exit
		case 0: {
			printf("bye!\n");
			break;
		}
		case 1: {
			MessageBoxA(NULL, "I'm MessageBoxA", "I'm MessageBoxA's title", MB_OK);
			break;
		}
		case 2: {
			MessageBoxW(NULL, L"I'm MessageBoxW", L"I'm MessageBoxW's title", MB_OK);
			break;
		}
			  // heap create and heap destory
		case 3: {
			headCreateAndDestory();
			break;
		}
			  // Create and write File
		case 4: {
			writeFileString();
			break;
		}
			  // Create and read File
		case 5: {
			readFileString();
			break;
		}
			  // Create, set value and close reg
		case 6: {
			regCreateAndSetValue();
			break;
		}
			  // open, delete value and close reg
		case 7: {
			regOpenAndDelValue();
			break;
		}
		case 8: {
			recvData();
			break;
		}
		case 9: {
			sendData();
			break;
		}
		case 10: {
			headRepeatedRelease();
			break;
		}
		case 11: {
			modifyExProgram();
			break;
		}
		case 12: {
			selfReplication();
			break;
		}
		case 13: {
			modifyStartupRegistry();
			break;
		}
		case 14: {
			openAnotherFolder();
			break;
		}
		case 15: {
			memoryOperation();
			break;
		}
		}
		// exit
		if (op == 0) {
			break;
		}
	}
	return 0;
}
void showMenu() {
	//printf("\n*************************************************************************************\n");
	printf("--------------------------------please select an option--------------------------------\n");
	printf("--Normal operation--:\n");
	printf("1.MessageBoxA      2.MessageBoxW            3.headCreateAndDestory   4.writeFileString\n");
	printf("5.readFileString   6.regCreateAndSetValue   7.regOpenAndDelValue     8.socketRecvData\n");
	printf("9.socketSendData\n");
	printf("--Malicious operation--:\n");
	printf("10.headRepeatedRelease   11.Modifying executable program   12.selfReplication\n");
	printf("13.modifyStartupRegistry   14.openAnotherFolder\n");
}
void headCreateAndDestory() {

	printf("Press any key to start HeapCreate!\n");
	getchar();
	HANDLE hHeap = HeapCreate(HEAP_NO_SERIALIZE, PAGE_SIZE * 10, PAGE_SIZE * 100);

	int* pArr = (int*)HeapAlloc(hHeap, 0, sizeof(int) * 30);
	for (int i = 0; i < 30; ++i)
	{
		pArr[i] = i + 1;
	}
	printf("Successfully created!\n");
	for (int i = 0; i < 30; ++i)
	{
		if (i % 5 == 0)
			printf_s("\n");
		printf("%3d ", pArr[i]);
	}
	printf_s("\n\n");
	printf("Press any key to start HeapFree!\n");
	getchar();
	HeapFree(hHeap, 0, pArr);
	printf("Press any key to start HeapDestory!\n");
	getchar();
	HeapDestroy(hHeap);

	printf("Successfully destory!\n");
}
void writeFileString()
{
	CHAR* pBuffer;
	int fileSize = 0;
	char writeString[100];
	bool flag;
	HANDLE hOpenFile = (HANDLE)CreateFile(L"a.txt", GENERIC_READ| GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);
	if (hOpenFile == INVALID_HANDLE_VALUE)
	{
		hOpenFile = NULL;
		printf("Can not open the file\n");
		return;
		//MessageBoxA(NULL, "Can not open the file", "Playwav", MB_OK);
	}
	printf("successfully open a file\n");
	printf("input a string:");
	scanf("%s", writeString);
	flag = WriteFile(hOpenFile, writeString, strlen(writeString), NULL, NULL);
	if (flag) {
		printf("successful writed!\n");
	}
	FlushFileBuffers(hOpenFile);
	CloseHandle(hOpenFile);
}
void readFileString() {
	CHAR* pBuffer;
	int fileSize = 0;
	bool flag;
	HANDLE hOpenFile = (HANDLE)CreateFile(L"a.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
	if (hOpenFile == INVALID_HANDLE_VALUE)
	{
		hOpenFile = NULL;
		printf("Can not open the file\n");
		return;
	}
	printf("successfully open a file\n");
	fileSize = GetFileSize(hOpenFile, NULL);
	pBuffer = (char*)malloc((fileSize + 1) * sizeof(char));
	flag = ReadFile(hOpenFile, pBuffer, fileSize, NULL, NULL);
	pBuffer[fileSize] = 0;
	if (flag) {
		printf("successfully read a string:%s!\n", pBuffer);
	}
	free(pBuffer);
	CloseHandle(hOpenFile);
}
void regCreateAndSetValue() {
// ����ע������ü�ֵ
	HKEY hKey = NULL;
	TCHAR Data[254];
	memset(Data, 0, sizeof(Data));
	wcsncpy_s(Data, TEXT("https://github.com/AgentGuo"), 254);

	size_t lRet = RegCreateKeyEx(HKEY_CURRENT_USER, (LPWSTR)L"aaaMykey", 0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS, NULL, &hKey, NULL);
	if (lRet == ERROR_SUCCESS) {
		printf("create successfully!\n");
	}
	else {
		printf("failed to create!\n");
	}
	// �޸�ע����ֵ��û���򴴽���
	size_t iLen = wcslen(Data);
	// ���ü�ֵ
	lRet = RegSetValueEx(hKey, L"panfeng", 0, REG_SZ, (CONST BYTE*)Data, sizeof(TCHAR) * iLen);
	if (lRet == ERROR_SUCCESS)
	{
		printf("set value successfully!\n");
		return;
	}
	else {
		printf("failed to set value!\n");
	}
	RegCloseKey(hKey);
}
void regOpenAndDelValue() {
	HKEY hKey = NULL;
	size_t lRet = RegOpenKeyEx(HKEY_CURRENT_USER, (LPWSTR)L"aaaMykey", 0, KEY_ALL_ACCESS, &hKey);
	if (lRet == ERROR_SUCCESS) {
		printf("open successfully!\n");
	}
	else {
		printf("open failed\n");
	}
	lRet = RegDeleteValue(hKey, L"panfeng");
	if (lRet == ERROR_SUCCESS) {
		printf("delete success!\n");
	}
	else {
		printf("delete fail!\n");
	}
	RegCloseKey(hKey);
}
void headRepeatedRelease() {

	printf("Press any key to start HeapCreate!\n");
	getchar();
	HANDLE hHeap = HeapCreate(HEAP_NO_SERIALIZE, PAGE_SIZE * 10, PAGE_SIZE * 100);

	int* pArr = (int*)HeapAlloc(hHeap, 0, sizeof(int) * 30);
	for (int i = 0; i < 30; ++i)
	{
		pArr[i] = i + 1;
	}
	printf("Successfully created!\n");
	for (int i = 0; i < 30; ++i)
	{
		if (i % 5 == 0)
			printf_s("\n");
		printf("%3d ", pArr[i]);
	}
	printf_s("\n\n");
	printf("Press any key to start the first HeapFree!\n");
	getchar();
	HeapFree(hHeap, 0, pArr);
	printf("Press any key to start the second HeapFree!\n");
	getchar();
	HeapFree(hHeap, 0, pArr);
	printf("Press any key to destroy the heap!\n");
	getchar();
	HeapDestroy(hHeap);
}
void modifyExProgram() {
	HANDLE hOpenFile = (HANDLE)CreateFile(L"a.exe", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);
	CloseHandle(hOpenFile);
}
void selfReplication() {
	//testCode.exe
	HANDLE hOpenFile = (HANDLE)CreateFile(L"testCode.exe", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
	CloseHandle(hOpenFile);
}
void modifyStartupRegistry() {
	HKEY hKey = NULL;
	size_t lRet = RegOpenKeyEx(HKEY_CURRENT_USER, (LPWSTR)L"SOFTWARE\Microsoft\Windows\CurrentVersion\Run", 0, KEY_READ, &hKey);
	if (lRet == ERROR_SUCCESS) {
		printf("open successfully!\n");
	}
	else {
		printf("open failed\n");
	}
	RegCloseKey(hKey);
}
void openAnotherFolder() {
	HANDLE hOpenFile = (HANDLE)CreateFile(L".\\testFolder\\a.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);
	CloseHandle(hOpenFile);
}
void recvData() {
	//��ʼ��DLL
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//�����׽���
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	//���������������
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));  //ÿ���ֽڶ���0���
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockAddr.sin_port = htons(1234);
	connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
	Sleep(500);
	//���շ��������ص�����
	char szBuffer[MAXBYTE] = { 0 };
	recv(sock, szBuffer, MAXBYTE, NULL);
	//������յ�������
	printf("Message form server: %s\n", szBuffer);
	//�ر��׽���
	closesocket(sock);
	//��ֹʹ�� DLL
	WSACleanup();
}
void sendData() {
	//��ʼ�� DLL
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//�����׽���
	SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	//���׽���
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));  //ÿ���ֽڶ���0���
	sockAddr.sin_family = PF_INET;  //ʹ��IPv4��ַ
	sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //�����IP��ַ
	sockAddr.sin_port = htons(1234);  //�˿�
	bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
	//�������״̬
	listen(servSock, 20);
	//���տͻ�������
	SOCKADDR clntAddr;
	int nSize = sizeof(SOCKADDR);
	SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
	//��ͻ��˷�������
	char str[32] = "Hello World!";
	send(clntSock, str, strlen(str) + sizeof(char), NULL);
	//�ر��׽���
	closesocket(clntSock);
	closesocket(servSock);
	//��ֹ DLL ��ʹ��
	WSACleanup();
}
void memoryOperation() {
	getchar();
	char temp[100] = "";
	printf("press any key to copy memory\n");
	getchar();
	//memccpy(temp, "hello\n", 6);
	memcpy(temp, "hello\n", 6);
	printf("%s", temp);
	printf("press any key to move memory\n");
	getchar();
	memmove(temp, "world\n", 6);
	printf("%s", temp);
}