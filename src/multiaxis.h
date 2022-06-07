#ifndef MULTIAXIS_H
#define MULTIAXIS_H

#include <QtGui/QWidget>
#include "ui_multiaxis.h"
#include <deviceinterface.h>

#include <settingsdlg.h>
#include <updatethread.h>
#include "main.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_canvas.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_rescaler.h>
#include <qwt_picker_machine.h>
#include <qwt_scale_widget.h>
#include <dimunits.h>
#include <functions.h>
#include <scriptdlg.h>
#include <scriptthreadmulti.h>
#include <logdlg.h>
#include <Timer.h>
#include <qwt_compat.h>
#include <status.h>
#include <xsettings.h>
#include <devicesearchsettings.h>

const int max_devices = 3;

struct Point {
	double x;
	double y;
	QDateTime t;
};

class Multiaxis : public QWidget
{
	Q_OBJECT

public:
	Multiaxis(QWidget *parent /*= 0*/, Qt::WFlags flags /*= 0*/, QList<QString> _device_names, QList<DeviceInterface*> _devinterfaces);
	~Multiaxis();

	device_t device[max_devices];
	char device_name[max_devices][256];
	ScriptDlg* scriptDlg;
	LogDlg* logDlg;
	LogSettings* logStgs;
	bool inited;

	QStringList getSerials();
	QString getSerialsSortedConcat();
signals:
	void InsertLineSgn(QDateTime, QString, QString, int, LogSettings*);

public slots:
	void Log(QString string, QString source, int loglevel);
	void ScriptExceptionLog(QString message);
	void UpdateState();

private slots:
	void OnStopBtnPressed();
	void OnSstpBtnPressed();
	void OnMoveBtnPressed();
	void OnShiftBtnPressed();
	void OnHomeBtnPressed();
	void OnZeroBtnPressed();
	void OnExitBtnPressed();
	void OnScriptStartBtnPressed();
	void OnScriptStopBtnPressed();
	void OnScriptStopBtnPressed(bool);
	void CheckForceStopCondition();
	void OnScriptingBtnPressed();
	void OnLogBtnPressed();
	void OnClearLogBtnPressed();
	void OnCalibrationBtnPressed();
	void OnSettings1BtnClicked();
	void OnSettings2BtnClicked();
	void OnSettings3BtnClicked();
	void OnPickerXYPointSelected(const QPointF &);
	void OnPickerZPointSelected(const QPointF &);
	void receiveSerials(QStringList serials, double delay);
	void rescalePlots();

protected:
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);
	bool eventFilter(QObject *obj, QEvent *event);

signals:
	void SgnDisableStagePages(bool set, int number);

private:
	void UpdateLogTable();
	void InitTable();
	QStringList headerLabels;
	void SaveAxisConfig();
	void LoadSingleConfigs();
	void LoadLineEraseDelay();
	void LoadSingleConfigs1();
	void SaveSingleConfigs();
	void LoadConfigs();
	void SaveConfigs();
	void FromUiToSettings(QSettings *settings);
	void FromSettingsToUi(QSettings *settings);
	void closeEvent(QCloseEvent *event);
	Timer t;
	int translate(int num);
	int tr_matrix[max_devices];
	std::map<unsigned int, bool> previousStateIsAlarm;

	DimensionalUnitsDlg *unitsDlg;
	PageLogSettingsWgt* pagelog;

	Ui::multiaxisClass ui;

	UpdateThread* updateThread;
	ScriptThreadMulti* scriptThreadMulti;

	QPushButton* scriptStartBtn;
	QPushButton* scriptStopBtn;
	QPushButton* scriptLoadBtn;
	QPushButton* scriptSaveBtn;
	QTextEdit *codeEdit;
	QTableWidget *logEdit;
	QList<device_t> devices;
//	QList<QString> device_names;
	QList<DeviceInterface*> devinterfaces;
	QList<SettingsDlg*> settingsDlgs;
	int devcount;

	QList<QLabel*> curPoss;
	QList<QLabel*> tableLabel;
	QList<StepSpinBox*> moveTos, shiftOns;
	QList<QPushButton*> joyDecs, joyIncs;
	QList<QGroupBox*> groupBoxs;
	QList<QLabel*> voltageValues, currentValues, speedValues, commandValues, powerValues;
	QList<QwtPlot::Axis> axes;
	QList<QwtPlot*> plots;

	QColor saved_bk_color;
	QColor alarm_bk_color;
	QFont font;
	int counter;
	double LeftBorder[max_devices];
	double RightBorder[max_devices];
	int edge_width;

	bool positionerNeedsUpdate[max_devices];
	bool noDevice[max_devices];
	bool exiting;

	InfoBox *exitBox;

	QwtPlot *plotXY, *plotZ;
	QwtPlotCurve *dotXY, *dotZ;
	QwtPlotCurve *tgtXY, *tgtZ;

	QwtPlotCurve *trace;
	std::vector<double> x_array, y_array;
	std::vector<QDateTime> t_array;
	std::deque<Point> points;
	double saved_x, saved_y;
	double line_erase_delay;

	QList<QwtPlotMarker*> markmin, markmax, mark;
	QPalette palette_green;
};

#endif // MULTIAXIS_H
