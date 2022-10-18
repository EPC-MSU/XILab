#include <devicesearchsettings.h>
#include <xsettings.h>
#include "ximc.h"

DeviceSearchSettings::DeviceSearchSettings(QString _filename)
{
	this->filename = _filename;
	load();
}

DeviceSearchSettings::~DeviceSearchSettings()
{
}

void DeviceSearchSettings::load()
{
	XSettings settings(filename, QSettings::IniFormat, QIODevice::ReadOnly);

	settings.beginGroup("Device_search_options");
	Enumerate_probe = settings.value("Enumerate_probe", true).toBool();
	Enumerate_all_com = settings.value("Enumerate_all_com", false).toBool();
	Enumerate_network = settings.value("Enumerate_network", false).toBool();

	int size = 0;
	int sizehosts = settings.beginReadArray("Server_hosts");
	settings.endArray();
	int sizeprotocol = settings.beginReadArray("Protocol_list");
	settings.endArray();
	if (sizehosts > sizeprotocol) size = sizeprotocol;
	else size = sizehosts;

	settings.beginReadArray("Server_hosts");

	Server_hosts.clear();
	for (int i = 0; i < size; i++) {
		settings.setArrayIndex(i);
		QString host = settings.value("hostname").toString();
		Server_hosts.append(host);
	}
	settings.endArray();

	settings.beginReadArray("Protocol_list");

	Protocol_list.clear();
	for (int i = 0; i < size; i++) {
		settings.setArrayIndex(i);
		QString protocol = settings.value("protocol").toString();
		Protocol_list.append(protocol);
	}
	settings.endArray();

	Virtual_devices = settings.value("Virtual_devices", 2).toUInt();
	settings.endGroup();
}

void DeviceSearchSettings::save()
{
	XSettings settings(filename, QSettings::IniFormat, QIODevice::ReadWrite);

	settings.beginGroup("Device_search_options");
	settings.setValue("Enumerate_probe", Enumerate_probe);
	settings.setValue("Enumerate_all_com", Enumerate_all_com);
	settings.setValue("Enumerate_network", Enumerate_network);

	settings.remove("Server_hosts");
	settings.beginWriteArray("Server_hosts");
	for (int i = 0; i < Server_hosts.size(); i++) {
		settings.setArrayIndex(i);
		settings.setValue("hostname", Server_hosts.at(i));
	}
	settings.endArray();

	settings.remove("Protocol_list");
	settings.beginWriteArray("Protocol_list");
	for (int i = 0; i < Protocol_list.size(); i++) {
		settings.setArrayIndex(i);
		settings.setValue("protocol", Protocol_list.at(i));
	}
	settings.endArray();

	settings.setValue("Virtual_devices", Virtual_devices);
	settings.endGroup();
}
