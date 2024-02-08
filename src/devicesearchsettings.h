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
	QList<std::pair<QString, QString>> scheme_host_pairs;
	unsigned int Virtual_devices;

private:
	QString filename;
};

#endif // DEVICESEARCHSETTINGS_H
