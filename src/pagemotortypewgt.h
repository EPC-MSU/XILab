#ifndef PAGEMOTORTYPEWGT_H
#define PAGEMOTORTYPEWGT_H

#include <QtGui/QWidget>
#include <QSettings>

#include <settingsdlg.h>
#include <motorsettings.h>
#include <controllersettings.h>

namespace Ui {
    class PageMotorTypeWgtClass;
}

class PageMotorTypeWgt : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PageMotorTypeWgt)
public:
	explicit PageMotorTypeWgt(QWidget *parent, MotorSettings *motorStgs);
    virtual ~PageMotorTypeWgt();

	void FromClassToUi();
	void FromUiToClass(MotorSettings *lmStgs);
	unsigned int getCurrentMotorType();

	void disableBLDC(bool);

private:
    Ui::PageMotorTypeWgtClass *m_ui;
	MotorSettings *mStgs;
	
private slots:
	void setMotorTypeDC(bool);
	void setMotorTypeStep(bool);
	void setMotorTypeBLDC(bool);

signals:
	void SgnMotorTypeChanged(unsigned int entype);
};

#endif // PAGEMOTORTYPEWGT_H
