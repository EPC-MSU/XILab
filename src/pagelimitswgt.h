#ifndef PAGELIMITSWGT_H
#define PAGELIMITSWGT_H

#include <QtGui/QWidget>
#include <QSettings>

#include <motorsettings.h>
#include <controllersettings.h>

namespace Ui {
    class PageLimitsWgtClass;
}

class PageLimitsWgt : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PageLimitsWgt)
public:
    explicit PageLimitsWgt(QWidget *parent, MotorSettings *motorStgs);
    virtual ~PageLimitsWgt();

	void disableCriticalUSB(bool value);

	void FromClassToUi();
	void FromUiToClass(MotorSettings *lmStgs);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::PageLimitsWgtClass *m_ui;

    MotorSettings *mStgs;
};

#endif // PAGELIMITSWGT_H
