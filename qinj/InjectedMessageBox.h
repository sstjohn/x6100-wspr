#ifndef _INJECTEDMESSAGEBOX_H_
#define _INJECTEDMESSAGEBOX_H_

#include <QMessageBox>

class InjectedMessageBox : public QMessageBox
{
	Q_OBJECT

protected:
	void keyPressEvent(QKeyEvent *e);
};

#endif
