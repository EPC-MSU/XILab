#ifndef PAGEDEBUGWGT_H
#define PAGEDEBUGWGT_H

#include <QtGui/QWidget>
#include <QSettings>

#include <motorsettings.h>
#include <settingsdlg.h>

namespace Ui {
    class PageDebugWgtClass;
}

class PageDebugWgt : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PageDebugWgt)
public:
	explicit PageDebugWgt(QWidget *parent, MotorSettings *motorStgs, char* dev_name, DeviceInterface *_devinterface);
	
    virtual ~PageDebugWgt();

private slots:
	void OnSpos();
	void OnGpos();
	void OnChmt();
	void OnCget();
	void OnSnvm();
	void OnGnvm();
	void OnLoft();
	void OnPwof();
	void OnEesv();
	void OnEerd();
	void OnRest();
	void OnClfr();
	void OnFix();
	void OnDbgr();
	void OnDbgw();
	void OnCopy();
	void redraw();
	void OnPidCalibration();

signals:
	void EESV();
	void EERD();

private:
    Ui::PageDebugWgtClass *m_ui;
    MotorSettings *mStgs;
	debug_read_t debug_read;
	debug_write_t debug_write;
	char chars[128];
	DeviceInterface* devinterface;
	char device_name[255];
};

#endif // PAGEDEBUGWGT_H
