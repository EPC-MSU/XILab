#ifndef JOYCALIB_H
#define JOYCALIB_H

#include <QDialog>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QSettings>
#include <ui_joycalib.h>
#include <deviceinterface.h>
#include <controllersettings.h>
#include "deadzoneslider.h"
#include <deque>

class JoystickCalibrationDlg : public QDialog
{
    Q_OBJECT

public:
    JoystickCalibrationDlg(QWidget *parent, DeviceInterface *_devinterface, QString title);
    ~JoystickCalibrationDlg();

	void InitUI();

signals:
	void sendJoySettings(int, int, int, float);

private:
    Ui::JoystickCalibrationDlgClass ui;
	DeviceInterface *devinterface;
	int zero_pos, min_value, max_value, deadzone_low, deadzone_high, stdev, counter;
	bool deadzone_calb;
	DeadZoneSlider* dz_slider;
	std::deque<int> msrs; // shorthand for measurements

private slots:
	void onStartPressed();
	void onOKPressed();
	void onApplyPressed();
	void onCancelPressed();


public slots:
	void close();
	void RefreshWindow();

};

#endif // JOYCALIB_H
