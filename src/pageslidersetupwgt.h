#ifndef PAGESLIDERSETUPWGT_H
#define PAGESLIDERSETUPWGT_H

#include <QtGui/QWidget>
#include <QSettings>
#include <QPushButton>
#include <controllersettings.h>
#include <slidersettings.h>

namespace Ui {
	class PageSliderSetupWgtClass;
}

class PageSliderSetupWgt : public QWidget {
    Q_OBJECT
	Q_DISABLE_COPY(PageSliderSetupWgt)
public:
    explicit PageSliderSetupWgt(QWidget *parent, SliderSettings *sliderStgs);
    virtual ~PageSliderSetupWgt();

	void FromUiToClass();
	void FromClassToUi();
	void FromUiToSettings(QSettings *settings);
	QString FromSettingsToUi(QSettings *settings, QSettings *default_stgs = NULL);

	Ui::PageSliderSetupWgtClass *m_ui;
private:
	SliderSettings *sStgs;
	bool monitor_changes;
signals:
	void SgnChangeValues();
public:
	void SetDisabledPart(bool set);
};

#endif // PAGESLIDERSETUPWGT_H
