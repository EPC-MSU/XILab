#include <devicethread.h>
#include <Timer.h>

DeviceThread::DeviceThread(QObject *parent, DeviceInterface *_devinterface, DeviceSearchSettings* _dss)
{
	devinterface = _devinterface;
	dss = _dss;
	wait_for_exit = false;
}

DeviceThread::~DeviceThread()
{
}

void DeviceThread::run()
{
	int open_flags = 0;
	open_flags += ENUMERATE_PROBE * dss->Enumerate_probe;
	open_flags += ENUMERATE_ALL_COM * dss->Enumerate_all_com;
	open_flags += ENUMERATE_NETWORK * dss->Enumerate_network;
	QString qs;
	if (dss->Server_hosts.size() == 0)
		qs = QString("");
	else {
		qs = QString("addr=");
		for (int i=0; i<dss->Server_hosts.size(); i++) {
			qs.append( dss->Server_hosts.at(i) ).append(",");
		}
		qs.chop(1);
	}
	QByteArray qa = qs.toAscii();
	const char* hints = qa.constData();

	QList<Qt::ItemFlags> flags;
	QStringList urls, descriptions, friendlyNames, positionerNames;
	QList<uint32_t> serials;
	int namesCount;
	std::vector<char*> deviceUrls;
	libximc::set_bindy_key(BindyKeyfileName().toLocal8Bit());
	dev_enum = devinterface->enumerate_devices(open_flags, hints);
	if (dev_enum == NULL) {
		emit finished(false, QStringList(), QStringList(), QStringList(), QStringList(), QList<uint32_t>(), flags);
		return;
	}
	namesCount = get_device_count(dev_enum);
	deviceUrls.resize(namesCount);
	for (int i=0; i<namesCount; i++) {
		deviceUrls.at(i) = get_device_name(dev_enum, i);
	}
	qDebug()<<"Found "<<namesCount<<" devices";

	if(namesCount == 0){
		emit finished(true, QStringList(), QStringList(), QStringList(), QStringList(), QList<uint32_t>(), flags);
		return;
	}

	for (int i = 0; i < namesCount; i++)
	{
		bool device_ok = true;
		if(wait_for_exit) return;

		uint32_t sn;
		///device_information_t device_info;
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
	free_enumerate_devices(dev_enum);
	emit finished(true, urls, descriptions, friendlyNames, positionerNames, serials, flags);
}

QString DeviceThread::getPortName(char *name)
{
	return QString(name).replace(QRegExp("%5C"), QString("/"));
}
