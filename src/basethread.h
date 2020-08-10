#ifndef BASETHREAD_H
#define BASETHREAD_H

#include <QThread>
#include <QTimer>

class BaseThread: public QThread
{
	Q_OBJECT
public:
	
	bool wait_for_exit;
	virtual void user_update();
	void snore(int ms);

protected:
	void run();
	QTimer *timer;

private slots:
	void update();

};


#endif