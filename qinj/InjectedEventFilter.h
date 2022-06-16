#ifndef _INJECTEDEVENTFILTER_H_
#define _INJECTEDEVENTFILTER_H_

#include <QEvent>

#include "XWsprWidget.h"

class InjectedEventFilter : public QObject
{
	Q_OBJECT
public:
	InjectedEventFilter(QObject *parent = 0);
private:
	XWsprWidget *wsprWidget;
Q_SIGNALS:
	void appsMenuShowing();
protected:
	bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif 
