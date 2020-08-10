#ifndef PAGEINTRFSETTINGSWGT_H
#define PAGEINTRFSETTINGSWGT_H

#include <QWidget>
#include <QSettings>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <attensettings.h>
#include <QButtonGroup>
#include <QDebug>
#include <QSignalMapper>

namespace Ui {
class PageIntrfSettingsWgt;
}

class PageIntrfSettingsWgt : public QWidget
{
    Q_OBJECT

public:
    const static int one_wheel = 1;
    const static int two_wheels = 2;
    explicit PageIntrfSettingsWgt(QWidget *parent = 0, AttenSettings *intStgs =0);
    ~PageIntrfSettingsWgt();
    void FromUiToClass();
    void FromClassToUi();
    void FromUiToSettings(QSettings *settings);
    QString FromSettingsToUi(QSettings *settings, QSettings *default_stgs = NULL);
signals:
    void clickedInterface(int, AttenSettings*);
public slots:
    void recClickedInterface(int);
    void setDisabled(int);
private:

    Ui::PageIntrfSettingsWgt* ui;
    AttenSettings* intStgs;
    QLabel* trans_label[FILTER_COUNT*WHEEL_COUNT];
    QLineEdit* trans_line[FILTER_COUNT*WHEEL_COUNT];
    QHBoxLayout* vboxHTrans[WHEEL_COUNT];//layout for filters
    QVBoxLayout* vboxVTrans[FILTER_COUNT*WHEEL_COUNT];//vertical layout for filters and lab_filters
    QButtonGroup* wavelength_group;
    QButtonGroup* wheels_group;
    QLabel* titleLabel[WHEEL_COUNT];
    QVBoxLayout* titleBox;
    QSignalMapper* signalMapper;

};

#endif // PAGEINTRFSETTINGSWGT_H
