import IPython
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *

top_level_widget = None
qinj = None

def message_box(text = "Hello, world!\n\nPress MFK to dismiss."):
	_setup()
	QMetaObject.invokeMethod(
		qinj,
		"messageBoxRequested",
		Q_ARG(str, text)
	)

def _setup():
	global top_level_widget, qinj
	if not top_level_widget:
		top_level_widget = QApplication.instance().topLevelWidgets()[-1]
	if not qinj:
		qinj = top_level_widget.findChild(QObject, "qinj")

def run():
	_setup()
	IPython.embed(header="qinj console")
