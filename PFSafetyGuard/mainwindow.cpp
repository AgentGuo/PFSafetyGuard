#include "mainwindow.h"
#include "ui_mainwindow.h"
char typeStr[20][20] = { "None", "MessageBoxA", "MessageBoxW",
"CreateFile", "WriteFile", "ReadFile", "HeapCreate",
"HeapDestory", "HeapFree", "RegCreateKeyEx", "RegSetValueEx",
"RegCloseKey", "RegOpenKeyEx", "RegDeleteValue", "socket",
"bind", "send", "connect", "recv" };
info recvInfo;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	initUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::initUI() {
	//QPixmap iconaaa(":/images/images/safe.ico");
	////label->setPixmap(iconaaa);
	//ui->label_5->setPixmap(iconaaa);
	//ui->tempButton->setIcon
	//ui->label_5->setWindowIcon(QIcon(":/images/images/safe.ico"));
	//ui->infoButton->setDisabled(true);
	ui->infoButton->setIcon(QIcon(":/images/images/safe.ico"));
	connect(&threadA, SIGNAL(newInfo(QString, int)), this, SLOT(on_ThreadA_newInfo(QString, int)));
	connect(&threadA, SIGNAL(newProcessModules(QString)), this, SLOT(on_ThreadA_newProcessModules(QString)));
	connect(&threadA, SIGNAL(newProcessPriority(QString)), this, SLOT(on_ThreadA_newProcessPriority(QString)));
	connect(&threadA, SIGNAL(newProcessID(QString)), this, SLOT(on_ThreadA_newProcessID(QString)));
	connect(&threadA, SIGNAL(newProcessName(QString)), this, SLOT(on_ThreadA_newProcessName(QString)));
	//connect(&threadA, SIGNAL(newValue(QString)), this, SLOT(on_ThreadA_newValue(QString)));
	connect(&threadA, SIGNAL(newInfo()), this, SLOT(on_ThreadA_newInfo()));
}
void MainWindow::on_openFileButton_pressed() {
	QString fileName = QFileDialog::getOpenFileName(
		this, tr("open image file"),
		"./", tr("Image files(*.txt *.exe);;All files (*.*)"));

	if (fileName.isEmpty())
	{
		QMessageBox mesg;
		mesg.warning(this, "warning", "open file failed");
		return;
	}
	else
	{
		ui->filePathTextEdit->setText(fileName);
	}
}
void MainWindow::on_tempButton_pressed() {
	QByteArray temp = ui->filePathTextEdit->toPlainText().toLatin1();
	threadA.init(temp.data());
	threadA.start();
}

void MainWindow::on_clsButton_pressed() {
	ui->infoTree->clear();
}


void MainWindow::on_ThreadA_newValue(QString str) {
	//ui->tempLabel->setText(str);
	//ui->filePathTextEdit->setText(str);
}

void MainWindow::closeEvent(QCloseEvent *event) {
	if (threadA.isRunning()) {
		threadA.stopThread();
		threadA.wait();
	}
	event->accept();
}

void MainWindow::on_ThreadA_newInfo() {
	//QString temp = QString(QLatin1String(fileName));
	//emit newValue(QString(QLatin1String(fileName)));
	//msleep(1500);
	QTreeWidgetItem* item = new QTreeWidgetItem();
	char temp[128] = "";
	sprintf(temp, "%d-%d-%d %-02d:%-02d  (%-d.%-ds)",
		recvInfo.st.wYear, recvInfo.st.wMonth, recvInfo.st.wDay,
		recvInfo.st.wHour, recvInfo.st.wMinute, recvInfo.st.wSecond,
		recvInfo.st.wMilliseconds);
	//QTreeWidgetItem* item2 = new QTreeWidgetItem();
	//item->setText(0, "1111");
	item->setData(0, 0, typeStr[recvInfo.type]);
	item->setData(1, 0, temp);
	for (int i = 0; i < recvInfo.argNum; i++) {
		QTreeWidgetItem* item2 = new QTreeWidgetItem();
		item2->setData(0, 0, recvInfo.argName[i]);
		item2->setData(1, 0, recvInfo.argValue[i]);
		item->addChild(item2);
	}
	//item->setData(1, 0, "2222");
	//item2->setData(0, 0, "3333");
	//item2->setData(1, 0, "4444");
	//item->addChild(item2);
	//item2->setData(0, 0, "5555");
	//item2->setData(1, 0, "6666");
	//item->addChild(item2);
	ui->infoTree->addTopLevelItem(item);
}
void MainWindow::on_ThreadA_newProcessName(QString str) {
	ui->processName->setText(str);
}
void MainWindow::on_ThreadA_newProcessID(QString str) {
	ui->processID->setText(str);
}
void MainWindow::on_ThreadA_newProcessPriority(QString str) {
	ui->processPriority->setText(str);
}
void MainWindow::on_ThreadA_newProcessModules(QString str) {
	ui->processModules->setText(str);
}
void MainWindow::on_ThreadA_newInfo(QString str, int status) {
	ui->info->setText(str);
	if (status == 2) {
		ui->infoButton->setIcon(QIcon(":/images/images/error.ico"));
	}
	else if (status == 1) {
		ui->infoButton->setIcon(QIcon(":/images/images/warning.ico"));
	}
	else {
		ui->infoButton->setIcon(QIcon(":/images/images/safe.ico"));
	}
	//ui->label_5->setWindowIcon(QIcon(":/images/images/safe.ico"));
	//ui->label_5->setWindowIcon
	//ui->info->setTextColor()
}