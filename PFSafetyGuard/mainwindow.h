#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <windows.h>
#include <thread>
#include <Qthread>
#include <QCloseEvent>
#include "psapi.h"
#include <unordered_set>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using namespace std;
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
struct info {
	int type, argNum;
	SYSTEMTIME st;
	char argName[10][30] = { 0 };
	char argValue[10][70] = { 0 };
};

class myThread :public QThread {
	Q_OBJECT
private:
	bool running;
	char filePath[256], fileName[128];
	unordered_set<int> heapSet;
	unordered_set<string> folderSet;
protected:
	void run();
public:
	void init(char * path);
	void getFileName(char *filePath, char *fileName);
	void stopThread();
	int GetProcessPriority(HANDLE hProcess);
	void checkFunc();
	void createFileCheck();
	void getLastFolder(char* filePath, string & folder);
signals:
	void newValue(QString str);
	void newInfo();
	void newProcessName(QString str);
	void newProcessID(QString str);
	void newProcessPriority(QString str);
	void newProcessModules(QString str);
	void newInfo(QString str, int status);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
	myThread threadA;
protected:
	void closeEvent(QCloseEvent *event);
private slots:
	void on_openFileButton_pressed();
	void on_tempButton_pressed();
	void on_clsButton_pressed();
	void on_ThreadA_newValue(QString str);
	void on_ThreadA_newInfo();
	void on_ThreadA_newProcessName(QString str);
	void on_ThreadA_newProcessID(QString str);
	void on_ThreadA_newProcessPriority(QString str);
	void on_ThreadA_newProcessModules(QString str);
	void on_ThreadA_newInfo(QString str, int status);
public:
    MainWindow(QWidget *parent = nullptr);
	void initUI();
    ~MainWindow();
private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
