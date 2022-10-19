#include <joycalib.h>
#include <mainwindow.h>
#include <functions.h>
#include <QMessageBox>
#define _MATH_DEFINES_DEFINED // math.h conflict with qwt
#include <math.h>

const int deque_size = 50;
const int dz_multiplier = 5; // deadzone multiplier: size of deadzone in stdevs

JoystickCalibrationDlg::JoystickCalibrationDlg(QWidget *parent, DeviceInterface *_devinterface, QString title)
    : QDialog(parent)
{
	devinterface = _devinterface;

	ui.setupUi(this);

	QObject::connect(this,     SIGNAL(finished(int)), this, SLOT(close()));
	QObject::connect(ui.start, SIGNAL(clicked()),     this, SLOT(onStartPressed()));

	QObject::connect(ui.btnOK,     SIGNAL(clicked()), this, SLOT(onOKPressed()));
	QObject::connect(ui.btnApply,  SIGNAL(clicked()), this, SLOT(onApplyPressed()));
	QObject::connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(onCancelPressed()));


	QString wndTitle = "Joystick calibration " + title;
	setWindowTitle(wndTitle);

	zero_pos = min_value = max_value = deadzone_low = deadzone_high = -1;
	deadzone_calb = false;

	dz_slider = new DeadZoneSlider(Qt::Horizontal, this);
	ui.dz_layout->addWidget(dz_slider);
}

JoystickCalibrationDlg::~JoystickCalibrationDlg()
{
//	get_additional_enabled = false;
//	disconnect(this, finished....);
}

void JoystickCalibrationDlg::InitUI()
{

}

void JoystickCalibrationDlg::close()
{

}

void JoystickCalibrationDlg::RefreshWindow()
{
	measurement_t meas;
	meas.chart_data = devinterface->cs->chartData();

//	ui.time->setText(z1.toString());
	ui.joy->setText(toStr(meas.chart_data.Joy));

	int joy = meas.chart_data.Joy;
	msrs.push_back(joy);
	if (msrs.size() > (int)deque_size) {
		msrs.pop_front();
	}

	if (deadzone_calb) {
		if (zero_pos == -1) {
			zero_pos = deadzone_low = deadzone_high = joy;
		}
		else {
			zero_pos = 0;
			stdev = 0;
			int N = static_cast<int>(msrs.size());
			if (N > 0) {
				for (int i=0; i<N; i++) {
					zero_pos += msrs.at(i);
				}
				zero_pos /= N;
				for (int i=0; i<N; i++) {
					stdev += pow((double)(msrs.at(i) - zero_pos), 2);
				}
				stdev = pow((stdev/N), 0.5);

				deadzone_low = zero_pos - dz_multiplier*stdev;
				deadzone_high = zero_pos + dz_multiplier*stdev;
			}
		}
	}
	if (zero_pos != -1)
		ui.zero_pos->setText(toStr(zero_pos));
	if (deadzone_low != -1)
		ui.deadzone_low->setText(toStr(deadzone_low));
	if (deadzone_high != -1)
		ui.deadzone_high->setText(toStr(deadzone_high));

	if (min_value == -1)
		min_value = max_value = joy;
	else {
		min_value = min(min_value, joy);
		max_value = max(max_value, joy);
	}
	ui.min_value->setText(toStr(min_value));
	ui.max_value->setText(toStr(max_value));

	dz_slider->SetWholeRange(min_value, max_value);
	dz_slider->SetDeadZoneRange(deadzone_low, deadzone_high);
	dz_slider->setValue(joy);
	dz_slider->repaint();
	counter--;
	if (deadzone_calb)
		ui.start->setText(toStr(counter));
	else
		ui.start->setText(QString("Start"));
	if (counter == 0) {
		deadzone_calb = false;
		ui.start->setEnabled(true);
	}
}

void JoystickCalibrationDlg::onStartPressed()
{
	counter = deque_size;
	deadzone_calb = true;
	deadzone_low = deadzone_high = zero_pos;
	ui.start->setDisabled(true);
}

void JoystickCalibrationDlg::onOKPressed()
{
	onApplyPressed();
	this->accept();
}

void JoystickCalibrationDlg::onApplyPressed()
{
	if (max_value == zero_pos || min_value == zero_pos)
		emit sendJoySettings(min_value, zero_pos, max_value, 0);
	else
		emit sendJoySettings(min_value, zero_pos, max_value, min((float)255, 100*max(float((zero_pos-deadzone_low))/(zero_pos-min_value), float((deadzone_high-zero_pos))/(max_value-zero_pos))));
}

void JoystickCalibrationDlg::onCancelPressed()
{
	this->reject();
}
