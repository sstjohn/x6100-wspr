#ifndef _APPBUTTONWATCHER_H_
#define _APPBUTTONWATCHER_H_

#include <QEvent>

#include "XWsprWidget.h"

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

#endif 
