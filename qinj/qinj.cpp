#include <cstdlib>
#include <QThread>
#include <QString>
#include <QScreen>
#include <QApplication>
#include <QLabel>
#include "qinj.h"

extern "C" {
#include <Python.h> 
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

bool Injection::screenshotRequested(QString filename)
{
	QScreen *screen = QApplication::primaryScreen();
	if (!screen)
		return false;

	QPixmap pixmap = screen->grabWindow(0);
	if (!pixmap)
		return false;

	return pixmap.save(filename);
}

void Injection::injectionThreadMoved(QThread *newThread)
{
	QWidget *topLevelWidget = QApplication::topLevelWidgets().back();
	this->setParent(topLevelWidget);
	
	this->lblTxf = topLevelWidget->findChild<QLabel *>("labelFullbandTxe");
	if (this->lblTxf) {
		this->lblTxf->setText(this->getTestText());
		this->lblTxf->show();
	}

	this->tmrTestFlash = new QTimer(this);
	connect(this->tmrTestFlash, &QTimer::timeout, this, &Injection::testFlashTimerExpired);
	this->tmrTestFlash->start(1000);
}

void Injection::testFlashTimerExpired()
{
	if (this->lblTxf->isHidden())
		this->lblTxf->show();
	else
		this->lblTxf->hide();
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
	PyRun_SimpleString("from IPython import embed; embed()");
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
