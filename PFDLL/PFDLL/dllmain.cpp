// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "framework.h"
#include "detours.h"
#include "stdio.h"
#include "stdarg.h"
#include "windows.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unordered_map>
#include <WinSock2.h>
#pragma comment(lib, "detours.lib")
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
#define MESSAGEBOXA 1
#define MESSAGEBOXW 2
#define CREATEFILE 3
#define WRITEFILE 4
#define READFILE 5
#define HEAPCREATE 6
#define HEAPDESTORY 7
#define HEAPFREE 8
#define REGCREATEKEYEX 9
#define REGSETVALUEEX 10
#define REGCLOSEKEY 11
#define REGOPENKEYEX 12
#define REGDELETEVALUE 13
#define THESOCKET 14
#define BIND 15
#define SEND 16
#define CONNECT 17
#define RECV 18
using namespace std;
SYSTEMTIME st;

struct info {
	int type, argNum;
	SYSTEMTIME st;
	char argName[10][30] = {0};
	char argValue[10][70] = { 0 };
};

info sendInfo;
HANDLE hSemaphore = OpenSemaphore(EVENT_ALL_ACCESS, FALSE, L"mySemaphore");
HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, NULL, L"ShareMemory");
LPVOID lpBase = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(info));
//message_queue mq(open_only, "my_message_queue");

// 第一和引入需要hool的函数和替换的函数
static int (WINAPI* OldMessageBoxW)(_In_opt_ HWND hWnd, _In_opt_ LPCWSTR lpText, _In_opt_ LPCWSTR lpCaption, _In_ UINT uType) = MessageBoxW;
// 定义需要hook的函数
static int (WINAPI* OldMessageBoxA)(_In_opt_ HWND hWnd, _In_opt_ LPCSTR lpText, _In_opt_ LPCSTR lpCaption, _In_ UINT uType) = MessageBoxA;
// 定义需要替换的新的函数
extern "C" __declspec(dllexport) int WINAPI NewMessageBoxA(_In_opt_ HWND hWnd, _In_opt_ LPCSTR lpText, _In_opt_ LPCSTR lpCaption, _In_ UINT uType)
{
	// 向sendInfo中写入数据
	sendInfo.type = MESSAGEBOXA;
	GetLocalTime(&(sendInfo.st));
	sendInfo.argNum = 4;
	// 参数名
	sprintf(sendInfo.argName[0], "hWnd");
	sprintf(sendInfo.argName[1], "lpText");
	sprintf(sendInfo.argName[2], "lpCaption");
	sprintf(sendInfo.argName[3], "uType");
	// 参数值
	sprintf(sendInfo.argValue[0], "%08X", hWnd);
	sprintf(sendInfo.argValue[1], "%s", lpText);
	sprintf(sendInfo.argValue[2], "%s", lpCaption);
	sprintf(sendInfo.argValue[3], "%08X", uType);
	// 将sendinfo赋值到共享内存
	memcpy(lpBase, &sendInfo, sizeof(info));
	// 进行V操作，使得信号量+1
	ReleaseSemaphore(hSemaphore, 1, NULL);
	sendInfo.argNum = 0;
	// 返回原始接口
	return OldMessageBoxA(hWnd, lpText, lpCaption, uType);
}

