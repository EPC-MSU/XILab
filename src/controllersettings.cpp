#include <QString>
#include <functions.h>
#include <controllersettings.h>
#include "mainwindow.h"

ControllerSettings::ControllerSettings(DeviceInterface *_devinterface, QString *dev_name, bool autoLoad)
{
	devinterface = _devinterface;
	device_name = dev_name;

	//инициализация переменных
	firmware_version.major = 0;
	firmware_version.minor = 0;
	firmware_version.release = 0;

	bootloader_version_loaded = false;
	bootloader_version.major = 0;
	bootloader_version.minor = 0;
	bootloader_version.release = 0;

	serialnumber = 0;

	if(autoLoad)
		LoadAllSettings();
}

ControllerSettings::~ControllerSettings(void)
{
}

void ControllerSettings::LoadAllSettings(void)
{
	LoadControllerSettings();
}

void ControllerSettings::LoadControllerSettings(void)
{
	result_t result;
	bool saved_state = devinterface->cs->updatesEnabled();
	devinterface->cs->setUpdatesEnabled(false);

	try {
		if(devinterface->getMode() == FIRMWARE_MODE)
		{
			result = devinterface->get_firmware_version(&firmware_version.major, &firmware_version.minor, &firmware_version.release);
			if (result != result_ok) {
				throw my_exception("Error calling get_firmware_version");
			}
			else
			{
				if (firmware_version.major == 0 && firmware_version.minor == 0 && firmware_version.release == 0)
				{
					firmware_version.major = 4;
					firmware_version.minor = 3;
					firmware_version.release = 0;
				}
			}
		}
		if(!bootloader_version_loaded)
		{
			if(devinterface->get_bootloader_version(&bootloader_version.major, &bootloader_version.minor, &bootloader_version.release) == result_ok)
				bootloader_version_loaded = true;
			else {
				throw my_exception("Error calling get_bootloader_version");
			}
		}

		if (devinterface->get_serial_number(&serialnumber) != result_ok) {
			throw my_exception("Error calling get_serial_number");
		}
		if (devinterface->get_device_information(&device_info) != result_ok) {
			throw my_exception("Error calling get_device_information");
		}
	}
	catch (my_exception &e) {
		e.text();
		devinterface->close_device(); // Will trigger NoDevice according to requirements in bug #8220
	}
	// See #4799. For old bootloader which does not support hw version numbers.
	version_t lower = {0, 0, 0};
	version_t higher = {2, 2, 8};
	if (is_three_asc_order(lower, bootloader_version, higher)) {
		device_info.Major = 2;
		device_info.Minor = 3;
		device_info.Release = 0;
	}

	devinterface->cs->setUpdatesEnabled(saved_state);
}

bool ControllerSettings::isFirmware8SMC5()
{
	return (firmware_version.major == 4);
}

bool ControllerSettings::isFirmware8SMC4OrYounger()
{
	return (firmware_version.major < 4);
}

bool ControllerSettings::treatAs8SMC5()
{
	if (strcmp(device_info.Manufacturer, "URMC") == 0)
		return true;
	else
		return isFirmware8SMC5();
}
