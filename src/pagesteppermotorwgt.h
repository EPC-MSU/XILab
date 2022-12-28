#ifndef PAGESTEPPERMOTORWGT_H
#define PAGESTEPPERMOTORWGT_H

#include <QtGui/QWidget>
#include <QSettings>
#include <QCheckBox>

#include <motorsettings.h>
#include <stepspinbox.h>

namespace Ui {
    class PageStepperMotorWgtClass;
}

class PageStepperMotorWgt : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PageStepperMotorWgt)
public:
    MotorSettings *mStgs;

	explicit PageStepperMotorWgt(QWidget *parent, MotorSettings *motorStgs);
    virtual ~PageStepperMotorWgt();

	void FromClassToUi(bool _emit=true);
	void FromUiToClass(MotorSettings *lmStgs);

	void disableDifferentialFeedback (bool flag);
	unsigned int OnRadio();

	void stepmodeBoxDisable();
	void stepmodeBoxEnable();

protected:
    virtual void changeEvent(QEvent *e);
	virtual void showEvent(QShowEvent *e);

private:
    Ui::PageStepperMotorWgtClass *m_ui;
    
    void emitOnRadio();
	void conditionalBlockSignals(bool is_emit, bool should_block);

public slots:
	void OnStepModeChanged(int index);
	void OnSpeedChkUnchecked();
	void OnEncoderRadioChecked(bool checked);
	void OnEncoderMediatedRadioChecked(bool checked);
	void OnNoneRadioChecked(bool checked);
	void OnEmfRadioChecked(bool checked);

signals:
	void SgnSwitchCTP(unsigned int new_feedback_type);
	void SgnChangeFrac(unsigned int frac);
	void SgnStepperMotor(unsigned int feedback_type);

};

#endif // PAGESTEPPERMOTORWGT_H
