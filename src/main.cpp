#include <QtGui/QApplication>
#include <QDebug>
#include <main.h>
#include <startwindow.h>
#include <mainwindow.h>
#include <multiaxis.h>
#include <deviceinterface.h>
#include <Timer.h>
#include <messagelog.h>
#include <loggedfile.h>
#include <status.h>
#include <exception>
#include <stdlib.h>
#include <signal.h>
#include <attenuator.h>
#include <devicesearchsettings.h>

namespace urmc
{
#include <urmc.h>
}

QString xilab_ver = XILAB_VERSION;
Cactus cs;
DeviceInterface *devinterface;
bool init_success;
bool save_configs = true;
QObject* p_mainWnd;
StartWindow* startWnd;
bool first_start = true;
MessageLog *mlog;
QThread *messageLogThread;
bool singleaxis;
const char* valid_manufacturer = "XIMC";
const int retry_open_timeout_ms = 400;

static void XIMC_CALLCONV myCallback(int loglevel, const wchar_t* message, void *user_data)
{
	QString conv = QString::fromWCharArray(message);
	QString conv_1 = QString::fromWCharArray(message);
	QString conv_2 = "receive_synchronized: receive finally timed out";

	if ((QString::compare(conv_1.remove(0, 7), conv_2, Qt::CaseSensitive) != 0) || (startWnd->inited && !startWnd->return_probe()))
	{
		if (p_mainWnd != NULL) {
			if (singleaxis) {
				if (((MainWindow*)p_mainWnd)->inited) {
					mlog->InsertLine(QDateTime::currentDateTime(), conv, SOURCE_LIBRARY, loglevel, ((MainWindow*)p_mainWnd)->settingsDlg->logStgs);
				}
				else {
					mlog->InsertLine(QDateTime::currentDateTime(), conv, SOURCE_LIBRARY, loglevel, NULL);
				}
			}
			else {
				if (((Multiaxis*)p_mainWnd)->inited) {
					mlog->InsertLine(QDateTime::currentDateTime(), conv, SOURCE_LIBRARY, loglevel, ((Multiaxis*)p_mainWnd)->logStgs);
				}
				else {
					mlog->InsertLine(QDateTime::currentDateTime(), conv, SOURCE_LIBRARY, loglevel, NULL);
				}
			}
		}
		else {
			mlog->InsertLine(QDateTime::currentDateTime(), conv, SOURCE_LIBRARY, loglevel, NULL);
		}
	}
}

void retry_open(DeviceInterface *devinterface, const char* device_name, int timeout)
{
	Timer t;
	t.start();
	do {
		devinterface->open_device(device_name);
		if (!devinterface->is_open()) {
			msleep(100);
		}
	}
	while (!devinterface->is_open() && t.getElapsedTimeInMilliSec() < timeout);
}

void signal_handler(int signum)
{
	signal(signum, signal_handler); // reset signal handler
	throw critical_exception("Segmentation fault");
}

size_t glob_size;
void* myalloc (size_t size)
{
	glob_size = size;
	return malloc(size);
}

