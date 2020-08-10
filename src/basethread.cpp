#include <basethread.h>

void BaseThread::snore(int ms)
{
	this->msleep(ms);
}

void BaseThread::run()
{
	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(update()), Qt::DirectConnection);
	timer->start(100);
	exec();
}

void BaseThread::update()
{
	if (wait_for_exit) {
		QObject::disconnect(this, SLOT(update()));
		timer->stop();
		exit();
	}
	try {
		user_update();
	} catch (...) {
		;
	}

}

void BaseThread::user_update()
{
}