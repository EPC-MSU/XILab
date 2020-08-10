#ifndef CONTROLLERSETTINGS_H
#define CONTROLLERSETTINGS_H

#if defined(WIN32) || defined(WIN64)
#include <WinSock2.h> // needed to get rid of double includes by msvc
#include <windows.h>
#endif

#include <QtGui>
#include <deviceinterface.h>
#include "version_struct.h"

class ControllerSettings: public QObject
{
	Q_OBJECT
public:
    ControllerSettings(DeviceInterface *_devinterface, QString *dev_name, bool autoLoad = true);
    ~ControllerSettings(void);

	uint32_t serialnumber;
	firmware_version_t firmware_version;
	bootloader_version_t bootloader_version;
	device_information_t device_info;
	bool bootloader_version_loaded;
	device_t *device;
	QString *device_name;

	void LoadControllerSettings(void);
	void LoadAllSettings();

	bool treatAs8SMC5();
	bool isFirmware8SMC4OrYounger();

private:
	DeviceInterface *devinterface;

	bool isFirmware8SMC5();
};

#endif // CONTROLLERSETTINGS_H
