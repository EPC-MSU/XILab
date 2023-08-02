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
const char* valid_manufacturer = "EPC ";
//QMovie movie;

static void XIMC_CALLCONV myCallback(int loglevel, const wchar_t* message, void *user_data)
{
	Q_UNUSED(user_data)
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

int chek_hwmajor(unsigned int major, unsigned int minor)
{
	int ret_messg = QMessageBox::Ok;
	if (major == 2 && minor ==2 )
	{
		
		QMessageBox mes1;
		QLabel lab1, lab2;
		QTextBrowser text1;
		QMovie movie;

		movie.setFileName(":/settingsdlg/images/settingsdlg/warning.gif");

		lab1.setMovie(&movie); // label имеет тип QLabel*
		movie.setSpeed(25);
		movie.setCacheMode(QMovie::CacheAll);

		movie.start();

		lab2.setText(" ");

		lab2.setFixedWidth(50);

		mes1.layout()->addWidget(&lab2);
		mes1.layout()->addWidget(&lab1);
		


		mes1.setText("Warning: This version of mDrive Direct Control added new features that are not present in the 8SMC4 controller. Therefore, this version of mDrive Direct Control is not fully compatible with your 8SMC4 controller. For full compatibility and proper operation of the controller, please install XILab 1.14.12 and firmware 3.9.22 or use the  <html><a href=http://files.xisupport.com/Software.en.html#compatibility-table><span style= text-decoration: underline; color:#0000ff;> compatibility table.</span></a></html>");
		mes1.setInformativeText("If you still want to open the controller, click OK. To exit, click Cancel");
		mes1.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);//

		mes1.setDefaultButton(QMessageBox::NoButton);

		mes1.setIcon(QMessageBox::Warning);


		mes1.show();
		ret_messg = mes1.exec();
	}
	return ret_messg;
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
			QMessageBox::warning(0, "Error", "Cannot find default keyfile, your mDrive Direct Control installation is damaged\n");
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
			retry_open(devinterface, devices.at(0).toLocal8Bit().data(), 400);
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
			if (chek_hwmajor(dev_info.Major, dev_info.Minor) != QMessageBox::Ok)
			{
				return 0;
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
				retry_open(iface, (*i).toLocal8Bit().data(), 400);
#ifndef SERVICEMODE
				device_information_t dev_info;
				if (iface->get_device_information(&dev_info) != result_ok || strcmp(dev_info.Manufacturer, valid_manufacturer) != 0) {
					startWnd->hide();
					throw my_exception("Error: devices identification failed");
				}
				if (chek_hwmajor(dev_info.Major, dev_info.Minor) != QMessageBox::Ok)
				{
					return 0;
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

	int result = 0;
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