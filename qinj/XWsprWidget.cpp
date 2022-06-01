#include "XWsprWidget.h"
#include <QWidget>
#include <QStandardItemModel>
#include <QTableView>
#include <QHBoxLayout>
#include <QDebug>
#include <QHeaderView>
#include <QScrollBar>

void XWsprWidget::wsprReceived(QStringList data)
{
	QList<QStandardItem *> cols;
	for (const auto& i : data)
		cols << new QStandardItem(i);
	wsprStore->insertRow(0, cols);
}

void XWsprWidget::scrollTable(bool up)
{
	qtv->verticalScrollBar()->setValue(
		qtv->verticalScrollBar()->value() + (up ? -1 : 1)
	);
}

void XWsprWidget::wsprConnectionReceived()
{
	QLocalSocket *incoming = wsprReceiver->nextPendingConnection();
	if (!incoming)
		return;
	connect(incoming, &QLocalSocket::readyRead, [=]() {
		while (incoming->canReadLine()) {
			const QByteArray data = incoming->readLine().trimmed();
			wsprReceived(QString(data).split("\t"));
		}
	});
}

XWsprWidget::XWsprWidget(QWidget *parent) :
	QWidget(parent)
{
	setObjectName("XWsprWidget");

	setGeometry(50, 50, 700, 375);
	setAutoFillBackground(true);
	move(parent->rect().center() - this->rect().center());

	wsprStore = new QStandardItemModel(0, 6);
	QStringList headers;
	headers << "Time" << "SNR" << "Freq" << "Call" << "Grid" << "Pwr";
        wsprStore->setHorizontalHeaderLabels(headers);

        qtv = new QTableView();
	qtv->verticalHeader()->setDefaultSectionSize(40);
	qtv->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	qtv->verticalHeader()->hide();
        qtv->setShowGrid(false);
	qtv->setModel(this->wsprStore);
	
	for (int i = 1; i < 6; i++)
	{
		if (i == 1 || i == 5)
			qtv->setColumnWidth(i, 60);
		else
			qtv->setColumnWidth(i, 140);
	}
	qtv->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

        layout = new QHBoxLayout();
        layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(qtv);
        setLayout(layout); 

	QLocalServer::removeServer("wspr");
	wsprReceiver = new QLocalServer(this);
	if (!wsprReceiver->listen("wspr")) {
		qWarning("Unable to start the server: %s.", wsprReceiver->errorString().toLocal8Bit().data());
		wsprReceiver = NULL;
		return;
	}
	connect(wsprReceiver, &QLocalServer::newConnection, this, &XWsprWidget::wsprConnectionReceived);
}

XWsprWidget::~XWsprWidget() 
{
	if (wsprReceiver)
		wsprReceiver->close();
}
