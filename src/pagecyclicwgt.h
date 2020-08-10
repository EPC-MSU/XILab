#ifndef PAGECYCLICWGT_H
#define PAGECYCLICWGT_H

#include <QtGui/QWidget>
#include <QSettings>
#include <controllersettings.h>
#include <cyclicsettings.h>

namespace Ui {
	class PageCyclicWgtClass;
}

class PageCyclicWgt : public QWidget {
    Q_OBJECT
	Q_DISABLE_COPY(PageCyclicWgt)
public:
    explicit PageCyclicWgt(QWidget *parent, CyclicSettings *cyclicStgs);
    virtual ~PageCyclicWgt();

	void FromUiToClass();
	void FromClassToUi();
	CyclicSettings::UnitType GetCyclicType();
	void FromUiToSettings(QSettings *settings);
	QString FromSettingsToUi(QSettings *settings, QSettings *default_stgs = NULL);

	Ui::PageCyclicWgtClass *m_ui;
private:
	CyclicSettings *cStgs;
};

#endif // PAGECYCLICWGT_H
