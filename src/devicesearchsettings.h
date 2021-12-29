#ifndef DEVICESEARCHSETTINGS_H
#define DEVICESEARCHSETTINGS_H

#include "xsettings.h"

class DeviceSearchSettings {
public:
	DeviceSearchSettings(QString filename);
    ~DeviceSearchSettings();
	void load();
	void save();

	bool Enumerate_probe;
	bool Enumerate_all_com;
	bool Enumerate_network;
	QList<QString> Server_hosts;
	unsigned int Virtual_devices;
	QList<QString> Protocol_list;

private:
	QString filename;
};

#endif // DEVICESEARCHSETTINGS_H
