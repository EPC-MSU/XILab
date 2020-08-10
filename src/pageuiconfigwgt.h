#ifndef PAGEUICONFIGWGT_H
#define PAGEUICONFIGWGT_H

#include <QWidget>
#include <attensettings.h>
#include <QSettings>
#include <QDebug>
#include <xsettings.h>
#include <functions.h>


namespace Ui {
class PageUiConfigWgt;
}

class PageUiConfigWgt : public QWidget
{
    Q_OBJECT

public:
    explicit PageUiConfigWgt(QWidget *parent, AttenSettings* intSettings);
    ~PageUiConfigWgt();
    void FromUiToClass();
    void FromClassToUi();
	void FromUiToSettings(QSettings *settings);
	QString FromSettingsToUi(QSettings *settings, QSettings *default_stgs = NULL);
	void SetDisabledAtten(bool set);

private:
    Ui::PageUiConfigWgt* ui;
	AttenSettings *skinSettings;

signals:
    void ChangeInterface(SkinType);

public slots:
    void GeneralChoose(bool choose) { if(choose) emit ChangeInterface(GeneralSkin) ;}
    void AttenuatorChoose(bool choose) { if(choose) emit ChangeInterface(AttenuatorSkin) ;}
    void ChangeInterNeed(SkinType type) { qDebug() << type;}
};


#endif // PAGEUICONFIGWGT_H
