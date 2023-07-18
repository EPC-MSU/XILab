#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include <QtGui/QDialog>
#include <QtGui>
#include <QCloseEvent>

#include <updatethread.h>
#include <pageborderswgt.h>
#include <pagecyclicwgt.h>
#include <pagelimitswgt.h>
#include <pagelogsettingswgt.h>
#include <pagemotortypewgt.h>
#include <pagedcmotorwgt.h>
#include <pagebldcmotorwgt.h>
#include <pagesteppermotorwgt.h>
#include <pagehomepositionwgt.h>
#include <pageintrfsettingswgt.h>
#include <pagepidwgt.h>
#include <pagettlwgt.h>
#include <pageuartwgt.h>
#include <pageextiowgt.h>
#include <pageslidersetupwgt.h>
#include <pageprogramconfig.h>
#include <pageuiconfigwgt.h>
#include <pageuserunitswgt.h>
#include <pagegraphcommonwgt.h>
#include <pagegraphwgt.h>
#include <pageaboutdevicewgt.h>
#include <pageaboutwgt.h>
#include <pagepositionernamewgt.h>
#include <pagestagewgt.h>
#include <pagestagedcmotorwgt.h>
#include <pagestagegearwgt.h>
#include <pagestageencoderwgt.h>
#include <pagestagehallsensorwgt.h>
#include <pagestageaccessorieswgt.h>
#include <pagepowerwgt.h>
#include <pagebrakewgt.h>
#include <pagectlwgt.h>
#include <pagectpwgt.h>
#include <controllersettings.h>
#include <motorsettings.h>
#include <stagesettings.h>
#include <logsettings.h>
#include <attensettings.h>
#include <xsettings.h>
#ifdef SERVICEMODE
	#include <pagecalibwgt.h>
	#include <pagestep3wgt.h>
	#include <pagedebugwgt.h>
    #include <pagenetsetwgt.h>
#endif

#define TIME_MOVIE 2000

//константы меню
extern int PageRootDeviceConfigurationNum;
extern int PageBordersNum;
extern int PageLimitsNum;
extern int PageMotorTypeNum;
extern int PageDCMotorNum;
extern int PageBLDCMotorNum;
extern int PageStepperMotorNum;
extern int PagePowerNum;
extern int PageBrakeNum;
extern int PageCTLNum;
extern int PageCTPNum;
extern int PageHomePositionNum;
extern int PagePidNum;
extern int PageTtlNum;
extern int PageUartNum;
extern int PageExtioNum;
extern int PageUserUnitsNum;
extern int PageADNum;
extern int PageRootServiceNum;
extern int PageCalibNum;
extern int PageKeyNum;
extern int PageDbgNum;
extern int PageNetworkNum;
extern int PageIntrfSettingsNum;//attenuator skin
extern int PageSliderSetupNum;//general skin
extern int PageCyclicNum;
extern int PageProgramConfigNum;
extern int PageUiConfigNum;
extern int PageLogSettingsNum;
extern int PageGraphNum;
extern int PageGraphPosNum;
extern int PageGraphSpeedNum;
extern int PageGraphEncoderNum;
extern int PageGraphFlagsNum;
extern int PageGraphEnVoltNum;
extern int PageGraphPwrVoltNum;
extern int PageGraphUsbVoltNum;
extern int PageGraphVoltANum;
extern int PageGraphVoltBNum;
extern int PageGraphVoltCNum;
extern int PageGraphEnCurrNum;
extern int PageGraphPwrCurrNum;
extern int PageGraphUsbCurrNum;
extern int PageGraphCurrANum;
extern int PageGraphCurrBNum;
extern int PageGraphCurrCNum;
extern int PageGraphPwmNum;
extern int PageGraphTempNum;
extern int PageGraphJoyNum;
extern int PageGraphPotNum;
extern int PageAboutNum;
extern int PageRootStageConfigurationNum;
extern int PagePositionerNameNum;
extern int PageStageNum;
extern int PageStageDCMotorNum;
extern int PageStageEncoderNum;
extern int PageStageHallsensorNum;
extern int PageStageGearNum;
extern int PageStageAccessoriesNum;

namespace Ui {
    class SettingsDlg;
}

