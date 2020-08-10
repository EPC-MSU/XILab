#ifndef PAGEBORDERSWGT_H
#define PAGEBORDERSWGT_H

#include <QtGui/QWidget>
#include <QSettings>

#include <motorsettings.h>
#include <settingsdlg.h>
#include <controllersettings.h>
#include <stepspinbox.h>

namespace Ui {
    class PageBordersWgtClass;
}

class PageBordersWgt : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PageBordersWgt)
public:
	explicit PageBordersWgt(QWidget *parent, MotorSettings *motorStgs);
    virtual ~PageBordersWgt();

	void FromClassToUi();
	void FromUiToClass(MotorSettings *lmStgs);

	QString GetBordersType();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::PageBordersWgtClass *m_ui;
    MotorSettings *mStgs;

	void InitUI();
public slots:
    void OnSw1CurChanged(int index);
    void OnSw2CurChanged(int index);
};

#endif // PAGEBORDERSWGT_H
