#ifndef __COLORCHANGER_H__
#define __COLORCHANGER_H__

#include <QThread>
#include <QLabel>
#include <QEvent>
#include <QApplication>
#include <QTextDocument>
#include <QPushButton>

void __attribute__((constructor)) colorchanger_initialize();

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
	void buttonColorChanger(QPushButton *pb);

protected:
	bool eventFilter(QObject *obj, QEvent *event);
	
public: 
        ColorChangerInjection(QObject *parent = 0);

Q_SIGNALS:
	void fButtonsNeedUpdate();

public Q_SLOTS:
	void injectionThreadMoved();
	void volLblTextColorChange();
	void fButtonsColorChange();
};
#endif
