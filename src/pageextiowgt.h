#ifndef PAGEEXTIOWGT_H
#define PAGEEXTIOWGT_H

#include <QtGui/QWidget>
#include <QSettings>

#include <motorsettings.h>
#include <settingsdlg.h>

namespace Ui {
    class PageExtioWgtClass;
}

class PageExtioWgt : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PageExtioWgt)
public:
	explicit PageExtioWgt(QWidget *parent, MotorSettings *motorStgs);
	
    virtual ~PageExtioWgt();

	void FromClassToUi();
	void FromUiToClass(MotorSettings *lmStgs);

private:
    Ui::PageExtioWgtClass *m_ui;
    MotorSettings *mStgs;
};

#endif // PAGEEXTIOWGT_H
