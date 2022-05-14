#include <QThread>
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
		SLOT(onInjectionThreadMoved(QThread *))
	);
}

void Injection::onInjectionThreadMoved(QThread *newThread)
{
	QWidget *topLevelWidget = QApplication::topLevelWidgets().back();
	this->setParent(topLevelWidget);
	
	this->lblTxf = topLevelWidget->findChild<QLabel *>("labelFullbandTxe");
	if (this->lblTxf) {
		this->lblTxf->setText("TEST");
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
		injectionThread = new InjectionThread();
		injectionThread->start();
		iPythonThread = new IPythonThread();
		iPythonThread->start();
	}
}
