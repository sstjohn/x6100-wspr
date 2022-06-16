#include "InjectedMessageBox.h"

void InjectedMessageBox::keyPressEvent(QKeyEvent *e)
{
	this->done(0);
}
