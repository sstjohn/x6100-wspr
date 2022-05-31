#ifndef __QINJ_H__
#define __QINJ_H__

#include <QThread>
#include <QLabel>
#include <QEvent>
#include <QApplication>
#include <QTextDocument>

void __attribute__((constructor)) initialize();

class ColorChangerInjectionThread : public QThread 
{
	Q_OBJECT

private:
	void run() override;

Q_SIGNALS:
	void injectionThreadMoved();

};

class ColorChangerInjection : public QObject
{
        Q_OBJECT

private:
	QMetaObject::Connection threadMovedConnection;
	QWidget *topLevelWidget = 0;
	QTextDocument *doc = 0;

public: 
        ColorChangerInjection(QObject *parent = 0);

public Q_SLOTS:
	void injectionThreadMoved();
	void volLblTextChanged();
};
#endif
