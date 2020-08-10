#ifndef PAGELOGSETTINGSWGT_H
#define PAGELOGSETTINGSWGT_H

#include <QtGui/QWidget>
#include <QSettings>
#include <QPushButton>
#include <controllersettings.h>
#include <logsettings.h>

namespace Ui {
	class PageLogSettingsWgtClass;
}

class PageLogSettingsWgt : public QWidget {
    Q_OBJECT
	Q_DISABLE_COPY(PageLogSettingsWgt)
public:
    explicit PageLogSettingsWgt(QWidget *parent, LogSettings *logStgs);
    virtual ~PageLogSettingsWgt();

	void FromUiToClass();
	void FromClassToUi();
	void FromUiToSettings(QSettings *settings);
	QString FromSettingsToUi(QSettings *settings, QSettings *default_stgs = NULL);

	Ui::PageLogSettingsWgtClass *m_ui;
private:
	LogSettings *lStgs;
};

#endif // PAGELOGSETTINGSWGT_H
