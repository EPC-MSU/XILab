#ifndef PAGECTPWGT_H
#define PAGECTPWGT_H

#include <QtGui/QWidget>
#include <QSettings>

#include <motorsettings.h>
#include <settingsdlg.h>

namespace Ui {
    class PageControlPositionWgtClass;
}

class PageControlPositionWgt : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PageControlPositionWgt)
public:
	explicit PageControlPositionWgt(QWidget *parent, MotorSettings *motorStgs);
    virtual ~PageControlPositionWgt();

    void FromClassToUi();
	void FromUiToClass(MotorSettings *lmStgs);
	void setPartEnabled(bool enable);

private:
    Ui::PageControlPositionWgtClass *m_ui;
    MotorSettings *mStgs;
};

#endif // PAGECTPWGT_H
