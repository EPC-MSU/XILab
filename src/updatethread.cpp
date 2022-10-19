#include <time.h>
#include <updatethread.h>
#include <mainwindow.h>

#if defined(WIN32) || defined(WIN64)
#include <Cfgmgr32.h>
#include <Setupapi.h>
#endif

extern QObject* p_mainWnd;
int reset_time_counter = 0;
const int UPDATE_INTERVAL = 100;
const int UPDATE_INTERVAL_BTL = 500;
const int REBOOT_TWONODEVICE_MSECS = 20000; // max time between two "no device" events which triggers ximc_usbser_sys_fix

UpdateThread::UpdateThread(int _number, QObject *parent, char* dev_name, DeviceInterface *_devinterface)
{
	Q_UNUSED(parent)
	number = _number;
	devinterface = _devinterface;
	wait_for_exit = false;
	strcpy(device_name, dev_name);
	QSdev_name = QString(dev_name);
	reconnect_time = 1000;
	
	cs = devinterface->cs; // a shortcut for less typing
}

UpdateThread::~UpdateThread()
{
	delete timer;
}

void UpdateThread::user_update()
{
	measurement_t meas;

	if(cs->updatesEnabled()){		
		if(devinterface->getMode() == FIRMWARE_MODE)
		{
			timer->setInterval(UPDATE_INTERVAL);
			result_t result;
			bool use_calb = cs->useCalibration();
			if (use_calb) {
				calibration_t calibration = cs->calbData();
				result = devinterface->get_status_calb(&meas.status_calb, &calibration);
			} else {
				result = devinterface->get_status(&meas.status);
			}
			if(result == result_nodevice) {
				cs->setConnect(false);
				emit done();
				waitForReconnect(100);
				return;
			} else if (result == result_ok) {
				cs->setConnect(true);
				if (use_calb) {
					cs->setStatusCalbData(meas.status_calb);
				} else {
					cs->setStatusData(meas.status);
				}
			} else {
				emit done();
				// msleep (1000) was here, removed
				return;
			}
			if(!cs->updatesEnabled())
				return;
			if(cs->chartsEnabled() || cs->additionalEnabled()) {
				result = devinterface->get_chart_data(&meas.chart_data);
				if(result == result_nodevice) {
					cs->setConnect(false);
					emit done();
					waitForReconnect(100);
					return;
				} else if (result == result_ok) {
					cs->setConnect(true);
					cs->setChartData(meas.chart_data);
				} else {
					emit done();
					// msleep (1000) was here, removed
					return;
				}

			}
			if(!cs->updatesEnabled())
				return;
			if(cs->analogEnabled()){
				result = devinterface->get_analog_data(&meas.analog_data);
				if(result == result_nodevice) {
					cs->setConnect(false);
					emit done();
					waitForReconnect(100);
					return;
				} else if (result == result_ok) {
					cs->setConnect(true);
					cs->setAnalogData(meas.analog_data);
				} else {
					emit done();
					// msleep (1000) was here, removed
					return;
				}
			}
			if(!cs->updatesEnabled())
				return;
		}
		else if(devinterface->getMode() == BOOTLOADER_MODE)
		{
			timer->setInterval(UPDATE_INTERVAL_BTL);
			result = devinterface->get_status(&meas.status);
			if(result == result_nodevice) {
				cs->setConnect(false);
				emit done();
				waitForReconnect(100);
				return;
			} else {
				cs->setConnect(true);
			}
		}
		else
		{
			waitForReconnect(100);
		}

		emit done();
	}
}

bool UpdateThread::waitForReconnect(int ms)
{
		ms = INT_MAX;

	clock_t t = clock();
	do{
		if(wait_for_exit) {
			ms = 0; // Stop trying to reconnect because we are terminating
		}
		if (devinterface->is_open()) {
			devinterface->close_device();
		}
		qDebug()<<"waiting for reconnect "<<reconnect_time<<" ms";

		cs->setConnect(false);
		emit done();

		msleep(reconnect_time);			// драйверу Luminary требуется время на реализацию своих багов

		// If library can't reconnect, but the usb-com port is there, then reset it [windows only]
		// Only attempt it after 10 retries from the library to avoid race conditions
#if defined(WIN32) || defined(WIN64)
		int i = 0;
		int devIndex;

		SP_DEVINFO_DATA devInfo;
		SP_DEVINFO_LIST_DETAIL_DATA devInfoListDetail;

		char string[MAX_DEVICE_ID_LEN] = "USB\\VID_1CBE&PID_0007&MI_";
		TCHAR wstring[MAX_DEVICE_ID_LEN];
		MultiByteToWideChar(CP_ACP, 0, string, -1,  wstring, MAX_DEVICE_ID_LEN);

		TCHAR devID[MAX_DEVICE_ID_LEN];
		BYTE friendlyName[512];
		QString comport = TruncateDeviceName(device_name);

		SP_PROPCHANGE_PARAMS pcp;
		pcp.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
		pcp.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
		pcp.Scope = DICS_FLAG_CONFIGSPECIFIC;
		pcp.StateChange = DICS_PROPCHANGE;
		pcp.HwProfile = 0;

		GUID guid = {0x4d36e978, 0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}}; // GUID of device class "Ports (COM & USB)"
		HDEVINFO devs = SetupDiGetClassDevsExW(&guid, NULL, NULL, DIGCF_PRESENT, NULL, NULL, NULL);
		devInfoListDetail.cbSize = sizeof(devInfoListDetail);
		if(!SetupDiGetDeviceInfoListDetailW(devs,&devInfoListDetail))
			((MainWindow*)p_mainWnd)->Log("Failed @ SetupDiGetDeviceInfoListDetail", SOURCE_XILAB, 4);
		devInfo.cbSize = sizeof(devInfo);
		for(devIndex=0;SetupDiEnumDeviceInfo(devs,devIndex,&devInfo);devIndex++) {
			if(CM_Get_Device_ID_ExW(devInfo.DevInst,devID,MAX_DEVICE_ID_LEN,0,devInfoListDetail.RemoteMachineHandle)!=CR_SUCCESS) {
				devID[0] = TEXT('\0');
			}
			if (wcsstr(devID, wstring) != NULL) {
				if(SetupDiGetDeviceRegistryPropertyW(devs, &devInfo, SPDRP_FRIENDLYNAME, NULL, friendlyName, sizeof(friendlyName), NULL)) {
					if (QString::fromUtf16((ushort*)friendlyName).contains("("+comport+")")) {
						i++;
						if (reset_time_counter >= 2 || qAbs(QDateTime::currentDateTime().msecsTo(last_wfr_datetime)) < REBOOT_TWONODEVICE_MSECS) {
							reset_time_counter = 0;
							ximc_fix_usbser_sys(comport.toLocal8Bit().constData());
						}
					}
				}
			}
		}
		if (i == 0)
			reset_time_counter = 0;
		else
			reset_time_counter++;

		if(devs != INVALID_HANDLE_VALUE)
			SetupDiDestroyDeviceInfoList(devs);
#endif

		devinterface->open_device(device_name);

	} while(!devinterface->is_open() && ((int) (1000*(clock()-t)/CLOCKS_PER_SEC) < ms));

	last_wfr_datetime = QDateTime::currentDateTime();
	reset_time_counter = 0;
	return devinterface->is_open();
}

void UpdateThread::setReconnectInterval(int timeMs)
{
	reconnect_time = timeMs;
}
