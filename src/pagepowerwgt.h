#ifndef PAGEPOWERWGT_H
#define PAGEPOWERWGT_H

#include <QtGui/QWidget>
#include <QSettings>

#include <motorsettings.h>
#include <settingsdlg.h>

namespace Ui {
    class PagePowerWgtClass;
}

class PagePowerWgt : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PagePowerWgt)
public:
	explicit PagePowerWgt(QWidget *parent, MotorSettings *motorStgs);
	
    virtual ~PagePowerWgt();

	void FromClassToUi();
	void FromUiToClass(MotorSettings *lmStgs);

private:
    Ui::PagePowerWgtClass *m_ui;
    MotorSettings *mStgs;
};

#endif // PAGEPOWERWGT_H
