#ifndef PAGEPIDWGT_H
#define PAGEPIDWGT_H

#include <QtGui/QWidget>
#include <QSettings>

#include <motorsettings.h>
#include <deviceinterface.h>

#include "pidcalibratorwgt.h"

namespace Ui {
    class PagePidWgtClass;
}

class PagePidWgt : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PagePidWgt)
public:
    explicit PagePidWgt(QWidget *parent, MotorSettings *motorStgs, DeviceInterface *_devinterface, uint32_t _serial);
    virtual ~PagePidWgt();

	virtual bool eventFilter(QObject *object, QEvent *e);

	void FromClassToUi();
	void FromUiToClass(MotorSettings *lmStgs);
	void SetMotorType(unsigned int entype);

public slots:
	void setPIDSettings(float Kpf, float Kif, float Kdf, int Kp, int Ki, int Kd);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::PagePidWgtClass *m_ui;
    MotorSettings *mStgs;

	PIDCalibratorWgt calibrator_window;
	DeviceInterface *devface;
	void validate(void);
private slots:
	void onPIDTune(); // нажатие кнопки PIDTune

signals:
	void PIDTuneShowed(); // появление окна PIDTune
	void Apply();        // применить настройки. См. #20642-11
};

#endif // PAGEPIDWGT_H
