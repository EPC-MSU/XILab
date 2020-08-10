#ifndef DIMUNITS_H
#define DIMUNITS_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QSettings>
#include <ui_dimunits.h>

class DimensionalUnitsDlg : public QDialog
{
    Q_OBJECT

public:
    DimensionalUnitsDlg(QWidget *parent, QString title, QStringList serials, double delay);
    ~DimensionalUnitsDlg();
	void insertWidget(QWidget *widget);

signals:
	void sendSerials(QStringList, double);

private:
	Ui::DimensionalUnitsDlgClass ui;

private slots:
	void onOKPressed();
	void onCancelPressed();
	void onBtnUpPressed();
	void onBtnDownPressed();

};

#endif // DIMUNITS_H