int main(int argc, char *argv[])
{
	signal(SIGSEGV, signal_handler); // set signal handler to handle SIGSEGV
	QApplication app(argc, argv);	
	QCoreApplication::addLibraryPath(QApplication::applicationDirPath());
#ifdef __APPLE__
	QCoreApplication::addLibraryPath("/usr/lib");
#endif

	devinterface = new DeviceInterface();

	// Feature #17299 - json profile generator
	QStringList args = QCoreApplication::arguments();
	const int notfound = -1;
	int indexConvert = args.indexOf("--convert");
	int indexDevice = args.indexOf("--comport");
	if (indexConvert != notfound && args.size() > indexConvert+1) {
		QString paramConvert = args.at(indexConvert+1);
		QString paramDevice, urmcDevice;
		if (indexDevice != notfound && args.size() > indexDevice+1) {
			paramDevice = QString("xi-com:\\\\.\\COM%1").arg(args.at(indexDevice+1));
		} else { // pick first available device
			device_enumeration_t dev_enum = devinterface->enumerate_devices(ENUMERATE_PROBE, "");
			paramDevice = get_device_name(dev_enum, 0);
			free_enumerate_devices(dev_enum);
		}
		urmcDevice = paramDevice;
		urmcDevice.replace(QRegExp("^xi-") ,"");
		QByteArray qb = paramDevice.toLocal8Bit();
		char* deviceName = qb.data();
		QByteArray qb_urmc = urmcDevice.toLocal8Bit();
		char* urmcName = qb_urmc.data();
		// convert
		retry_open(devinterface, deviceName, retry_open_timeout_ms);
		if (!devinterface->is_open()) {
			QMessageBox::warning(0, "Error", "Failed to open device " + paramDevice + "\nPress OK to exit.");
			exit(0);
		}
		devinterface->close_device();
		// declare local vars
		const char* src_extension = "cfg";
		const char* tgt_extension = "json";
		int profiles_found = 0, profiles_processed = 0, profiles_converted = 0;
		result_t result;
		QString readprofile, writeprofile;
		QString restoreErrors;

		// init stuff
		MotorSettings motorStgs(devinterface);
		StageSettings stageStgs(devinterface);
		QStringList config_filter = QStringList() << QString("*.%1").arg(src_extension);
		QDirIterator it(paramConvert, config_filter, QDir::Files, QDirIterator::IteratorFlag::Subdirectories);
		QDirIterator itcount(paramConvert, config_filter, QDir::Files, QDirIterator::IteratorFlag::Subdirectories);
		// fixme iter copy
		while (itcount.hasNext()) {
			profiles_found++;
			itcount.next();
		}
		urmc::device_t urmc_id = urmc::urmc_open_device(urmcName);
		if (urmc_id == device_undefined) {
			QMessageBox::warning(0, "Error", "Failed to open device " + urmcDevice + " as urmc.\nPress OK to exit.");
			exit(0);
		}
		urmc::urmc_close_device(&urmc_id);

		QProgressDialog progress("Converting...", "Abort", 0, profiles_found, NULL);
		progress.setMinimumDuration(1000);
		progress.show();
		while (it.hasNext()) { // iterate over all profile files
	        if (progress.wasCanceled()) {
				break;
			}
			progress.setValue(profiles_processed);
			QApplication::processEvents();

			// get/set file names
			it.next();
			profiles_processed++;
			writeprofile = readprofile = it.filePath();
			writeprofile.replace(QRegExp(QString(".%1$").arg(src_extension)), QString(".%1").arg(tgt_extension));
			qDebug() << "reading" << readprofile << ", writing" << writeprofile;

			// open with libximc
			devinterface->open_device(deviceName);
			if (!devinterface->is_open()) {
				continue;
			}
			
			// load profile
			XSettings settings(readprofile, QSettings::IniFormat, QIODevice::ReadOnly);
			motorStgs.FromSettingsToClass(&settings, &restoreErrors);
			stageStgs.FromSettingsToClass(&settings, &restoreErrors);

			// apply profile
			motorStgs.FromClassToDevice();
			stageStgs.FromClassToDevice();

			// close with libximc
			devinterface->close_device();

			// open with liburmc
			urmc::device_t urmc_id = urmc::urmc_open_device(urmcName);
			if (urmc_id == device_undefined) {
				continue;
			}

			// urmc_get_profile
			char* buffer = nullptr;
			result = urmc::urmc_get_profile(urmc_id, &buffer, myalloc);

			// close with liburmc
			urmc::urmc_close_device(&urmc_id);

			// save profile and free allocation
			// we were asked to save even if get_profile result was failure, in this case file may contain garbage
			QFile file(writeprofile);
			if (file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
				// we don't write 0 symbol in file
				if (file.write(buffer, glob_size - 1) == glob_size - 1) {
					profiles_converted++;
				}
				file.close();
			}
			free(buffer);
		}
		QMessageBox::information(0, "Info", "Conversion complete.\n"
			"Found " + toStr(profiles_found) + " profiles.\n"
			"Read " + toStr(profiles_processed) + " profiles.\n"
			"Created " + toStr(profiles_converted) + " profiles.\n"
			"Press OK to exit.");
		exit(0);
	}
	// Feature #17299 end

	mlog = new MessageLog();
	messageLogThread = new QThread();
	mlog->moveToThread(messageLogThread);
	messageLogThread->start();

	mlog->setSerial(0);
	mlog->InsertLine(QDateTime::currentDateTime(), "Starting", "main.cpp", 0, NULL);

	LoggedFile file("");
	file.setLog(mlog); // should be called before any operation on logged files

	libximc::set_logging_callback(myCallback, NULL);

	if ( !QDir(getDefaultPath()).exists() ) {

		if ( !QDir().mkdir(getDefaultPath()) ) {
			QMessageBox::warning(0, "Error", "Cannot write to settings directory\n" + getDefaultPath());
			exit(0);
		}

		if (QDir(getOldDefaultPath()).exists() && getOldDefaultPath() != getDefaultPath()){
			// try copy settings from old path

			QDir old_dir(getOldDefaultPath());
			QStringList files_list = old_dir.entryList(QDir::Files);

			QDir new_dir(getDefaultPath());

			QString file_name;
			foreach(file_name, files_list)
			{
				QString src = old_dir.absoluteFilePath(file_name);
				QString trg = new_dir.absoluteFilePath(file_name);
				QFile::copy(src, trg);
			}
		}
	}
	if (!QFile(MakeProgramConfigFilename()).exists()) {
		QFile file(MakeProgramConfigFilename());
		if ( !file.open(QIODevice::ReadWrite) ) {
			QMessageBox::warning(0, "Error", "Cannot write to settings file\n" + file.fileName());
			exit(0);
		}
		file.close();
	}
	//check for Bindy keyfile in user settings dir and copy one from installation dir if there isn't any
	if (!QFile(BindyKeyfileName()).exists()) {
		QFile keyfile(DefaultBindyKeyfileName());
		if ( !keyfile.exists() ) {
			QMessageBox::warning(0, "Error", "Cannot find default keyfile, your Xilab installation is damaged\n");
		} else {
			if ( !keyfile.copy(BindyKeyfileName()) ) {
				QMessageBox::warning(0, "Error", "Cannot write to user settings directory\n");
			}
		}
	}

	/*StartWindow**/ startWnd = new StartWindow();
do {
	init_success = true;
	startWnd->show();
	startWnd->startSearching();
	QList<QString> devices;
	do {
		devices = startWnd->getSelectedDevices();
		QApplication::processEvents();
		msleep(10);
	} while (devices.isEmpty());
	try {
		if (devices.at(0) == "nodevices")
			return 0;
		singleaxis = (devices.size() == 1);
		if (singleaxis) {// One device selected, single axis interface
			retry_open(devinterface, devices.at(0).toLocal8Bit().data(), retry_open_timeout_ms);
			if (!devinterface->is_open()) {
				startWnd->hide();
				throw my_exception("Error calling open_device");
			}
#ifndef SERVICEMODE
			device_information_t dev_info;
			if (devinterface->get_device_information(&dev_info) != result_ok || strcmp(dev_info.Manufacturer, valid_manufacturer) != 0) {
				startWnd->hide();
				devinterface->close_device();
				throw my_exception("Error: device identification failed");
			}
#endif
			QApplication::processEvents();
			MainWindow* mainWnd = new MainWindow(NULL, devices.at(0), devinterface);
			QApplication::processEvents();
			mainWnd->Init();
			p_mainWnd = mainWnd;
			QApplication::processEvents();
		} else { // Multi-axis interface
			QList<DeviceInterface*> ifaces;
			QList<QString> names = devices;
			QList<QString>::iterator i;
			for (i = names.begin(); i != names.end(); ++i) {
				DeviceInterface* iface = new DeviceInterface();
				ifaces.push_back(iface);
				retry_open(iface, (*i).toLocal8Bit().data(), retry_open_timeout_ms);
#ifndef SERVICEMODE
				device_information_t dev_info;
				if (iface->get_device_information(&dev_info) != result_ok || strcmp(dev_info.Manufacturer, valid_manufacturer) != 0) {
					startWnd->hide();
					throw my_exception("Error: devices identification failed");
				}
#endif
				QApplication::processEvents();
			}
			Multiaxis* mainMulti = new Multiaxis(NULL, NULL, names, ifaces);
			p_mainWnd = mainMulti;
		}
	}

	catch (my_exception& e)
	{
		init_success = false;
		first_start = false;
		QMessageBox::warning(0, "An exception has occurred", e.text());
	}
} while (init_success != true);

	if (singleaxis) {
		QObject::connect(((MainWindow*)p_mainWnd), SIGNAL(InsertLineSgn(QDateTime, QString, QString, int, LogSettings*)),
						 mlog,                     SLOT(InsertLine(QDateTime, QString, QString, int, LogSettings*)),
						 Qt::QueuedConnection);
		startWnd->hide();
		((MainWindow*)p_mainWnd)->show();
	} else {
		QObject::connect((Multiaxis*)p_mainWnd, SIGNAL(InsertLineSgn(QDateTime, QString, QString, int, LogSettings*)),
						 mlog,      SLOT(InsertLine(QDateTime, QString, QString, int, LogSettings*)),
						 Qt::QueuedConnection);
		startWnd->hide();
		((Multiaxis*)p_mainWnd)->show(); // although (QObject*) would do just fine
	}

	int result;
	try {
		result = app.exec();
	} catch (critical_exception &e) {
		QMessageBox::warning(0, "An exception has occurred", e.text());
	}
	delete mlog;
	startWnd->~StartWindow();
	return result;
}

QString compileDate()
{
	QDate date(2012, 02, 18);	// default date
	QLocale locale(QLocale::English);
	QDate date1 = locale.toDate(__DATE__, "MMM  d yyyy");
	QDate date2 = locale.toDate(__DATE__, "MMM dd yyyy");
	if (!date1.isNull())
		date = date1;
	else if (!date2.isNull())
		date = date2;
	else
		qDebug() << "impossible date";
	return date.toString("yyyy-MM-dd"); // because ISO 8601
}