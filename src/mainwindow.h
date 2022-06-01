#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QTimer>
#include <QDateTime>
#include <QMenuBar>
#include <qwt_slider.h>
#include <deviceinterface.h>
#include <stepspinbox.h>
#include <controllersettings.h>
#include <settingsdlg.h>
#include <chartdlg.h>
#include <scriptdlg.h>
#include <updatethread.h>
#include <infobox.h>
#include <scriptthread.h>
#include <messagelog.h>
#include <Timer.h>
#include <QString>
#include <myexception.h>
#include <attenuator.h>

#define MESSAGE_TIMEOUT		2000
#define ST_PANE_WIDTH		50
#define ST_BAR_WIDTH		10
enum CyclicState {
	CYCLIC_LEFT = 1,
	CYCLIC_RIGHT = 2,
};

enum State_Atten {
	IDLE = 1,
	WAIT_MOVE1 = 2,
	WAIT_STOP1 = 3,
	WAIT_MOVE2 = 4,
	WAIT_STOP2 = 5,
	MOVE_ERROR = 6,
	START_CALIBRATE = 9,
	WAIT_CALIBRATE = 10,
	CONTINUE_CALIBRATE = 11,
};

namespace Ui
{
    class MainWindowClass;
}

// Установкой делителя divisor можно управлять цифрами на шкале слайдера
class SliderScaleDraw: public QwtScaleDraw
{
public:
    SliderScaleDraw(double _divisor)
    {
		divisor = _divisor;
    }
	void setDivisor(double new_divisor)
	{
		divisor = new_divisor;
	}
    virtual QwtText label(double v) const
    {
		QString s;
		s.setNum(v/divisor);
		return s;
    }
private:
	double divisor;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent, QString _device_name, DeviceInterface *_devinterface);
    ~MainWindow();

    void InitUI();
	void Init();
    void ClearState(int device_mode = FIRMWARE_MODE);
	void NormalState();

    device_t device;
    char device_name[256];
    UpdateThread *updateThread;
	ScriptThread *scriptThread;

	ScriptDlg *scriptDlg;
	SettingsDlg *settingsDlg;
	bool inited;				//флаг, определяющий было ли инициализирована программа (если нет - при закрытии не сохранять данные)

private:
	
	DeviceInterface* devinterface;
	Cactus *cs;
    Ui::MainWindowClass *ui;
	QMenuBar *menuBar;
    ChartDlg *chartDlg;
	SliderScaleDraw *sliderScaleDraw;
	Attenuator* attenuator;
	void slider_borders(double left, double right, double position, double *slider_left, double *slider_right);
	void LoadConfigs(QString filename);
	void SaveConfigs(QString filename);
	void LoadProgramConfigs(bool controls_inited);
	void SaveProgramConfigs();
	void FromUiToSettings(QSettings *settings);
	void FromSettingsToUi(QSettings *settings, QSettings *default_stgs = NULL);
	void SetLblDivisor(double div);
	void CyclicMotion();
	void AttenuatorMotion();
	void UpdateLogTable();
	void InitTable();
	long long getMicroposition(libximc::status_t status);
	result_t doMove(double target, bool relative = false);
	bool isNear(double pos1, double pos2);

    void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);
	bool eventFilter(QObject *object, QEvent *event);

	InfoBox * exitBox;
	InfoBox infoBox;

	StepSpinBox moveSpinBox, shiftSpinBox;

	double slider_divisor;

    QPixmap onPix;
    QPixmap offPix;
	QPixmap redPix;
	QPixmap cyclicPix;
	QPixmap cyclic_offPix;

	QPalette palette_red;
	QPalette palette_grey;
	QPalette palette_green;
	QPalette palette_yellow;

	#define INDICATOR_COUNT 7
	QPalette palette_rainbow[INDICATOR_COUNT];
	struct {
		QString name;
		unsigned int flag;
	} btn_list[INDICATOR_COUNT];

	QColor saved_bk_color;
	QColor alarm_bk_color;

    QLabel errcLbl;
    QLabel errdLbl;
	QLabel errvLbl;
	QLabel homdLbl;
	QLabel slipLbl;
	QLabel pwhtLbl;	
	QLabel wrmLbl;
	QLabel engLbl;
	QLabel extLbl;

	QLabel btns[INDICATOR_COUNT];
		
	QLabel eeprLbl;
	QLabel wnd1Lbl;
	QLabel wnd2Lbl;
	QLabel encdLbl;
	QLabel ctblLbl;
	

	bool cyclic;
	CyclicSettings::UnitType prev_cyclic_state;
	bool move_to_left_bdr;
	bool move_to_right_bdr;

	bool encoder_advertised;
	bool h_bridge_complained;

	int last_cyclic_move;

	bool BackToNormalState; //флаг для восстановления нормального состояния кнопок в главном окне (после возвращения из неактивного режима)
	bool exiting;

	bool positionerNeedsUpdate;
	bool deviceLost;
	bool previousStateIsAlarm;

	//attenuator position
	State_Atten state_atten;
	int posit_steps_x;//new position of second wheel [0,200]
	int posit_steps_y;//new position of first wheel [0,200]
	int prev_Pos_x;
	int prev_Pos_y;
	int prev_steps_y;//last move steps of first wheel
	//long long current_microposition, target_microposition;
    double current_position, target_position;

	//положение реальных границ и текущие границы на слайдере
	double slider_left;
	double slider_right;
	double left_border;
	double right_border;

	int temp;

	QPushButton* scriptStartBtn;
	QPushButton* scriptStopBtn;
	QPushButton* scriptLoadBtn;
	QPushButton* scriptSaveBtn;
	QTextEdit *codeEdit;

	QStringList headerLabels;
	Timer t;

protected:
	void closeEvent(QCloseEvent *event);

signals:
	void InsertLineSgn(QDateTime, QString, QString, int, LogSettings*);
	void SgnDisableStagePages(bool set);

public slots:
	void error_from_atten(const QString& );//user enter bad transmittance
	void AttenMove();//change position when button "MOVE" clicked
	void AttenCalibrate(); // do "HOME" then "ZERO" when "Calibrate" button is clicked in attenuator interface
	void changeInterface(AttenSettings IntSet, bool correct = true);//General or Attenuator skin

	void UpdateState();
	void LoadPosition();
	void Log(QString string, QString source, int loglevel);
	void SetCyclicEnable(bool on);
	void critSettingsChanged();
	void CommandErrorLog(const char* message, result_t result);
	void CommandWarningLog(const char* message, result_t result);
	void ScriptExceptionLog(QString message);

    void OnSettingsBtnPressed();
    void OnChartBtnPressed();
	void OnCyclicBtnPressed();
	void OnScriptBtnPressed();
	void OnScriptStartBtnPressed();
	void OnScriptStopBtnPressed(); // a wrapper for dialog close signal, calls OnScriptStopBtnPressed(false)
	void OnScriptStopBtnPressed(bool hardstop);
	void CheckForceStopCondition();
	void OnTechSupportClicked();

    void OnLeftBdrBtnPressed();
    void OnLeftBtnPressed();
    void OnStopBtnPressed();
    void OnSstpBtnPressed();
    void OnRightBtnPressed();
    void OnRightBdrBtnPressed();
    void OnMoveBtnPressed();
    void OnShiftBtnPressed();

	void OnGoHomeBtnPressed();
	void stopByCancel();
    void OnZeroBtnPressed(QString side = "");
	void OnClearLogBtnPressed();
	void OnSaveLogBtnPressed();
    void OnExitBtnPressed();

	void makeNewInstance();

	void OnPIDCalibratorShow();
};

#endif // MAINWINDOW_H