class SettingsDlg : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(SettingsDlg)
public:
    explicit SettingsDlg(QWidget *parent, UpdateThread *_updateThread, DeviceInterface *_devinterface, bool _isMultiaxis);
    virtual ~SettingsDlg();

	QString MakeConfigFilename();

	void FromUiToSettings(QSettings *settings);
	void FromSettingsToUi(QSettings *settings, QSettings *default_stgs = NULL);
	void AllPagesFromUiToSettings(QSettings *settings);
	void AllPagesFromSettingsToUi(QSettings *settings, QSettings *default_stgs = NULL);
	void UpdateCalibration();
	QString getStepFracString(int value1, int value2, bool pad);			//возвращает строку в формате "value1 value2/stepFrac"
	QString getFormattedPosition();
	QString getFormattedSpeed();

    MotorSettings *motorStgs;
	ControllerSettings *controllerStgs;
	CyclicSettings *cyclicStgs;
	AttenSettings *intStgs;
	SliderSettings *sliderStgs;
	LogSettings *logStgs;
	GraphSettings *graphStgs;
	StageSettings *stageStgs;
	UserUnitSettings *uuStgs; // user unit settings class to save to/load from settings
	UserUnitSettings saved_uuStgs; // a copy of user unit settings to compare on settingsdlg close
	UserUnitSettings save_tmp_uuStgs; // a temporary copy of user unit settings to keep from overwriting
	UserUnitSettings prev_tmp_uuStgs; // previous user unit settings for comparison (not saved)
	UserUnitSettings curr_tmp_uuStgs; // current active user unit settings for comparison (not saved)
	DeviceInterface *devinterface;

	QList<QTreeWidgetItem*> treeWgtsLst;
	QList<QWidget*> pageWgtsLst;
	//QMovie movie;

	bool force_save_all;

protected:
    virtual void changeEvent(QEvent *e);
	virtual void showEvent(QShowEvent *e);
	virtual void closeEvent(QCloseEvent *e);
	virtual void hideEvent(QHideEvent *e);

private:
	QList<VersionRange> fw_ok_ranges;
	QList<VersionRange> hw_ok_ranges;
	bool isMultiaxis;
    Ui::SettingsDlg *m_ui;
	unsigned int old_feedback_type;
	QTimer timer3;
    network_settings_t network_settings;
public: // temporary plug for multiaxis ui until device interface class gets obsoleted by libximc syncs and we get rid of it entirely
	UpdateThread *updateThread;
private:
	FirmwareUpdateThread *firmwareUpdate;
	QString default_load_path;
	QString default_save_path;
	QString default_firmware_path;
	QString config_file;
	QString temp_config_file;
	bool firstLoadConfig;			//true - конфиг загружен но не сохранен
    void InitControls();

	void CheckForChanges();
	bool VersionValid(QSettings *settings, version_t v, QString groupname, QList<VersionRange> ok_ranges);
	bool HardwareVersionValid(QSettings *settings, device_information_t device_info);
	bool FirmwareVersionValid(QSettings *settings, firmware_version_t fw_info);
	bool CheckCompatibility(QString xilab_ver, firmware_version_t fw_info);

	void FromUiToClass(MotorSettings* stgs);
	void FromUiToClass(StageSettings* stgs);

	void CheckProfileFwHwVersions(XSettings *settings);
    void LoadNetworkSettings();

public slots:
	void CheckProfileFwHwVersions();
    void OnCurItemChanged(QTreeWidgetItem *curItem, QTreeWidgetItem *prevItem);
    void OnBtnBoxClicked(QAbstractButton * button);
    void OnRestoreFlashBtnClicked();
    void OnSaveFlashBtnClicked();
	void OnSaveFileBtnClicked();
	void OnRestoreFileBtnClicked();
	void OnCompareFileBtnClicked();
    bool AllPagesFromDeviceToClassToUi(bool load_settings = true, bool load_all_settings = true);
    void AllPagesFromUiToClassToDevice();
	void StageSettingsFromDeviceToClassToUi();
	void saveWindowParams();
	
	unsigned int getCurrentMotorType();
	void firmwareUploaded(result_t result);

	void OnPrecedenceChecked (bool checked);
	void motorTypeChanged(unsigned int entype);
	void StepperMotorChanged(unsigned int feedback_type);
	void OnSwitchCTP(unsigned int new_feedback_type);
	void OnChangeUU();
	void OnChangeFrac(unsigned int value);
	void OnNetResult(QString, bool, int);
	void setStyles(bool permanent);
	void setStyles();
	//change main skin general or attenuator
    void SkinTypeChanged(SkinType skin_type);

signals:
	void SgnCyclicBtnEnable(bool on);
	void SgnChangeSettings();
	void SgnChangeCritSettings();
	void SgnHardwareChanged(unsigned int entype);
	void SgnUpdateCalibration(UserUnitSettings uustgs, unsigned int MicrostepMode);
	void SgnLog(QString, QString, int);
	void clickedChangeInterface(AttenSettings, bool correct = true);
};

#endif // SETTINGSDLG_H
