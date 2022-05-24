#include <cstdlib>
#include <QThread>
#include <QString>
#include <QScreen>
#include <QRect>
#include <QMessageBox>
#include <QKeyEvent>
#include <QApplication>
#include <QLabel>
#include <QDebug>
#include <QPushButton>
#include "qinj.h"

extern "C" {
#include <Python.h> 
}


AppButtonWatcher::AppButtonWatcher(QObject *parent = 0) :
	QObject(parent) {}

bool AppButtonWatcher::eventFilter(QObject *obj, QEvent *event)
{	
	bool result = QObject::eventFilter(obj, event);
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if (keyEvent->modifiers() & Qt::ControlModifier) {
			if (keyEvent->key() == Qt::Key_2) {
				Q_EMIT appsMenuShowing();
			}
		}
	} 
	return result;
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

	AppButtonWatcher *appButtonWatcher = new AppButtonWatcher(this);
	topLevelWidget->installEventFilter(appButtonWatcher);
	connect(appButtonWatcher, &AppButtonWatcher::appsMenuShowing, this, &Injection::appsMenuShowing, Qt::QueuedConnection);
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
				messageBoxRequested("You pressed the WSPR button!");
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

static bool __initialized = false;

__attribute__((constructor)) 
void initialize()
{
	if (!__initialized) {
		__initialized = true;
		bool iPythonWanted = NULL != std::getenv("QINJ_CONSOLE");
		injectionThread = new InjectionThread();
		injectionThread->start();
		if(iPythonWanted) {
			iPythonThread = new IPythonThread();
			iPythonThread->start();
		}
	}
}
