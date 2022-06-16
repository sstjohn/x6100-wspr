#include <QKeyEvent>

#include <QDebug>

#include "InjectedEventFilter.h"
#include "Injection.h"

InjectedEventFilter::InjectedEventFilter(QObject *parent) : QObject(parent) 
{
	wsprWidget = static_cast<Injection *>(parent)->getWsprWidget();
}

bool InjectedEventFilter::eventFilter(QObject *obj, QEvent *event)
{	
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if (wsprWidget->isHidden()) {
			if (keyEvent->modifiers() & Qt::ControlModifier) {
				if (keyEvent->key() == Qt::Key_2) {
					Q_EMIT appsMenuShowing();
				}
			}
		} else {
			if (keyEvent->key() != Qt::Key_F5)	
				return true;
		}
	} else if (event->type() == QEvent::KeyRelease) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if (!wsprWidget->isHidden() && keyEvent->key() != Qt::Key_F5)
			return true;
	} else if (event->type() == QEvent::Wheel) {
		if (!wsprWidget->isHidden()) {
			QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
			if (!wheelEvent->modifiers()) {
				wsprWidget->scrollTable(wheelEvent->angleDelta().y() > 0);
			}
			return true;
		}
	} 
	return QObject::eventFilter(obj, event);
}


