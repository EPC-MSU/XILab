#ifndef CHARTDLG_H
#define CHARTDLG_H

#include <QStatusBar>
#include <QDialog>
#include <QLabel>
#include <QTimer>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QSettings>
#include <qwt_plot_marker.h>
#include <ui_chartdlg.h>
#include <chartlog.h>
#include <graphsettings.h>
#include <updatethread.h>
#include <graphpicker.h>
#include <graphtimeoffset.h>
#include <graphplot.h>
#include <graphplotmulti.h>
#include <graphcommonvars.h>
#include <motorsettings.h>
#include <controllersettings.h>
#include <vector>
#include <settingsdlg.h>
#include <status.h>
#include <myexception.h>

using std::min;
using std::max;

class ChartDlg : public QDialog
{
    Q_OBJECT

public:
    ChartDlg(QWidget *parent, SettingsDlg *settingsDlg, QString title, bool is_viewer);
    ~ChartDlg();

	void InitUI(GraphSettings *graphStgs);
	void FromUiToSettings(QSettings *settings);
	void FromSettingsToUi(QSettings *settings, QSettings *default_stgs = NULL);
	void showAndRespectAutostart();
	void plotsReplot();
	void disableDeprecatedBoxes(bool value);

	int dp;
	QTimer *timer;
	bool enabled;
	bool slider_enabled;
    ChartLog *log;

private:
    Ui::ChartDlgClass ui;
	SettingsDlg *local_settingsDlg;
	MotorSettings *local_motorStgs;
	QScrollBar *scrollBar;
	QVBoxLayout *scrLayout;
	unsigned int local_entype;
	Cactus *local_cs;

	GraphPlot *plots[PLOTSNUM];
	GraphPlotMulti *plotmulti;

	QStatusBar *statusBar;
	QLabel filesizeLbl;
	QLabel stateLbl;
	QLabel fileopenLbl;
	QString default_load_path;
	QString default_save_path;
	int lastWidth;
	QString position_unit;
	QString speed_unit;
	double uu_ratio;
	bool uu_enabled;
	QStringList csvHeader;

	void normMeasures();
	void CommonPlotSetup();
	void AxisUpdate();
	void AlignExtents();
	void ClearExtents();
	bool setBreakLine(double t1, double t2);
	void InitMarker(QwtPlotMarker *mark, double x, double size);
	void ClearPlots(bool first = false);
	void ClearPlot(GraphPlot *plot);
	void ClearPlotMulti(GraphPlotMulti *plot);
	void setScaleDrawMinTime(double min_time);
	void MoveGraph(double time_offset);
	double minTime();
	//void removeData(int n, int all);
	void updatePlotsRightDist();
	void setFileChanged();						//ставит звездочку у имени файла при его изменении
	void LoadFile_v10(QString filename);
	void LoadFile_v11(QString filename);
	void LoadFile_v12(QString filename);
	void LoadFile_v20(QString filename);
	
	std::vector<double> timeData;
	std::vector<double> data[PLOTSNUM];
	std::vector<unsigned int> data_genrflags;
	std::vector<unsigned int> data_gpioflags;
	std::vector<double> data2[DIGNUM];
	QList<unsigned int> flag_list;
	bool plots_visible[PLOTSNUM+1];
	double calc_offset;
	double common_offset;
	double last_offset;
	double summ_offset;
	int break_cnt;
	int delay;
	QwtPlotMarker *markers[100000];
	TList *last_curr;

	int counter;
	int datapoints;
	bool inited;
	bool break_by_dataUpdate;
	bool device_connected;
	bool global_started;
	volatile bool onceshot;
	bool is_viewer;
	QDateTime basetime;
	TList *sliderBegin;
	GraphSettings *gStgs;

	TimeOffset *timeoffset;

	//маркеры для обозначения ограничителей
	QwtPlotMarker speedLimitMarker[2], currLimitMarker, voltLimitMarker[2];

public slots:
	void InsertMeasure();
	void ReinitPlots();
	void ChangePlotCnt();
	void sliderUpdate(int value);
	void sliderPressed();
	void sliderReleased();
	
	void updateDivisor(GraphPlot *plot);
	void updateDivisor(int);

	void updatePlotsVisible();

	void close();
	void OnStartBtnClicked();
	void OnStopBtnClicked();
	void OnViewBtnClicked();
	void OnLoadBtnClicked();
	void OnSaveBtnClicked();
	void OnClearBtnClicked();
	void OnExportBtnClicked();
	void update(int int_offset=-1);

	void updateSettings();
	void setHardware(unsigned int entype);
};

class CheckedFile : public QFile {
public:
	CheckedFile(QString filename) : QFile(filename) { };
	qint64 read(char* data, qint64 maxSize) {
		qint64 read_bytes = QFile::read(data, maxSize);
		if (read_bytes != maxSize)
			throw my_exception("Read error");
		return read_bytes;
	};
};

#endif // CHARTDLG_H
