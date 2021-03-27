#include "mainwindow.h"
#include "ui_mainwindow.h"
extern info recvInfo;
char priorityStr[8][20] = { "NORMAL", "IDLE" , "REALTIME", "HIGH", "NULL", "ABOVENORMAL", "BELOWNORMAL" };
void myThread::run() {
	int lastType = -1;
	SYSTEMTIME lastSt;
	//QString temp = QString(QLatin1String(fileName));
	//emit newValue(QString(QLatin1String(fileName)));
	//msleep(1500);
	emit newProcessName(QString(QLatin1String(fileName)));
	HANDLE hSemaphore = CreateSemaphore(NULL, 0, 1, L"mySemaphore");
	HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(info), L"ShareMemory");
	LPVOID lpBase = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	STARTUPINFOA startupInfo = { 0 };
	PROCESS_INFORMATION  processInformation = { 0 };
	char temp[512];
	char moduleName[256];
	HMODULE hMod[100];
	DWORD cbNeeded;
	int moduleNum;
	// *****需要修改部分****
	// 启动注册器进程
	BOOL bSuccess = CreateProcessA(".\\syringeFolder\\syringe.exe", filePath, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startupInfo, &processInformation);
	sprintf(temp, "%d", processInformation.dwProcessId);
	emit newProcessID(QString(QLatin1String(temp)));
	sprintf(temp, "%s", priorityStr[GetProcessPriority(processInformation.hProcess)]);
	emit newProcessPriority(QString(QLatin1String(temp)));
	memset(temp, 0, sizeof(temp));
	msleep(500);
	if (EnumProcessModules(processInformation.hProcess, hMod, sizeof(hMod), &cbNeeded))
	{
		// 模块个数
		moduleNum = cbNeeded / sizeof(HMODULE);
		for (int i = 0; i < moduleNum; i++) {
			GetModuleFileNameA(hMod[i], moduleName, 256);
			strcat(temp, moduleName);
			strcat(temp, "\n");
		}
		emit newProcessModules(QString(QLatin1String(temp)));
	}
	msleep(1500);
	while (running) {
		//for (int i = 0; i < 2; i++) {
		//	emit newValue(QString::number(i));
		//	msleep(500);
		//}
		// 等待500ms
		if (WaitForSingleObject(hSemaphore, 10) == WAIT_OBJECT_0) {
			memcpy(&recvInfo, lpBase, sizeof(info));
			if (lastSt.wMilliseconds == recvInfo.st.wMilliseconds && lastSt.wSecond == recvInfo.st.wSecond && lastType == recvInfo.type) {
				continue;
			}
			lastSt = recvInfo.st;
			lastType = recvInfo.type;
			emit newInfo();
			checkFunc();
		}
	}
	quit();
}

void  myThread::getFileName(char *filePath, char *fileName) {
	int len = strlen(filePath);
	while (filePath[len - 1] != '\\') {
		len--;
	}
	strcpy(fileName, filePath + len);
}

void myThread::init(char * path) {
	running = true;
	memset(filePath, 0, sizeof(filePath));
	memset(fileName, 0, sizeof(fileName));
	strcpy(filePath, path);
	for (int i = 0; filePath[i] != 0; i++) {
		if (filePath[i] == '/') {
			filePath[i] = '\\';
		}
	}
	getFileName(filePath, fileName);
}

void myThread::stopThread() {
	running = false;
}

int myThread::GetProcessPriority(HANDLE hProcess)
{
	switch (GetPriorityClass(hProcess))
	{
	case NORMAL_PRIORITY_CLASS:return 0;
	case  IDLE_PRIORITY_CLASS:return 1;
	case REALTIME_PRIORITY_CLASS:return 2;
	case HIGH_PRIORITY_CLASS:return 3;
	case ABOVE_NORMAL_PRIORITY_CLASS:return 5;
	case BELOW_NORMAL_PRIORITY_CLASS:return 6;
	default:return 4;
	}
}
void myThread::checkFunc() {
	unsigned  temp;
	switch (recvInfo.type)
	{
	case CREATEFILE: {
		createFileCheck();
		break;
	}
	case HEAPCREATE: {
		//cout << strtoul(recvInfo.argValue[3], NULL, 16) << endl;
		temp = strtoul(recvInfo.argValue[3], NULL, 16);
		heapSet.insert(temp);
		//heapSet.insert(recv)
		break;
	}
	case HEAPFREE: {
		temp = strtoul(recvInfo.argValue[0], NULL, 16);
		if (heapSet.find(temp) == heapSet.end()) {
			emit newInfo(QString(QLatin1String("warning: The heap repeatedly releases or releases a non-existent heap!\n")), 2);
		}
		else {
			heapSet.erase(temp);
		}
	}
	case REGOPENKEYEX: {
		if (strstr(recvInfo.argValue[1], "SOFTWARE\Microsoft\Windows\CurrentVersion\Run")) {
			emit newInfo(QString(QLatin1String("warning: The program is modifying the startup key in the registry\n")), 2);
		}
		break;
	}
	}
}
void myThread::createFileCheck() {
	unsigned dwDesiredAccess = strtoul(recvInfo.argValue[1], NULL, 16);
	char copyFileName[128] = "";
	string copyFolder;
	getFileName(recvInfo.argValue[0], copyFileName);
	getLastFolder(recvInfo.argValue[0], copyFolder);
	if (dwDesiredAccess & GENERIC_WRITE) {
		if (strstr(copyFileName, ".exe") || strstr(copyFileName, ".dll") || strstr(copyFileName, ".ocx")) {
			emit newInfo(QString(QLatin1String("warning: Modifying executable program!\n")), 2);
		}
	}
	if (dwDesiredAccess & GENERIC_READ) {
		if (strcmp(fileName, copyFileName) == 0) {
			emit newInfo(QString(QLatin1String("warning: May be trying to self-replication\n")), 2);
		}
	}
	if (folderSet.find(copyFolder) == folderSet.end()) {
		folderSet.insert(copyFolder);
	}
	if (folderSet.size() >= 2) {
		emit newInfo(QString(QLatin1String("warning: Edited files in multiple folders!\n")), 2);
	}
}
void myThread::getLastFolder(char* filePath, string & folder) {
	int index = strlen(filePath);
	// 去除文件名
	while (filePath[index - 1] != '\\') {
		index--;
	}
	// 去除斜杠
	while (filePath[index - 1] == '\\') {
		index--;
	}
	// 得到文件夹
	while (filePath[index - 1] != '\\') {
		index--;
	}
	index++;
	while (filePath[index - 1] != '\\') {
		folder.push_back(filePath[index - 1]);
		index++;
	}
}