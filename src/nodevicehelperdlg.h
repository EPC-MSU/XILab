#ifndef NODEVICEHELPERDLG_H
#define NODEVICEHELPERDLG_H

#include <QDialog>
//#include <ui_nodevicehelperdlg.h>

namespace Ui {
    class NoDeviceHelperDlgClass;
}

class NoDeviceHelperDlg : public QDialog
{
    Q_OBJECT

public:
    NoDeviceHelperDlg(QWidget *parent, QString title);
    ~NoDeviceHelperDlg();
	void enableLinux();
	void enableWin();
	void enableApple();

private:
	void Init();
	Ui::NoDeviceHelperDlgClass *m_ui;
	QString username;
	void setUdevOk(bool);
	void setGroupOk(bool);
	void checkUdevOk();
	void checkGroupOk();

private slots:
	void onFixGroupPressed();
	void onFixUdevPressed();

};

#endif // NODEVICEHELPERDLG_H
