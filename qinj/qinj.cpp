#include "qinj.h"

static bool __qinj_initialized = false;

void qinj_initialize()
{
	if (!__atomic_test_and_set(&__qinj_initialized, 0)) {
		bool iPythonWanted = NULL != std::getenv("QINJ_CONSOLE");
		(new InjectionThread())->start();
		if(iPythonWanted) {
			(new IPythonThread())->start();
		}
	}
}
