#ifndef PAGEBRAKEWGT_H
#define PAGEBRAKEWGT_H

#include <QtGui/QWidget>
#include <QSettings>

#include <motorsettings.h>
#include <settingsdlg.h>

namespace Ui {
    class PageBrakeWgtClass;
}

class PageBrakeWgt : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PageBrakeWgt)
public:
	explicit PageBrakeWgt(QWidget *parent, MotorSettings *motorStgs);
	
    virtual ~PageBrakeWgt();

    void FromClassToUi();
	void FromUiToClass(MotorSettings *lmStgs);

private:
    Ui::PageBrakeWgtClass *m_ui;
    MotorSettings *mStgs;
};

#endif // PAGEBRAKEWGT_H
