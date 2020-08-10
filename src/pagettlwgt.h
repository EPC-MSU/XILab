#ifndef PAGETTLWGT_H
#define PAGETTLWGT_H

#include <QtGui/QWidget>
#include <QSettings>

#include <motorsettings.h>
#include <controllersettings.h>
#include <stepspinbox.h>

namespace Ui {
    class PageTtlWgtClass;
}

class PageTtlWgt : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PageTtlWgt)
public:
    explicit PageTtlWgt(QWidget *parent, MotorSettings *motorStgs);
    virtual ~PageTtlWgt();

	void FromClassToUi();
	void FromUiToClass(MotorSettings *lmStgs);
	void UpdateControlsVisible();

private:
	Ui::PageTtlWgtClass *m_ui;
    MotorSettings *mStgs;

public slots:
	void OnOneOfTwoChecked();
	void RewriteClutterValue();
	
};

#endif // PAGETTLWGT_H
