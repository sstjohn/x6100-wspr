#ifndef _INJECTION_H_
#define _INJECTION_H_

#include <QLabel>
#include <QTimer>

#include "XWsprWidget.h"

class Injection : public QObject
{
        Q_OBJECT

private:
	QMetaObject::Connection threadMovedConnection;
	QMetaObject::Connection wsprButtonConnection;

	QLabel *lblTxf = 0;
	QTimer *tmrTestFlash = 0;
	const char *getTestText();
	QWidget *topLevelWidget = 0;
	XWsprWidget *wsprWidget = 0;

public: 
        Injection(QObject *parent = 0);
	XWsprWidget *getWsprWidget();

public Q_SLOTS:
	void injectionThreadMoved(QThread *newThread);
	void testFlashTimerExpired();
	bool screenshotRequested(QString);
	void messageBoxRequested(QString);
	void appsMenuShowing();
};

#endif
