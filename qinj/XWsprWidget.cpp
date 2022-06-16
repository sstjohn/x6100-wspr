#include "XWsprWidget.h"
#include <QWidget>
#include <QStandardItemModel>
#include <QTableView>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollBar>

void XWsprWidget::wsprReceived(const QString &time,
		const QString &snr,
		const QString &freq,
		const QString &call,
		const QString &grid,
		const QString &power)
{
	QList<QStandardItem *> cols;
	cols << new QStandardItem(time);
	cols << new QStandardItem(snr);
	cols << new QStandardItem(freq);
	cols << new QStandardItem(call);
	cols << new QStandardItem(grid);
	cols << new QStandardItem(power);
	wsprStore->insertRow(0, cols);
}

void XWsprWidget::scrollTable(bool up)
{
	qtv->verticalScrollBar()->setValue(
		qtv->verticalScrollBar()->value() + (up ? -1 : 1)
	);
}

XWsprWidget::XWsprWidget(QWidget *parent) :
	QWidget(parent)
{
	setObjectName("XWsprWidget");

	setGeometry(0, 176, 800, 250);
	setAutoFillBackground(true);

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

	show();
}
