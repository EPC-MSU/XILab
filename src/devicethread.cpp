#include <QNetworkInterface>
#include <devicethread.h>
#include <Timer.h>

DeviceThread::DeviceThread(QObject *parent, DeviceInterface *_devinterface, DeviceSearchSettings* _dss)
{
	Q_UNUSED(parent)
	devinterface = _devinterface;
	dss = _dss;
	wait_for_exit = false;
}

DeviceThread::~DeviceThread()
{
}

/*
* Getting a list of network interfaces.
*/
QStringList DeviceThread::SearchAdapters()
{
	/*
	 * Take all the interfaces.
	*/
	QList<QNetworkInterface> networkInterfaces = QNetworkInterface::allInterfaces();

	QStringList url;
	for (int i = 0; i < networkInterfaces.size(); i++)
	{
		QFlags<QNetworkInterface::InterfaceFlags> _flags = networkInterfaces.at(i).flags();
		/*
		 * Selection by flags of the working interfaces.
		 */
		if (!(!(QNetworkInterface::IsUp & _flags) || (QNetworkInterface::IsLoopBack & _flags)))		
			/*
			 * Iterate through interface addresses.
			*/
			for (int k = 0; k < networkInterfaces.at(i).addressEntries().size(); k++)			
				/*
				 * Select IP4 addresses.
				*/
				if (networkInterfaces.at(i).addressEntries().at(k).ip().toString().lastIndexOf(".") != -1)				
					url.append(networkInterfaces.at(i).addressEntries().at(k).ip().toString());	
	}
	return url;
}


void DeviceThread::run()
{
	int fl_ch = 1;
	int open_flags = 0;
	open_flags += ENUMERATE_PROBE * dss->Enumerate_probe;
	open_flags += ENUMERATE_ALL_COM * dss->Enumerate_all_com;
	open_flags += ENUMERATE_NETWORK * dss->Enumerate_network;
	QString qs;
	/*
	 * Getting a list of network interfaces.
	*/
	QStringList list_interfase = DeviceThread::SearchAdapters();

	if (dss->Enumerate_network)
	{
		if (dss->scheme_host_pairs.size() == 0)
		{
			qs = QString("addr=");
			/*
			 * Adding the 0th interface to the search and generating the number of search passes.
			*/
			if (list_interfase.size() > 0)
			{
				qs = QString("addr=\nadapter_addr=") + list_interfase.at(0);
				fl_ch = list_interfase.size();
			}
		}
		else {
			/*
			 * Network interface search is not performed when specifying a specific IP address.			 
			*/
			qs = QString("addr=");
			for (int i = 0; i < dss->scheme_host_pairs.size(); i++) {
				if (QString::compare(dss->scheme_host_pairs.at(i).first, QString("xi-udp://"), Qt::CaseInsensitive) == 0)
					qs.append(QString("xi-udp://")).append(dss->scheme_host_pairs.at(i).second).append(",");
				else if (QString::compare(dss->scheme_host_pairs.at(i).first, QString("xi-tcp://"), Qt::CaseInsensitive) == 0)
					qs.append(QString("xi-tcp://")).append(dss->scheme_host_pairs.at(i).second).append(",");
				else
					qs.append(dss->scheme_host_pairs.at(i).second).append(",");
				
			}
			qs.chop(1);
		}
	}
	QByteArray qa = qs.toAscii();

	QList<Qt::ItemFlags> flags;
	QStringList urls, descriptions, friendlyNames, positionerNames;
	QList<uint32_t> serials;
	bool full_enum = false;

	/*
	 * Search cycle for all interfaces.
	*/
	for (int ch = 0; ch < fl_ch; ch++) {
		if (ch>0)
		{
			qs = QString("addr=\nadapter_addr=") + list_interfase.at(ch);
			qa = qs.toAscii();
		}
		int namesCount;
		std::vector<char*> deviceUrls;
		libximc::set_bindy_key(BindyKeyfileName().toLocal8Bit());
		dev_enum = devinterface->enumerate_devices(open_flags, qa.constData());
		if (dev_enum == 0) {
			emit finished(full_enum, urls, descriptions, friendlyNames, positionerNames, serials, flags);
			return;
		}
		namesCount = get_device_count(dev_enum);
		deviceUrls.resize(namesCount);
		for (int i = 0; i < namesCount; i++) {
			deviceUrls.at(i) = get_device_name(dev_enum, i);
		}
		qDebug() << "Found " << namesCount << " devices";

		if (namesCount == 0){
			emit finished(true, urls, descriptions, friendlyNames, positionerNames, serials, flags);
			return;
		}

		for (int i = 0; i < namesCount; i++)
		{
			/*
			 * Checks if the device is not in the list on subsequent passes
			*/
			if (urls.indexOf(QString(deviceUrls[i]), 0) == -1)
			{
				if (wait_for_exit) return;

				uint32_t sn;
				controller_name_t controller_name;
				stage_name_t stage_name;

				if (devinterface->get_enumerate_device_serial(dev_enum, i, &sn) == result_ok) {
					serials.append(sn);
				}
				else {
					serials.append(0);
				}

				if (devinterface->get_enumerate_device_controller_name(dev_enum, i, &controller_name) == result_ok) {
					friendlyNames.append(QString(controller_name.ControllerName));
				}
				else {
					friendlyNames.append("");
				}

				if (devinterface->get_enumerate_device_stage_name(dev_enum, i, &stage_name) == result_ok) {
					positionerNames.append(QString(stage_name.PositionerName));
				}
				else {
					positionerNames.append("");
				}

				flags.append(Qt::NoItemFlags | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
				urls.append(QString(deviceUrls[i]));
				descriptions.append("Description?");
			}

		}
		free_enumerate_devices(dev_enum);
	}

	emit finished(true, urls, descriptions, friendlyNames, positionerNames, serials, flags);
}

QString DeviceThread::getPortName(char *name)
{
	return QString(name).replace(QRegExp("%5C"), QString("/"));
}
