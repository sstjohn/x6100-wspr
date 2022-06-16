#ifndef _XWSPRWIDGET_H_
#define _XWSPRWIDGET_H_

#include <QStandardItemModel>
#include <QWidget>
#include <QObject>
#include <QLayout>
#include <QTableView>

class XWsprWidget : public QWidget 
{
	Q_OBJECT

public:
	XWsprWidget(QWidget *parent = 0);
	void scrollTable(bool up);
	QLayout *layout;

private:
	QStandardItemModel *wsprStore;
	QTableView *qtv;

public Q_SLOTS:
	void wsprReceived(const QString &time,
		const QString &snr,
		const QString &freq,
		const QString &call,
		const QString &grid,
		const QString &power);
};

#endif
