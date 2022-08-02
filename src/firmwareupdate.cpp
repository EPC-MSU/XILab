#include <functions.h>
#include <commonvars.h>
#include <firmwareupdate.h>
#include <loggedfile.h>

FirmwareUpdateThread::FirmwareUpdateThread(QObject *parent, UpdateThread *_updateThread, DeviceInterface *_devinterface)
{
	Q_UNUSED(parent)
	devinterface = _devinterface;
	updateThread = _updateThread;
	safe_copy(this->device_name, updateThread->device_name);
}

void FirmwareUpdateThread::run()
{
	devinterface->cs->setUpdatesEnabled(false);
	devinterface->close_device();

	int timeout = INITIAL_TIMEOUT_MS;
	result_t result = result_nodevice;
	while (timeout < MAX_TIMEOUT_MS && result != result_ok) {
		sleep_act(timeout);
		result = devinterface->command_update_firmware(device_name, (unsigned char*)fw_data.data(), fw_data.size());
		timeout = timeout * TIMEOUT_MULTIPLIER;
	}
	if (result == result_ok) {
		sleep_act(BOOTLOADER_DELAY);
	}
	updateThread->waitForReconnect(5000);
	emit update_finished(result);
	devinterface->cs->setUpdatesEnabled(true);
}
