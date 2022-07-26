#ifndef PAGEDCMOTORWGT_H
#define PAGEDCMOTORWGT_H

#include <QtGui/QWidget>
#include <QSettings>

#include <motorsettings.h>
#include <controllersettings.h>

namespace Ui {
    class PageDCMotorWgtClass;
}

class PageDCMotorWgt : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PageDCMotorWgt)
public:
    explicit PageDCMotorWgt(QWidget *parent, MotorSettings *motorStgs);
    virtual ~PageDCMotorWgt();

	void FromClassToUi(bool _emit=true);
	void FromUiToClass(MotorSettings *lmStgs);
	
	void disableDifferentialFeedback (bool flag);

protected:
    virtual void changeEvent(QEvent *e);
	virtual void showEvent(QShowEvent *e);

private:
    Ui::PageDCMotorWgtClass *m_ui;
    MotorSettings *mStgs;
    void emitOnRadio();

public slots:
	void OnSpeedChkUnchecked();
	void OnEncoderRadioChecked(bool checked);
signals:
	void SgnSwitchCTP(unsigned int new_feedback_type);
};

#endif // PAGEDCMOTORWGT_H
