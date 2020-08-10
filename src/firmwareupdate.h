#ifndef FIRMWAREUPDATE_H
#define FIRMWAREUPDATE_H

#include <QThread>
#include <QFile>
#include <functions.h>
#include <updatethread.h>
#include <infobox.h>
#include <status.h>

#define INITIAL_TIMEOUT_MS 100
#define MAX_TIMEOUT_MS 1600
#define TIMEOUT_MULTIPLIER 2

class FirmwareUpdateThread: public QThread
{
	Q_OBJECT
public:
	FirmwareUpdateThread(QObject *parent, UpdateThread *_updateThread, DeviceInterface *_devinterface);
	
	QByteArray fw_data;

protected:
	void run();
private:
	char device_name[255];
	DeviceInterface *devinterface;
	UpdateThread *updateThread;
signals:
	void update_finished(result_t result);
};


#endif