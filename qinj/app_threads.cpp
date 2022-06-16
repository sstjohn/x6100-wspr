#include <QApplication>

#include "app_threads.h"
#include "Injection.h"

extern "C" {
#include "Python.h"
}

void InjectionThread::run()
{
	while (!QApplication::instance()) { QThread::sleep(1); }
	Injection *injection = new Injection();

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
