#ifndef DEVICETHREAD_H
#define DEVICETHREAD_H

#include <QThread>
#include <deviceinterface.h>
#include <controllersettings.h>
#include <functions.h>
#include "devicesearchsettings.h"

class DeviceThread: public QThread
{
	Q_OBJECT
public:
	DeviceThread(QObject *parent, DeviceInterface *_devinterface, DeviceSearchSettings* _dss);
	~DeviceThread();

	bool wait_for_exit;

private:
	DeviceInterface *devinterface;
	DeviceSearchSettings *dss;
	QString getPortName(char *name);

	device_t device;
	device_information_t device_info;
	device_enumeration_t dev_enum;
protected:
	QStringList SearchAdapters();
	void run();
signals:
	void finished(bool enum_ok, QStringList names, QStringList descriptions, QStringList friendlyNames, QStringList positionerNames, QList<uint32_t>, QList<Qt::ItemFlags> flags);
};



#endif