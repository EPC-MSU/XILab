#ifndef PAGEGRAPHSETUPWGT_H
#define PAGEGRAPHSETUPWGT_H

#include <QtGui/QWidget>
#include <QSettings>

#include <graphsettings.h>

namespace Ui {
	class PageGraphCommonWgtClass;
}

//Common
class PageGraphCommonWgt : public QWidget {
    Q_OBJECT
	Q_DISABLE_COPY(PageGraphCommonWgt)
public:
    explicit PageGraphCommonWgt(QWidget *parent, GraphCommonSettings *gcommonStgs);
    virtual ~PageGraphCommonWgt();

	void FromUiToClass();
	void FromClassToUi();
	void FromUiToSettings(QSettings *settings);
	QString FromSettingsToUi(QSettings *settings, QSettings *default_stgs = NULL);

	Ui::PageGraphCommonWgtClass *m_ui;
private:
	GraphCommonSettings *gcomStgs;
};

#endif // PAGEGRAPHSETUPWGT_H
