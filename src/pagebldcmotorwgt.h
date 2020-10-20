#ifndef PAGEBLDCMOTORWGT_H
#define PAGEBLDCMOTORWGT_H

#include <QtGui/QWidget>
#include <QSettings>

#include <motorsettings.h>
#include <controllersettings.h>

#include "linearmotorwizard.h"

namespace Ui {
    class PageBLDCMotorWgtClass;
}

class PageBLDCMotorWgt : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PageBLDCMotorWgt)
public:
    explicit PageBLDCMotorWgt(QWidget *parent, MotorSettings *motorStgs);
    virtual ~PageBLDCMotorWgt();

	void FromClassToUi(bool _emit=true);
	void FromUiToClass(MotorSettings *lmStgs);

	void disableDifferentialFeedback (bool flag);

protected:
    virtual void changeEvent(QEvent *e);
	virtual void showEvent(QShowEvent *e);

private:
    Ui::PageBLDCMotorWgtClass *m_ui;
    MotorSettings *mStgs;
	LinearMotorWizardDlg linearDlg;

    void emitOnRadio();

	void InitUI();

private slots:
	void OnWizard();

public slots:
	void OnSpeedChkUnchecked();
	void div1000ChkUnchecked();
	void OnEncoderRadioChecked(bool checked);
	void OnLinearMotorSettings(uint32_t, uint32_t);
signals:
	void SgnSwitchCTP(unsigned int new_feedback_type);
};

#endif // PAGEBLDCMOTORWGT_H
