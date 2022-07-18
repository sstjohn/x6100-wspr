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
	if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if (keyEvent->modifiers() & Qt::ControlModifier ||
		    keyEvent->key() == Qt::Key_F1 ||
		    keyEvent->key() == Qt::Key_F2 ||
		    keyEvent->key() == Qt::Key_F3 ||
		    keyEvent->key() == Qt::Key_F4 ||
		    keyEvent->key() == Qt::Key_F5 ||
		    keyEvent->key() == Qt::Key_PageDown ||
		    keyEvent->key() == Qt::Key_PageUp) {
			qWarning("key event filtered");
			return true;
		}
	} else if (event->type() == QEvent::Wheel) {
		QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
		if (wheelEvent->modifiers()) {
			wsprWidget->scrollTable(wheelEvent->angleDelta().y() > 0);
			return true;
		}
	} else if (event->type() > QEvent::User) {
		qWarning("filtering event type %i", (int)event->type());
		return true;
	}
	return QObject::eventFilter(obj, event);
}


