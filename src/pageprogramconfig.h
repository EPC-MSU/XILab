#ifndef PAGEPROGRAMCONFIG_H
#define PAGEPROGRAMCONFIG_H

#include <QWidget>
#include <QSettings>
#include <QDebug>
#include <xsettings.h>
#include <functions.h>
#include <devicesearchsettings.h>
#include "bindy.h"

#define XI_NET "" /*"xi-net://"*/
#define XI_UDP "xi-udp://"/*"udp"*/
#define XI_TCP "xi-tcp://"/*"tcp"*/

namespace Ui {
class PageProgramConfigWgt;
}

class PageProgramConfigWgt : public QWidget
{
    Q_OBJECT

public:
    explicit PageProgramConfigWgt(QWidget *parent, DeviceSearchSettings* _dss);
    ~PageProgramConfigWgt();
	void FromUiToClass();
	void FromClassToUi();
	void FromUiToSettings();
	void FromSettingsToUi();

private:
    Ui::PageProgramConfigWgt* ui;
	DeviceSearchSettings* dss;
	void SetTable(QList<std::pair<QString, QString>> scheme_host_pairs);
	QIcon x_icon, w_icon;
	bindy::Bindy *bindy;
	bindy::conn_id_t conn_id;
	bindy::user_vector_t local_users, remote_users;

	void displayStatus (QString text);
	void displayStatusServerCount(uint64_t);
	void refreshUserLists ();

private slots:
	void DetectHosts();
	void slotCellClicked ( int row, int column );
	void slotCellChanged ( int row, int column );
	void copyUsersLocalToRemote();
	void copyUsersRemoteToLocal();
	void deleteUsersLocal();
	void deleteUsersRemote();
	void createUserLocal();
	void createUserRemote();
	void slotSchemeChanged();
};


#endif // PAGEPROGRAMCONFIG_H
