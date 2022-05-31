#include <cstdlib>
#include <QThread>
#include <QString>
#include <QKeyEvent>
#include <QApplication>
#include <QDebug>
#include <QTextFrame>
#include <QTextCursor>

#include "colorchanger.h"


ColorChangerInjection *injection = 0;
ColorChangerInjectionThread *injectionThread = 0;

void ColorChangerInjection::volLblTextChanged()
{
	for (QTextBlock block = doc->begin(); block != doc->end(); block = block.next()) {
		for (QTextBlock::iterator it = block.begin(); !(it.atEnd()); ++it) {
			QTextFragment frag = it.fragment();
			if (frag.isValid()) {
				const QTextCharFormat curFormat = frag.charFormat();
				QTextCharFormat newFormat = QTextCharFormat(curFormat);
				newFormat.setForeground(QBrush("white"));
				if (curFormat != newFormat) {
					QTextCursor *cursor = new QTextCursor(block);
					cursor->select(QTextCursor::BlockUnderCursor);
					cursor->mergeCharFormat(newFormat);
				}
			}
		}
	}
}

void ColorChangerInjection::injectionThreadMoved()
{
	topLevelWidget = QApplication::topLevelWidgets().back();
	setParent(topLevelWidget);

	QLabel *lblVol = topLevelWidget->findChild<QLabel *>("labelVolum");
	doc = lblVol->findChild<QTextDocument *>("");
	connect(doc, &QTextDocument::contentsChanged, this, &ColorChangerInjection::volLblTextChanged);
}

ColorChangerInjection::ColorChangerInjection(QObject *parent) : QObject(parent) 
{
	this->threadMovedConnection = connect(QThread::currentThread(),
                SIGNAL(injectionThreadMoved()),
                this,
                SLOT(injectionThreadMoved())
        );
}


void ColorChangerInjectionThread::run()
{
	while (!QApplication::instance()) { QThread::sleep(1); }
	injection = new ColorChangerInjection();
	QThread *appThread = QApplication::instance()->thread();
	injection->moveToThread(appThread);
	Q_EMIT injectionThreadMoved();
}

static bool __initialized = 0;

void initialize()
{
	if (!__atomic_test_and_set(&__initialized, 0)) {
		injectionThread = new ColorChangerInjectionThread();
		injectionThread->start();
	}
}
