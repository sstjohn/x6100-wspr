#include <QStandardItemModel>
#include <QWidget>
#include <QObject>
#include <QLayout>
#include <QtNetwork>
#include <QTableView>

class XWsprWidget : public QWidget 
{
	Q_OBJECT

public:
	XWsprWidget(QWidget *parent = 0);
	~XWsprWidget();
	void scrollTable(bool up);

	QLayout *layout;
	

private:
	QStandardItemModel *wsprStore;
	QLocalServer *wsprReceiver;
	QTableView *qtv;

public Q_SLOTS:
	void wsprReceived(QStringList);
	void wsprConnectionReceived();

};
