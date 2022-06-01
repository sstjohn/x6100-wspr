#include <cstdlib>
#include <QThread>
#include <QString>
#include <QScreen>
#include <QRect>
#include <QMessageBox>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QApplication>
#include <QLabel>
#include <QDebug>
#include <QPushButton>
#include "qinj.h"

extern "C" {
#include <Python.h> 
}

AppButtonWatcher::AppButtonWatcher(QObject *parent) : QObject(parent) 
{
	wsprWidget = static_cast<Injection *>(parent)->getWsprWidget();
}

bool AppButtonWatcher::eventFilter(QObject *obj, QEvent *event)
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


void InjectedMessageBox::keyPressEvent(QKeyEvent *e)
{
	this->done(0);
}

Injection *injection = 0;
InjectionThread *injectionThread = 0;
IPythonThread *iPythonThread = 0;

Injection::Injection(QObject *parent) : QObject(parent) 
{
	setObjectName("qinj");

	this->threadMovedConnection = connect(QThread::currentThread(),
		SIGNAL(injectionThreadMoved(QThread *)),
		this,
		SLOT(injectionThreadMoved(QThread *))
	);
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

void Injection::injectionThreadMoved(QThread *newThread)
{
	topLevelWidget = QApplication::topLevelWidgets().back();
	setParent(topLevelWidget);
	
	lblTxf = topLevelWidget->findChild<QLabel *>("labelFullbandTxe");
	if (lblTxf) {
		lblTxf->setText(getTestText());
		lblTxf->show();
	}

	if (NULL == std::getenv("QINJ_DONTFLASH_TEXT")) {
		tmrTestFlash = new QTimer(this);
		connect(this->tmrTestFlash, &QTimer::timeout, this, &Injection::testFlashTimerExpired);
		tmrTestFlash->start(1000);
	}

	wsprWidget = new XWsprWidget(topLevelWidget);

	AppButtonWatcher *appButtonWatcher = new AppButtonWatcher(this);
	QApplication::instance()->installEventFilter(appButtonWatcher);
	connect(appButtonWatcher, &AppButtonWatcher::appsMenuShowing, this, &Injection::appsMenuShowing, Qt::QueuedConnection);
}

XWsprWidget *Injection::getWsprWidget() { return this->wsprWidget; }

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

void InjectionThread::run()
{
	while (!QApplication::instance()) { QThread::sleep(1); }
	injection = new Injection();

	QThread *appThread = QApplication::instance()->thread();
	injection->moveToThread(appThread);
	Q_EMIT injectionThreadMoved(appThread);
}

void IPythonThread::run()
{
	Py_Initialize();
	PyRun_SimpleString("import qinj_console; qinj_console.run()");
	Py_Finalize();
	QApplication::quit();
}

static bool __qinj_initialized = false;

void qinj_initialize()
{
	if (!__atomic_test_and_set(&__qinj_initialized, 0)) {
		bool iPythonWanted = NULL != std::getenv("QINJ_CONSOLE");
		injectionThread = new InjectionThread();
		injectionThread->start();
		if(iPythonWanted) {
			iPythonThread = new IPythonThread();
			iPythonThread->start();
		}
	}
}
