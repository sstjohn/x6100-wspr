#ifndef __QINJ_H__
#define __QINJ_H__

#include <QTimer>
#include <QThread>
#include <QString>
#include <QScreen>
#include <QMessageBox>
#include <QLabel>
#include <QKeyEvent>
#include <QApplication>

#include "XWsprWidget.h"

void __attribute__((constructor)) qinj_initialize();

class AppButtonWatcher : public QObject
{
	Q_OBJECT
public:
	AppButtonWatcher(QObject *parent = 0);
private:
	XWsprWidget *wsprWidget;
Q_SIGNALS:
	void appsMenuShowing();
protected:
	bool eventFilter(QObject *obj, QEvent *event) override;
};

class InjectedMessageBox : public QMessageBox
{
	Q_OBJECT

protected:
	void keyPressEvent(QKeyEvent *e);
};

class InjectionThread : public QThread 
{
	Q_OBJECT

private:
	void run() override;

Q_SIGNALS:
	void injectionThreadMoved(QThread *newThread);
};

class IPythonThread : public QThread 
{
private:
	void run() override;
};

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
	void tryLabel(QString);
};
#endif
