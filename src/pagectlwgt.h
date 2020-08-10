#ifndef PAGECTLWGT_H
#define PAGECTLWGT_H

#include <QtGui/QWidget>
#include <QSettings>
#include <controllersettings.h>
#include <motorsettings.h>
#include <settingsdlg.h>
#include <stepspinbox.h>
#include <joycalib.h>

namespace Ui {
    class PageCTLWgtClass;
}

class PageCTLWgt : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PageCTLWgt)
public:
	explicit PageCTLWgt(QWidget *parent, MotorSettings *motorStgs, UpdateThread *_updateThread);
    virtual ~PageCTLWgt();

	void FromClassToUi();
	void FromUiToClass(MotorSettings *lmStgs);

private:
    Ui::PageCTLWgtClass *m_ui;
    MotorSettings *mStgs;
	UpdateThread *updateThread;

	StepSpinBox speedSpin[10];
	QSpinBox timeSpin[9];
	void InitUI();

	JoystickCalibrationDlg *joyDlg;

public slots:
	void modeChanged(int i);
	void OnJoyCalibBtnPressed();
	void receiveJoySettings(int low, int center, int high, float deadzone);
};

#endif // PAGECTLWGT_H
