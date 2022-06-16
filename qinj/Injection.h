#ifndef _INJECTION_H_
#define _INJECTION_H_

#include <QDBusAbstractAdaptor>
#include <QLabel>
#include <QTimer>

#include "XWsprWidget.h"

class Injection : public QDBusAbstractAdaptor
{
        Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "lol.ssj.xwspr")

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
	void injectionThreadMoved();
	void testFlashTimerExpired();
	bool screenshotRequested(QString);
	void messageBoxRequested(QString);
	void appsMenuShowing();
	void wsprReceived(
		const QString &time,
		const QString &snr,
		const QString &freq,
		const QString &call,
		const QString &grid,
		const QString &power
	);
};

#endif
