#ifndef PAGEABOUTDEVICEWGT_H
#define PAGEABOUTDEVICEWGT_H

#include <QtGui/QWidget>
#include <QtGui/QDialog>
#include <QFileDialog>
#include <QFile>
#include <QString>
#include <QThread>
#include <deviceinterface.h>
#include <controllersettings.h>
#include <firmwareupdate.h>
#include <functions.h>
#include <infobox.h>
#include <motorsettings.h>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QCryptographicHash>
#include <tinyxml2.h>
#include <QStringList>
#include <choosefirmware.h>
#include <QNetworkConfigurationManager>

namespace Ui {
    class PageAboutDeviceWgtClass;
}

class PageAboutDeviceWgt : public QWidget{
	Q_OBJECT
    Q_DISABLE_COPY(PageAboutDeviceWgt)
public:
	PageAboutDeviceWgt(QWidget *parent, MotorSettings *_motorStgs, ControllerSettings *controllerStgs_loc, FirmwareUpdateThread *firmwareUpdate_loc, DeviceInterface *_devinterface, QString *_default_firmware_path);
    ~PageAboutDeviceWgt();

	void FromClassToUi();
	void FromUiToClass(MotorSettings *lmStgs);

private:
	ChooseFirmware* choosefirmwaredlg;
	DeviceInterface* devinterface;
    Ui::PageAboutDeviceWgtClass *m_ui;
	ControllerSettings *controllerStgs_local;
	FirmwareUpdateThread *firmwareUpdate_local;
	MotorSettings *motorStgs;
	InfoBox infoBox;
	QString *default_firmware_path;
	bool isDefaultLocation;

	QNetworkReply *reply;
	QNetworkAccessManager *manager;
	QUrl firmware_url;
	QString hash_string;
	void FwUpdateRoutine(QByteArray data);
	QDateTime download_start_time;
	QString getServiceMailInfo(const QString line_sep);

public slots:
	void OnUpdateFirmwareBtnClicked();
	void OnTechSupportClicked();
	void OnUpdateFirmwareFinished(result_t result);
	void OnUpdateSerialFinished();
	void defaultLocationChanged(bool isDefault);
	void RetryInternetConn(QUrl resource_address);
signals:
	void precedenceCheckedSgn(bool checked);
	void errorInLog(QString, QString, int);
	void networkResultSgn(QString, bool, int);
private slots:
	void slotReadyRead();
	void slotFinished(QNetworkReply* reply);
	void slotProgress(qint64 bytesReceived, qint64 bytesTotal);
	void showProgress();
	void hideProgress();
	void hideProgressHelper();
	void slotError(QNetworkReply::NetworkError);
	void OnUpdateFirmwareInternetBtnClicked();
	void CallChooseFirmDlg();
	void updateChooseFirmware(QString, QString);
};

#endif // PAGEABOUTDEVICEWGT_H
