#include <QThread>

class InjectionThread : public QThread 
{
	Q_OBJECT

private:
	void run() override;

Q_SIGNALS:
	void injectionThreadMoved();
};

class IPythonThread : public QThread 
{
private:
	void run() override;
};
