#ifndef VIRTUALXIMCSERVERTHREAD_H
#define VIRTUALXIMCSERVERTHREAD_H

#include <QThread>
#include <QTimer>

class VirtualXimcServerThread : public QThread
{
	Q_OBJECT
public:
	VirtualXimcServerThread();
	~VirtualXimcServerThread();

	bool wait_for_exit;

protected:
	void run();
};

#endif // VIRTUALXIMCSERVERTHREAD_H
