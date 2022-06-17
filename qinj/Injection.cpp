#include <QApplication>
#include <QDebug>
#include <QDBusConnection>
#include <QPushButton>
#include <QScreen>
#include <QThread>

#include "InjectedEventFilter.h"
#include "Injection.h"
#include "InjectedMessageBox.h"

Injection::Injection(QObject *parent) : QDBusAbstractAdaptor(parent)
{
	setObjectName("qinj");

	this->threadMovedConnection = connect(
		QThread::currentThread(),
		SIGNAL(injectionThreadMoved()),
		this,
		SLOT(injectionThreadMoved())
	);
}

void Injection::injectionThreadMoved()
{
	topLevelWidget = QApplication::topLevelWidgets().back();
	setParent(topLevelWidget);
	
	lblTxf = topLevelWidget->findChild<QLabel *>("labelFullbandTxe");
	if (lblTxf) {
		lblTxf->setText(getTestText());
		lblTxf->show();
	}

	if (NULL == std::getenv("QINJ_DONTFLASH_TEXT")) {
		tmrTxfFlash = new QTimer(this);
		connect(tmrTxfFlash, &QTimer::timeout, this, &Injection::txfFlashTimerExpired);
		tmrTxfFlash->start(1000);
	}

	wsprWidget = new XWsprWidget(topLevelWidget);

	InjectedEventFilter *eventFilter = new InjectedEventFilter(this);
	QApplication::instance()->installEventFilter(eventFilter);
	connect(
		eventFilter, &InjectedEventFilter::appsMenuShowing, 
		this, &Injection::appsMenuShowing, 
		Qt::QueuedConnection
	);

	QDBusConnection systemBus = QDBusConnection::systemBus();
	if (!systemBus.isConnected()) {
		qWarning("cannot connect to system bus");
	} else {
		systemBus.registerService("lol.ssj.xwspr");
		systemBus.registerObject("/", this, QDBusConnection::ExportAllSlots);
	}
}

const char *Injection::getTestText()
{
	const char *retVal = std::getenv("QINJ_TEXT");
	if (!retVal)
		retVal = "WSPR";
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

void Injection::txfFlashTimerExpired()
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

void Injection::wsprReceived(const QString &time,
		const QString &snr,
		const QString &freq,
		const QString &call,
		const QString &grid,
		const QString &power)
{
	if (wsprWidget) {
		wsprWidget->wsprReceived(time, snr, freq, call, grid, power);
	} else {		
		qWarning("%s %s %s %s %s %s", qUtf8Printable(time), qUtf8Printable(snr), 
		 qUtf8Printable(freq), qUtf8Printable(call), qUtf8Printable(grid), qUtf8Printable(power));
	}
}
