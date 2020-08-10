#ifndef PAGEUARTWGT_H
#define PAGEUARTWGT_H

#include <QtGui/QWidget>
#include <QSettings>

#include <motorsettings.h>
#include <settingsdlg.h>

namespace Ui {
    class PageUartWgtClass;
}

class PageUartWgt : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PageUartWgt)
public:
	explicit PageUartWgt(QWidget *parent, MotorSettings *motorStgs);
	
    virtual ~PageUartWgt();

	void FromClassToUi();
	void FromUiToClass(MotorSettings *lmStgs);

private:
    Ui::PageUartWgtClass *m_ui;
    MotorSettings *mStgs;
};

#endif // PAGEUARTWGT_H
