#include<iostream>
#include<cstdio>
#include<windows.h>
#include<detours.h>
#include<thread>
#pragma comment(lib, "detours.lib")
using namespace std;
int main(int argc, char* argv[]) {
	//printf("argv:\n");
	//for (int i = 0; i < argc; i++) {
	//	printf("%s\n", argv[i]);
	//}
	if (strstr(argv[0], "syringe")) {
		return 0;
	}
	wchar_t fileName[256] = L"";
	MultiByteToWideChar(CP_ACP, 0, argv[0], strlen(argv[0]), fileName, sizeof(fileName));
	wprintf(L"%s\n", fileName);
	//GetModuleFileName(0, FileName, 256);
	//wprintf(L"%ls\n", FileName);
	//if (wcsstr(FileName, L"syringe.exe")) {
	//	printf("yes\n");
	//}
	//else {
	//	printf("no\n");
	//}
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);
	// 文件夹路径
	WCHAR DirPath[MAX_PATH + 1];
	// ******需要修改部分**********
	wcscpy_s(DirPath, MAX_PATH, L"E:\\record\\6th\\softwareSecurity\\code\\PFDLL\\Debug");	// dll文件夹

	// 文件路径
	char DLLPath[MAX_PATH + 1] = "E:\\record\\6th\\softwareSecurity\\code\\PFDLL\\Debug\\PFDLL.dll"; // dll的地址
	// ******需要修改部分**********
	// 程序路径
	WCHAR EXE[MAX_PATH + 1] = { 0 };
	//wcscpy_s(EXE, MAX_PATH, L"E:\\record\\6th\\softwareSecurity\\CCProxy\\CCProxy.exe"); // MessageBoxA
	//wcscpy_s(EXE, MAX_PATH, L"E:\\record\\6th\\softwareSecurity\\code\\heapCreateAndDestory\\Debug\\heapCreateAndDestory.exe"); // HeapCreate & HeapDestory
	wcscpy_s(EXE, MAX_PATH, fileName); // HeapCreate & HeapDestory


	if (DetourCreateProcessWithDllEx(EXE, NULL, NULL, NULL, TRUE,
		CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED, NULL, DirPath,
		&si, &pi, DLLPath, NULL)) {
		// 恢复线程
		ResumeThread(pi.hThread);

		WaitForSingleObject(pi.hProcess, INFINITE);
	}
	else {
		char error[100];
		sprintf(error, "%d", GetLastError());
	}
	return 0;
}