extern "C" __declspec(dllexport) int WINAPI NewMessageBoxW(_In_opt_ HWND hWnd, _In_opt_ LPCWSTR lpText, _In_opt_ LPCWSTR lpCaption, _In_ UINT uType)
{
	char temp[70];
	sendInfo.type = MESSAGEBOXW;
	GetLocalTime(&(sendInfo.st));

	sendInfo.argNum = 4;
	// 参数名
	sprintf(sendInfo.argName[0], "hWnd");
	sprintf(sendInfo.argName[1], "lpText");
	sprintf(sendInfo.argName[2], "lpCaption");
	sprintf(sendInfo.argName[3], "uType");
	// 参数值
	sprintf(sendInfo.argValue[0], "%08X", hWnd);
	// 宽字节转char
	memset(temp, 0, sizeof(temp));
	WideCharToMultiByte(CP_ACP, 0, lpText, wcslen(lpText), temp, sizeof(temp), NULL, NULL);
	strcpy(sendInfo.argValue[1], temp);
	// 宽字节转char
	memset(temp, 0, sizeof(temp));
	WideCharToMultiByte(CP_ACP, 0, lpCaption, wcslen(lpCaption), temp, sizeof(temp), NULL, NULL);
	strcpy(sendInfo.argValue[2], temp);

	sprintf(sendInfo.argValue[3], "%08X", uType);

	memcpy(lpBase, &sendInfo, sizeof(info));
	ReleaseSemaphore(hSemaphore, 1, NULL);

	sendInfo.argNum = 0;
	return OldMessageBoxW(hWnd, lpText, lpCaption, uType);
}

// 打开文件

static HANDLE(WINAPI* OldCreateFile)(
	LPCWSTR               lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
	) = CreateFile;

