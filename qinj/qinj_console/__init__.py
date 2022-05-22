import IPython
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *

def message_box(text = "Hello, world!\n\nPress MFK to dismiss."):
	top_level_widget = QApplication.topLevelWidgets()[-1]
	
	#what is the correct way to do this?
	injection = top_level_widget.children()[-3]

	QMetaObject.invokeMethod(
		injection,
		"messageBoxRequested",
		Q_ARG(str, text)
	)

def run():
	IPython.embed(header="qinj console")
