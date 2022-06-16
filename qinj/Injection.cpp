#include <QApplication>
#include <QPushButton>
#include <QScreen>

#include "AppButtonWatcher.h"
#include "Injection.h"
#include "InjectedMessageBox.h"

Injection::Injection(QObject *parent) : QObject(parent) 
{
	setObjectName("qinj");

	this->threadMovedConnection = connect(
		QThread::currentThread(),
		SIGNAL(injectionThreadMoved(QThread *)),
		this,
		SLOT(injectionThreadMoved(QThread *))
	);
}

void Injection::injectionThreadMoved(QThread *newThread)
{
	topLevelWidget = QApplication::topLevelWidgets().back();
	setParent(topLevelWidget);
	
	lblTxf = topLevelWidget->findChild<QLabel *>("labelFullbandTxe");
	if (lblTxf) {
		lblTxf->setText(getTestText());
		lblTxf->show();
	}

	wsprWidget = new XWsprWidget(topLevelWidget);

	AppButtonWatcher *appButtonWatcher = new AppButtonWatcher(this);
	QApplication::instance()->installEventFilter(appButtonWatcher);
	connect(appButtonWatcher, &AppButtonWatcher::appsMenuShowing, this, &Injection::appsMenuShowing, Qt::QueuedConnection);
}

const char *Injection::getTestText()
{
	const char *retVal = std::getenv("QINJ_TEXT");
	if (!retVal)
		retVal = "TEST";
	return retVal;
}

void Injection::messageBoxRequested(
	QString txt = "test")
{
	InjectedMessageBox msgBox;
	msgBox.setWindowTitle("messagebox");
	msgBox.setText(txt);
	msgBox.exec();
}

bool Injection::screenshotRequested(QString filename)
{
	QScreen *screen = QApplication::primaryScreen();
	if (!screen)
		return false;

	QRect geo = screen->geometry();

	QPixmap pixmap = screen->grabWindow(0, geo.y(), geo.x(), geo.height(), geo.width());

	if (!pixmap)
		return false;

	return pixmap.save(filename);
}


XWsprWidget *Injection::getWsprWidget() 
{ 
	return this->wsprWidget; 
}

void Injection::testFlashTimerExpired()
{
	if (this->lblTxf->isHidden())
		this->lblTxf->show();
	else
		this->lblTxf->hide();
}	

void Injection::appsMenuShowing()
{
	QPushButton *f5 = topLevelWidget->findChild<QPushButton *>("pushButton_F5");
	f5->setIcon(QIcon());
	f5->setText("WSPR");	
	if (!wsprButtonConnection)
		wsprButtonConnection = connect(f5, &QPushButton::clicked, [=]() {
			if (f5->text() == "WSPR")
			{
				if (wsprWidget->isHidden()) {
					wsprWidget->show();
					wsprWidget->raise();
					f5->setFocus();
				} else {
					wsprWidget->hide();
					f5->clearFocus();
				}
			}
		});		
}

