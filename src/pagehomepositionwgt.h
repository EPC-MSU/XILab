#ifndef PAGEHOMEWGT_H
#define PAGEHOMEWGT_H

#include <QtGui/QWidget>
#include <QSettings>
#include <settingsdlg.h>
#include <motorsettings.h>
#include <stepspinbox.h>
#include <infobox.h>

namespace Ui {
    class PageHomePositionWgtClass;
}

class PageHomePositionWgt : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PageHomePositionWgt)
public:
	explicit PageHomePositionWgt(QWidget *parent, MotorSettings *motorStgs);
    virtual ~PageHomePositionWgt();

	void FromUiToClass(MotorSettings *lmStgs);
    void FromClassToUi();

private:
    Ui::PageHomePositionWgtClass *m_ui;
	MotorSettings *mStgs;
};

#endif // PAGEHOMEWGT_H