extern "C" __declspec(dllexport)HANDLE WINAPI NewCreateFile(
	LPCWSTR               lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
)
{
	HANDLE hFile = OldCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	if (GetFileType(hFile) == FILE_TYPE_DISK) {
		char temp[70];
		sendInfo.type = CREATEFILE;
		GetLocalTime(&(sendInfo.st));

		sendInfo.argNum = 7;
		// 参数名
		sprintf(sendInfo.argName[0], "lpFileName");
		sprintf(sendInfo.argName[1], "dwDesiredAccess");
		sprintf(sendInfo.argName[2], "dwShareMode");
		sprintf(sendInfo.argName[3], "lpSecurityAttributes");
		sprintf(sendInfo.argName[4], "dwCreationDisposition");
		sprintf(sendInfo.argName[5], "dwFlagsAndAttributes");
		sprintf(sendInfo.argName[6], "hTemplateFile");
		// 参数值
		// 宽字节转char
		memset(temp, 0, sizeof(temp));
		WideCharToMultiByte(CP_ACP, 0, lpFileName, wcslen(lpFileName), temp, sizeof(temp), NULL, NULL);
		strcpy(sendInfo.argValue[0], temp);
		sprintf(sendInfo.argValue[1], "%08X", dwDesiredAccess);
		sprintf(sendInfo.argValue[2], "%08X", dwShareMode);
		sprintf(sendInfo.argValue[3], "%08X", lpSecurityAttributes);
		sprintf(sendInfo.argValue[4], "%08X", dwCreationDisposition);
		sprintf(sendInfo.argValue[5], "%08X", dwFlagsAndAttributes);
		sprintf(sendInfo.argValue[6], "%08X", hTemplateFile);

		memcpy(lpBase, &sendInfo, sizeof(info));
		ReleaseSemaphore(hSemaphore, 1, NULL);
	}
	return hFile;
	//return OldCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

// 写文件

static BOOL(WINAPI* OldWriteFile)(
	HANDLE       hFile,
	LPCVOID      lpBuffer,
	DWORD        nNumberOfBytesToWrite,
	LPDWORD      lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped
	) = WriteFile;

extern "C" __declspec(dllexport)BOOL WINAPI NewWriteFile(
	HANDLE       hFile,
	LPCVOID      lpBuffer,
	DWORD        nNumberOfBytesToWrite,
	LPDWORD      lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped
)
{
	if (GetFileType(hFile) == FILE_TYPE_DISK) {
		sendInfo.argNum = 5;
		// 参数名
		sprintf(sendInfo.argName[0], "hFile");
		sprintf(sendInfo.argName[1], "lpBuffer");
		sprintf(sendInfo.argName[2], "nNumberOfBytesToWrite");
		sprintf(sendInfo.argName[3], "lpNumberOfBytesWritten");
		sprintf(sendInfo.argName[4], "lpOverlapped");
		// 参数值
		sprintf(sendInfo.argValue[0], "%08X", hFile);
		sprintf(sendInfo.argValue[1], "%08X", lpBuffer);
		sprintf(sendInfo.argValue[2], "%08X", nNumberOfBytesToWrite);
		sprintf(sendInfo.argValue[3], "%08X", lpNumberOfBytesWritten);
		sprintf(sendInfo.argValue[4], "%08X", lpOverlapped);

		sendInfo.type = WRITEFILE;
		GetLocalTime(&(sendInfo.st));
		memcpy(lpBase, &sendInfo, sizeof(info));
		ReleaseSemaphore(hSemaphore, 1, NULL);
	}
	return OldWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

static BOOL(WINAPI* OldReadFile)(
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped
	) = ReadFile;

extern "C" __declspec(dllexport)BOOL WINAPI NewReadFile(
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped
)
{
	if (GetFileType(hFile) == FILE_TYPE_DISK) {
		sendInfo.argNum = 5;
		// 参数名
		sprintf(sendInfo.argName[0], "hFile");
		sprintf(sendInfo.argName[1], "lpBuffer");
		sprintf(sendInfo.argName[2], "nNumberOfBytesToRead");
		sprintf(sendInfo.argName[3], "lpNumberOfBytesRead");
		sprintf(sendInfo.argName[4], "lpOverlapped");
		// 参数值
		sprintf(sendInfo.argValue[0], "%08X", hFile);
		sprintf(sendInfo.argValue[1], "%08X", lpBuffer);
		sprintf(sendInfo.argValue[2], "%08X", nNumberOfBytesToRead);
		sprintf(sendInfo.argValue[3], "%08X", lpNumberOfBytesRead);
		sprintf(sendInfo.argValue[4], "%08X", lpOverlapped);
		sendInfo.type = READFILE;
		GetLocalTime(&(sendInfo.st));
		memcpy(lpBase, &sendInfo, sizeof(info));
		ReleaseSemaphore(hSemaphore, 1, NULL);
	}
	return OldReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}

// 堆操作
static HANDLE(WINAPI* OldHeapCreate)(DWORD fIOoptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize) = HeapCreate;

extern "C" __declspec(dllexport)HANDLE WINAPI NewHeapCreate(DWORD fIOoptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize)
{
	HANDLE hFile = OldHeapCreate(fIOoptions, dwInitialSize, dwMaximumSize);
	sendInfo.argNum = 4;
	// 参数名
	sprintf(sendInfo.argName[0], "fIOoptions");
	sprintf(sendInfo.argName[1], "dwInitialSize");
	sprintf(sendInfo.argName[2], "dwMaximumSize");
	sprintf(sendInfo.argName[3], "HANDLE");
	// 参数值
	sprintf(sendInfo.argValue[0], "%08X", fIOoptions);
	sprintf(sendInfo.argValue[1], "%08X", dwInitialSize);
	sprintf(sendInfo.argValue[2], "%08X", dwMaximumSize);
	sprintf(sendInfo.argValue[3], "%08X", hFile);
	sendInfo.type = HEAPCREATE;
	GetLocalTime(&(sendInfo.st));
	memcpy(lpBase, &sendInfo, sizeof(info));
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return hFile;
}

static BOOL(WINAPI* OldHeapDestory)(HANDLE) = HeapDestroy;
extern "C" __declspec(dllexport) BOOL WINAPI NewHeapDestory(HANDLE hHeap)
{
	sendInfo.argNum = 1;
	// 参数名
	sprintf(sendInfo.argName[0], "hHeap");
	// 参数值
	sprintf(sendInfo.argValue[0], "%08X", hHeap);
	sendInfo.type = HEAPDESTORY;
	GetLocalTime(&(sendInfo.st));
	memcpy(lpBase, &sendInfo, sizeof(info));
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldHeapDestory(hHeap);
}

static BOOL(WINAPI* OldHeapFree)(HANDLE hHeap, DWORD dwFlags, _Frees_ptr_opt_ LPVOID lpMem) = HeapFree;
extern "C" __declspec(dllexport) BOOL WINAPI NewHeapFree(HANDLE hHeap, DWORD dwFlags, _Frees_ptr_opt_ LPVOID lpMem) {
	sendInfo.argNum = 3;
	// 参数名
	sprintf(sendInfo.argName[0], "hHeap");
	sprintf(sendInfo.argName[1], "dwFlags");
	sprintf(sendInfo.argName[2], "lpMem");
	// 参数值
	sprintf(sendInfo.argValue[0], "%08X", hHeap);
	sprintf(sendInfo.argValue[1], "%08X", dwFlags);
	sprintf(sendInfo.argValue[2], "%08X", lpMem);
	sendInfo.type = HEAPFREE;
	GetLocalTime(&(sendInfo.st));
	memcpy(lpBase, &sendInfo, sizeof(info));
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldHeapFree(hHeap, dwFlags, lpMem);
}

static LSTATUS(WINAPI* OldRegCreateKeyEx)(
	HKEY                        hKey,
	LPCWSTR                     lpSubKey,
	DWORD                       Reserved,
	LPWSTR                      lpClass,
	DWORD                       dwOptions,
	REGSAM                      samDesired,
	const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	PHKEY                       phkResult,
	LPDWORD                     lpdwDisposition) = RegCreateKeyEx;

extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegCreateKeyEx(
	HKEY                        hKey,
	LPCWSTR                     lpSubKey,
	DWORD                       Reserved,
	LPWSTR                      lpClass,
	DWORD                       dwOptions,
	REGSAM                      samDesired,
	const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	PHKEY                       phkResult,
	LPDWORD                     lpdwDisposition
) {
	char temp[70];
	sendInfo.argNum = 9;
	// 参数名
	sprintf(sendInfo.argName[0], "hKey");
	sprintf(sendInfo.argName[1], "lpSubKey");
	sprintf(sendInfo.argName[2], "Reserved");
	sprintf(sendInfo.argName[3], "lpClass");
	sprintf(sendInfo.argName[4], "dwOptions");
	sprintf(sendInfo.argName[5], "samDesired");
	sprintf(sendInfo.argName[6], "lpSecurityAttributes");
	sprintf(sendInfo.argName[7], "phkResult");
	sprintf(sendInfo.argName[8], "lpdwDisposition");
	// 参数值
	sprintf(sendInfo.argValue[0], "%08X", hKey);
	// 宽字节转char
	memset(temp, 0, sizeof(temp));
	WideCharToMultiByte(CP_ACP, 0, lpSubKey, wcslen(lpSubKey), temp, sizeof(temp), NULL, NULL);
	strcpy(sendInfo.argValue[1], temp);
	sprintf(sendInfo.argValue[2], "%08X", Reserved);
	sprintf(sendInfo.argValue[3], "%08X", lpClass);
	sprintf(sendInfo.argValue[4], "%08X", dwOptions);
	sprintf(sendInfo.argValue[5], "%08X", samDesired);
	sprintf(sendInfo.argValue[6], "%08X", lpSecurityAttributes);
	sprintf(sendInfo.argValue[7], "%08X", phkResult);
	sprintf(sendInfo.argValue[8], "%08X", lpdwDisposition);

	sendInfo.type = REGCREATEKEYEX;
	GetLocalTime(&(sendInfo.st));
	memcpy(lpBase, &sendInfo, sizeof(info));
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldRegCreateKeyEx(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
}

static LSTATUS(WINAPI* OldRegSetValueEx)(
	HKEY       hKey,
	LPCWSTR    lpValueName,
	DWORD      Reserved,
	DWORD      dwType,
	const BYTE* lpData,
	DWORD      cbData
	) = RegSetValueEx;

extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegSetValueEx(
	HKEY       hKey,
	LPCWSTR    lpValueName,
	DWORD      Reserved,
	DWORD      dwType,
	const BYTE * lpData,
	DWORD      cbData)
{
	char temp[70];
	sendInfo.argNum = 6;
	// 参数名
	sprintf(sendInfo.argName[0], "hKey");
	sprintf(sendInfo.argName[1], "lpValueName");
	sprintf(sendInfo.argName[2], "Reserved");
	sprintf(sendInfo.argName[3], "dwType");
	sprintf(sendInfo.argName[4], "lpData");
	sprintf(sendInfo.argName[5], "cbData");
	// 参数值
	sprintf(sendInfo.argValue[0], "%08X", hKey);
	// 宽字节转char
	memset(temp, 0, sizeof(temp));
	WideCharToMultiByte(CP_ACP, 0, lpValueName, wcslen(lpValueName), temp, sizeof(temp), NULL, NULL);
	strcpy(sendInfo.argValue[1], temp);
	sprintf(sendInfo.argValue[2], "%08X", Reserved);
	sprintf(sendInfo.argValue[3], "%08X", dwType);
	memset(temp, 0, sizeof(temp));
	WideCharToMultiByte(CP_ACP, 0, (LPCWCH)lpData, wcslen((LPCWCH)lpData), temp, sizeof(temp), NULL, NULL);
	strcpy(sendInfo.argValue[4], temp);
	//strcpy(sendInfo.argValue[4], (const char *)lpData);
	//sprintf(sendInfo.argValue[4], "%08X", lpData);
	sprintf(sendInfo.argValue[5], "%08X", cbData);


	sendInfo.type = REGSETVALUEEX;
	GetLocalTime(&(sendInfo.st));
	memcpy(lpBase, &sendInfo, sizeof(info));
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldRegSetValueEx(hKey, lpValueName, Reserved, dwType, lpData, cbData);
}

static LSTATUS(WINAPI* OldRegCloseKey)(HKEY hKey) = RegCloseKey;

extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegCloseKey(HKEY hKey)
{
	sendInfo.argNum = 1;
	// 参数名
	sprintf(sendInfo.argName[0], "hKey");
	// 参数值
	sprintf(sendInfo.argValue[0], "%08X", hKey);
	sendInfo.type = REGCLOSEKEY;
	GetLocalTime(&(sendInfo.st));
	memcpy(lpBase, &sendInfo, sizeof(info));
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldRegCloseKey(hKey);
}

static LSTATUS (WINAPI* OldRegOpenKeyEx)(
	HKEY    hKey,
	LPCWSTR lpSubKey,
	DWORD   ulOptions,
	REGSAM  samDesired,
	PHKEY   phkResult
	) = RegOpenKeyEx;
extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegOpenKeyEx(
	HKEY    hKey,
	LPCWSTR lpSubKey,
	DWORD   ulOptions,
	REGSAM  samDesired,
	PHKEY   phkResult)
{
	char temp[70];
	sendInfo.argNum = 5;
	// 参数名
	sprintf(sendInfo.argName[0], "hKey");
	sprintf(sendInfo.argName[1], "lpSubKey");
	sprintf(sendInfo.argName[2], "ulOptions");
	sprintf(sendInfo.argName[3], "samDesired");
	sprintf(sendInfo.argName[4], "phkResult");
	// 参数值
	sprintf(sendInfo.argValue[0], "%08X", hKey);
	// 宽字节转char
	memset(temp, 0, sizeof(temp));
	WideCharToMultiByte(CP_ACP, 0, lpSubKey, wcslen(lpSubKey), temp, sizeof(temp), NULL, NULL);
	strcpy(sendInfo.argValue[1], temp);
	sprintf(sendInfo.argValue[2], "%08X", ulOptions);
	sprintf(sendInfo.argValue[3], "%08X", samDesired);
	sprintf(sendInfo.argValue[4], "%08X", phkResult);

	sendInfo.type = REGOPENKEYEX;
	GetLocalTime(&(sendInfo.st));
	memcpy(lpBase, &sendInfo, sizeof(info));
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldRegOpenKeyEx(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

static LSTATUS(WINAPI* OldRegDeleteValue)(
	HKEY    hKey,
	LPCWSTR lpValueName
	) = RegDeleteValue;

extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegDeleteValue(
	HKEY    hKey,
	LPCWSTR lpValueName)
{
	char temp[70];
	sendInfo.argNum = 2;
	// 参数名
	sprintf(sendInfo.argName[0], "hKey");
	sprintf(sendInfo.argName[1], "lpValueName");
	// 参数值
	sprintf(sendInfo.argValue[0], "%08X", hKey);
	// 宽字节转char
	memset(temp, 0, sizeof(temp));
	WideCharToMultiByte(CP_ACP, 0, lpValueName, wcslen(lpValueName), temp, sizeof(temp), NULL, NULL);
	strcpy(sendInfo.argValue[1], temp);
	sendInfo.type = REGDELETEVALUE;
	GetLocalTime(&(sendInfo.st));
	memcpy(lpBase, &sendInfo, sizeof(info));
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldRegDeleteValue(hKey, lpValueName);
}


static SOCKET(WINAPI* Oldsocket)(
	int af,
	int type,
	int protocol
	) = socket;
extern "C" __declspec(dllexport)SOCKET WINAPI Newsocket(int af, int type, int protocol) {
	sendInfo.argNum = 3;
	// 参数名
	sprintf(sendInfo.argName[0], "af");
	sprintf(sendInfo.argName[1], "type");
	sprintf(sendInfo.argName[2], "protocol");
	// 参数值
	sprintf(sendInfo.argValue[0], "%08X", af);
	sprintf(sendInfo.argValue[1], "%08X", type);
	sprintf(sendInfo.argValue[2], "%08X", protocol);
	sendInfo.type = THESOCKET;
	GetLocalTime(&(sendInfo.st));
	memcpy(lpBase, &sendInfo, sizeof(info));
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return Oldsocket(af, type, protocol);
}

static int (WINAPI* Oldbind)(
	SOCKET         s,
	const sockaddr* name,
	int            namelen
	) = bind;
extern "C" __declspec(dllexport)int WINAPI Newbind(SOCKET s, const sockaddr * name,	int namelen) {
	//name->sa_data
	sendInfo.argNum = 5;
	// 参数名
	sprintf(sendInfo.argName[0], "s");
	sprintf(sendInfo.argName[1], "name");
	sprintf(sendInfo.argName[2], "namelen");
	sprintf(sendInfo.argName[3], "IP");
	sprintf(sendInfo.argName[4], "port");
	// 参数值
	sprintf(sendInfo.argValue[0], "%08X", s);
	sprintf(sendInfo.argValue[1], "%08X", name);
	sprintf(sendInfo.argValue[2], "%08X", namelen);
	struct sockaddr_in* sock = (struct sockaddr_in*)name;
	int port = ntohs(sock->sin_port);
	sprintf(sendInfo.argValue[3], "%s", inet_ntoa(sock->sin_addr));
	sprintf(sendInfo.argValue[4], "%d", port);
	sendInfo.type = BIND;
	GetLocalTime(&(sendInfo.st));
	memcpy(lpBase, &sendInfo, sizeof(info));
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return Oldbind(s, name, namelen);
}

static int (WINAPI* Oldsend)(
	SOCKET     s,
	const char* buf,
	int        len,
	int        flags
	) = send;
extern "C" __declspec(dllexport)int WINAPI Newsend(SOCKET s, const char* buf, int len, int flags) {
	sendInfo.argNum = 5;
	// 参数名
	sprintf(sendInfo.argName[0], "s");
	sprintf(sendInfo.argName[1], "buf");
	sprintf(sendInfo.argName[2], "len");
	sprintf(sendInfo.argName[3], "flags");
	sprintf(sendInfo.argName[4], "data");
	// 参数值
	sprintf(sendInfo.argValue[0], "%08X", s);
	sprintf(sendInfo.argValue[1], "%08X", buf);
	sprintf(sendInfo.argValue[2], "%08X", len);
	sprintf(sendInfo.argValue[3], "%08X", flags);
	sprintf(sendInfo.argValue[4], "%s", buf);
	sendInfo.type = SEND;
	GetLocalTime(&(sendInfo.st));
	memcpy(lpBase, &sendInfo, sizeof(info));
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return Oldsend(s, buf, len, flags);
}

static int (WINAPI* Oldconnect)(
	SOCKET         s,
	const sockaddr* name,
	int            namelen
	) = connect;
extern "C" __declspec(dllexport)int WINAPI Newconnect(SOCKET s, const sockaddr * name, int namelen) {
	//name->sa_data
	sendInfo.argNum = 5;
	// 参数名
	sprintf(sendInfo.argName[0], "s");
	sprintf(sendInfo.argName[1], "name");
	sprintf(sendInfo.argName[2], "namelen");
	sprintf(sendInfo.argName[3], "IP");
	sprintf(sendInfo.argName[4], "port");
	// 参数值
	sprintf(sendInfo.argValue[0], "%08X", s);
	sprintf(sendInfo.argValue[1], "%08X", name);
	sprintf(sendInfo.argValue[2], "%08X", namelen);
	struct sockaddr_in* sock = (struct sockaddr_in*)name;
	int port = ntohs(sock->sin_port);
	sprintf(sendInfo.argValue[3], "%s", inet_ntoa(sock->sin_addr));
	sprintf(sendInfo.argValue[4], "%d", port);
	sendInfo.type = CONNECT;
	GetLocalTime(&(sendInfo.st));
	memcpy(lpBase, &sendInfo, sizeof(info));
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return Oldconnect(s, name, namelen);
}

static void* (__cdecl* Oldmemcpy)(void* _Dst, const void* _Src, size_t _Size) = memcpy;
extern "C" __declspec(dllexport)void* __cdecl Newmemcpy(void* _Dst, const void* _Src, size_t _Size) {
	printf("memcpy hooked\n");
	return Oldmemcpy(_Dst, _Src, _Size);
}
//void* __cdecl memcpy(void* _Dst, const void* _Src, size_t _Size)

//static void (WINAPI* OldRtlCopyMemory)(
//	void* Destination,
//	const void* Source,
//	size_t      Length	
//	) = RtlCopyMemory;


static int (WINAPI* Oldrecv)(
	SOCKET     s,
	char* buf,
	int        len,
	int        flags
	) = recv;
extern "C" __declspec(dllexport)int WINAPI Newrecv(SOCKET s, char* buf, int len, int flags) {
	int i;
	sendInfo.argNum = 4;
	// 参数名
	sprintf(sendInfo.argName[0], "s");
	sprintf(sendInfo.argName[1], "buf");
	sprintf(sendInfo.argName[2], "len");
	sprintf(sendInfo.argName[3], "flags");
	// 参数值
	sprintf(sendInfo.argValue[0], "%08X", s);
	sprintf(sendInfo.argValue[1], "%08X", buf);
	sprintf(sendInfo.argValue[2], "%08X", len);
	sprintf(sendInfo.argValue[3], "%08X", flags);
	sendInfo.type = RECV;
	GetLocalTime(&(sendInfo.st));
	memcpy(lpBase, &sendInfo, sizeof(info));
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return Oldrecv(s, buf, len, flags);
}

BOOL WINAPI DllMain(HMODULE hModule,
	DWORD ul_reason_for_call,
	LPVOID lpReserved
)
{
	//wchar_t FileName[256];
	//GetModuleFileName(0, (LPWSTR)FileName, 256);
	//printf("文件名：%c%c%c%c：文件名", FileName[0], FileName[1], FileName[2], FileName[3]);
	//if (wcsstr(FileName, L"syringe.exe")) {
	//	return true;
	//}
	//wprintf(L"%ls\n", FileName);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		DisableThreadLibraryCalls(hModule);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)OldMessageBoxW, NewMessageBoxW);
		DetourAttach(&(PVOID&)OldMessageBoxA, NewMessageBoxA);
		DetourAttach(&(PVOID&)OldCreateFile, NewCreateFile);
		DetourAttach(&(PVOID&)OldWriteFile, NewWriteFile);
		DetourAttach(&(PVOID&)OldReadFile, NewReadFile);
		DetourAttach(&(PVOID&)OldHeapCreate, NewHeapCreate);
		DetourAttach(&(PVOID&)OldHeapDestory, NewHeapDestory);
		//OldHeapFree
		DetourAttach(&(PVOID&)OldHeapFree, NewHeapFree);
		DetourAttach(&(PVOID&)OldRegCreateKeyEx, NewRegCreateKeyEx);
		DetourAttach(&(PVOID&)OldRegSetValueEx, NewRegSetValueEx);
		DetourAttach(&(PVOID&)OldRegDeleteValue, NewRegDeleteValue);
		DetourAttach(&(PVOID&)OldRegCloseKey, NewRegCloseKey);
		DetourAttach(&(PVOID&)OldRegOpenKeyEx, NewRegOpenKeyEx);
		DetourAttach(&(PVOID&)Oldsocket, Newsocket);
		DetourAttach(&(PVOID&)Oldbind, Newbind);
		DetourAttach(&(PVOID&)Oldsend, Newsend);
		DetourAttach(&(PVOID&)Oldconnect, Newconnect);
		DetourAttach(&(PVOID&)Oldrecv, Newrecv);
		//DetourAttach(&(PVOID&)Oldmemcpy, Newmemcpy);
		DetourTransactionCommit();
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)OldMessageBoxW, NewMessageBoxW);
		DetourDetach(&(PVOID&)OldMessageBoxA, NewMessageBoxA);
		DetourDetach(&(PVOID&)OldCreateFile, NewCreateFile);
		DetourDetach(&(PVOID&)OldWriteFile, NewWriteFile);
		DetourDetach(&(PVOID&)OldReadFile, NewReadFile);
		DetourDetach(&(PVOID&)OldHeapCreate, NewHeapCreate);
		DetourDetach(&(PVOID&)OldHeapDestory, NewHeapDestory);
		DetourDetach(&(PVOID&)OldHeapFree, NewHeapFree);
		DetourDetach(&(PVOID&)OldRegCreateKeyEx, NewRegCreateKeyEx);
		DetourDetach(&(PVOID&)OldRegSetValueEx, NewRegSetValueEx);
		DetourDetach(&(PVOID&)OldRegDeleteValue, NewRegDeleteValue);
		DetourDetach(&(PVOID&)OldRegCloseKey, NewRegCloseKey);
		DetourDetach(&(PVOID&)OldRegOpenKeyEx, NewRegOpenKeyEx);
		DetourDetach(&(PVOID&)Oldsocket, Newsocket);
		DetourDetach(&(PVOID&)Oldbind, Newbind);
		DetourDetach(&(PVOID&)Oldsend, Newsend);
		DetourDetach(&(PVOID&)Oldconnect, Newconnect);
		DetourDetach(&(PVOID&)Oldrecv, Newrecv);
		//DetourDetach(&(PVOID&)Oldmemcpy, Newmemcpy);
		DetourTransactionCommit();
		UnmapViewOfFile(lpBase);
		CloseHandle(hMapFile);
		break;
	}
	}
	return true;
}