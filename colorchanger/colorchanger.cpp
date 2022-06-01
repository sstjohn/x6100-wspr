#include <cstdlib>
#include <QThread>
#include <QString>
#include <QKeyEvent>
#include <QApplication>
#include <QDebug>
#include <QTextFrame>
#include <QTextCursor>
#include <QPushButton>
#include <QPainter>

#include "colorchanger.h"

ColorChangerInjection *injection = 0;
ColorChangerInjectionThread *injectionThread = 0;

void ColorChangerInjection::fButtonNeedsUpdate(QPushButton *pb)
{
	if (pb) {
		this->buttonColorChanger(pb);
		return;
	}
	for (const char *name : {"pushButton_F1", "pushButton_F2", "pushButton_F3", "pushButton_F4", "pushButton_F5"}) {
		pb = topLevelWidget->findChild<QPushButton *>(name);
		this->buttonColorChanger(pb);
	}
}

void ColorChangerInjection::buttonColorChanger(QPushButton *pb)
{
	QPixmap orig = pb->icon().pixmap(153, 51);
	QImage newImage = QImage(orig.size(), QImage::Format_ARGB32_Premultiplied);
	QPainter *painter = new QPainter();

	newImage.fill(Qt::white);
	painter->begin(&newImage);
	painter->setCompositionMode(QPainter::CompositionMode_DestinationIn);
	painter->drawPixmap(0, 0, orig);
	painter->end();
	pb->setIcon(QIcon(QPixmap::fromImage(newImage)));
}

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

static bool __colorchanger_initialized = 0;

void colorchanger_initialize()
{
	if (!__atomic_test_and_set(&__colorchanger_initialized, 0)) {
		injectionThread = new ColorChangerInjectionThread();
		injectionThread->start();
	}
}
