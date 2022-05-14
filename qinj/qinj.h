#ifndef __QINJ_H__
#define __QINJ_H__

#include <QTimer>
#include <QThread>
#include <QLabel>
#include <QApplication>

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
	QLabel *lblTxf = 0;
	QTimer *tmrTestFlash = 0;

public: 
        Injection(QObject *parent = 0);

public Q_SLOTS:
	void onInjectionThreadMoved(QThread *newThread);
	void testFlashTimerExpired();
};
#endif
