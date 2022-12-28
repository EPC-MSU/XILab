#include <ui_settingsdlg.h>
#include <ui_pagesteppermotorwgt.h>
#include <ctime>
#include <QSettings>
#include <QMessageBox>
#include <mainwindow.h>
#include <settingsdlg.h>
#include <cyclicsettings.h>
#include <slidersettings.h>
#include <graphsettings.h>
#include <pageslidersetupwgt.h>
#include <pageprogramconfig.h>
#include <restoreerrorsdialog.h>
#include <comparesettingsdialog.h>
#include <functions.h>
#include <mainwindow.h>
#include <main.h>
#include <status.h>

//константы меню
int PageRootDeviceConfigurationNum;
int PageBordersNum;
int PageLimitsNum;
int PageMotorTypeNum;
int PageDCMotorNum;
int PageBLDCMotorNum;
int PageStepperMotorNum;
int PageStepperMotorEMFNum;
int PagePowerNum;
int PageBrakeNum;
int PageCTLNum;
int PageCTPNum;
int PageHomePositionNum;
int PagePidNum;
int PageTtlNum;
int PageUartNum;
int PageExtioNum;
int PageADNum;
int PageRootServiceNum;
int PageCalibNum;
int PageKeyNum;
int PageDbgNum;
int PageSliderSetupNum;
int PageIntrfSettingsNum;
int PageCyclicNum;
int PageProgramConfigNum;
int PageUiConfigNum;
int PageLogSettingsNum;
int PageUserUnitsNum;
int PageGraphNum;
int PageGraphPosNum;
int PageGraphSpeedNum;
int PageGraphEncoderNum;
int PageGraphFlagsNum;
int PageGraphEnVoltNum;
int PageGraphPwrVoltNum;
int PageGraphUsbVoltNum;
int PageGraphVoltANum;
int PageGraphVoltBNum;
int PageGraphVoltCNum;
int PageGraphEnCurrNum;
int PageGraphPwrCurrNum;
int PageGraphUsbCurrNum;
int PageGraphCurrANum;
int PageGraphCurrBNum;
int PageGraphCurrCNum;
int PageGraphPwmNum;
int PageGraphTempNum;
int PageGraphJoyNum;
int PageGraphPotNum;
int PageAboutNum;
int PageRootStageConfigurationNum;
int PagePositionerNameNum;
int PageStageNum;
int PageStageDCMotorNum;
int PageStageEncoderNum;
int PageStageHallsensorNum;
int PageStageGearNum;
int PageStageAccessoriesNum;

const char* PageGraphPosStr = "Graph_position_setup";
const char* PageGraphSpeedStr = "Graph_speed_setup";
const char* PageGraphEncoderStr = "Graph_encoder_setup";
const char* PageGraphFlagsStr = "Graph_flags_setup";
const char* PageGraphEnVoltStr = "Graph_envoltage_setup";
const char* PageGraphPwrVoltStr = "Graph_pwrvoltage_setup";
const char* PageGraphUsbVoltStr = "Graph_usbvoltage_setup";
const char* PageGraphVoltAStr = "Graph_voltageA_setup";
const char* PageGraphVoltBStr = "Graph_voltageB_setup";
const char* PageGraphVoltCStr = "Graph_voltageC_setup";
const char* PageGraphEnCurrStr = "Graph_encurrent_setup";
const char* PageGraphPwrCurrStr = "Graph_pwrcurrent_setup";
const char* PageGraphUsbCurrStr = "Graph_usbcurrent_setup";
const char* PageGraphCurrAStr = "Graph_currentA_setup";
const char* PageGraphCurrBStr = "Graph_currentB_setup";
const char* PageGraphCurrCStr = "Graph_currentC_setup";
const char* PageGraphPwmStr = "Graph_pwm_setup";
const char* PageGraphTempStr = "Graph_temperature_setup";
const char* PageGraphJoyStr = "Graph_joy_setup";
const char* PageGraphPotStr = "Graph_pot_setup";

SettingsDlg::SettingsDlg(QWidget *parent, UpdateThread *_updateThread, DeviceInterface *_devinterface, bool _isMultiaxis) :
    QDialog(parent),
    m_ui(new Ui::SettingsDlg)
{
	devinterface = _devinterface;
	updateThread = _updateThread;
	isMultiaxis = _isMultiaxis;

	motorStgs = new MotorSettings(devinterface);
	motorStgs->LoadFirstInfo();
	controllerStgs = new ControllerSettings(devinterface, &updateThread->QSdev_name);
	firmwareUpdate = new FirmwareUpdateThread(this, updateThread, devinterface);
	cyclicStgs = new CyclicSettings();

	sliderStgs = new SliderSettings();
	graphStgs = new GraphSettings();
	stageStgs = new StageSettings(devinterface);
	logStgs = new LogSettings();
	intStgs = new AttenSettings();
	uuStgs = new UserUnitSettings();
	force_save_all = false;

    m_ui->setupUi(this);
    InitControls();

	Q_FOREACH (QWidget* item, pageWgtsLst) {
		Q_FOREACH (QObject* sbox, item->findChildren<StepSpinBox*>()) {
			StepSpinBox *ssb = dynamic_cast<StepSpinBox*>(sbox);
			if (ssb != NULL) {
				ssb->setSerial(controllerStgs->serialnumber);
			}
		}
	}

	//обновляем контрол типа мотора, его использует интерфейс для определения типа мотора
	((PageMotorTypeWgt*)pageWgtsLst[PageMotorTypeNum])->FromClassToUi();

    QObject::connect(m_ui->treeWgt,         SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),  this, SLOT(OnCurItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
    QObject::connect(m_ui->buttonBox,       SIGNAL(clicked(QAbstractButton *)),                              this, SLOT(OnBtnBoxClicked(QAbstractButton*)));
    QObject::connect(m_ui->restoreFlashBtn, SIGNAL(clicked()),                                               this, SLOT(OnRestoreFlashBtnClicked()));
    QObject::connect(m_ui->saveFlashBtn,    SIGNAL(clicked()),                                               this, SLOT(OnSaveFlashBtnClicked()));
	QObject::connect(m_ui->restoreFileBtn,  SIGNAL(clicked()),                                               this, SLOT(OnRestoreFileBtnClicked()));
	QObject::connect(m_ui->saveFileBtn,		SIGNAL(clicked()),                                               this, SLOT(OnSaveFileBtnClicked()));
	QObject::connect(m_ui->compareFileBtn,  SIGNAL(clicked()),                                               this, SLOT(OnCompareFileBtnClicked()));

	QObject::connect(this, SIGNAL(SgnChangeSettings()), this, SLOT(setStyles()));
	QObject::connect(((PageMotorTypeWgt*)pageWgtsLst[PageMotorTypeNum]), SIGNAL(SgnMotorTypeChanged(unsigned int)), this, SLOT(motorTypeChanged(unsigned int)));
	QObject::connect(((PageStepperMotorWgt*)pageWgtsLst[PageStepperMotorNum]), SIGNAL(SgnStepperMotor(unsigned int)), this, SLOT(StepperMotorChanged(unsigned int)));
    QObject::connect(((PageUiConfigWgt*)pageWgtsLst[PageUiConfigNum]),  SIGNAL(ChangeInterface(SkinType)), this, SLOT(SkinTypeChanged(SkinType)));
	QObject::connect(this, SIGNAL(finished(int)), this, SLOT(saveWindowParams()));
	QObject::connect(firmwareUpdate, SIGNAL(update_finished(result_t)), this, SLOT(firmwareUploaded(result_t)));
		
	m_ui->reconnectDelaySpin->hide();

	old_feedback_type = 0;
	motorTypeChanged(motorStgs->entype.EngineType);
	StepSpinBox::setStepFactorAll(controllerStgs->serialnumber, true, motorStgs->getStepFrac());

	QString wndTitle = "Settings " + QString(this->controllerStgs->device_info.ManufacturerId) + toStr(this->controllerStgs->serialnumber);
	setWindowTitle(wndTitle);
}

SettingsDlg::~SettingsDlg()
{
    delete m_ui;
    delete motorStgs;
	delete cyclicStgs;
	delete sliderStgs;
	delete logStgs;
}

void SettingsDlg::closeEvent(QCloseEvent *e)
{
	e->ignore();
	m_ui->buttonBox->button(QDialogButtonBox::Cancel)->click();
	// how's that for a workaround?
}

void SettingsDlg::hideEvent(QHideEvent *e)
{
	Q_UNUSED(e)
	devinterface->cs->setAnalogEnabled(false);
}

void SettingsDlg::showEvent(QShowEvent *e)
{
	devinterface->cs->setAnalogEnabled(treeWgtsLst[PageCalibNum]->isSelected());
	CheckForChanges();
	e->accept();
}

void SettingsDlg::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void SettingsDlg::firmwareUploaded(result_t result)
{
	if (result != result_ok)
		return;
	
	controllerStgs->LoadControllerSettings();
	((PageAboutDeviceWgt*)pageWgtsLst[PageADNum])->FromClassToUi();

	treeWgtsLst[PageBordersNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	treeWgtsLst[PageLimitsNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	treeWgtsLst[PageMotorTypeNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	treeWgtsLst[PageStepperMotorNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	treeWgtsLst[PageBrakeNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	treeWgtsLst[PageCTLNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	treeWgtsLst[PageCTPNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	treeWgtsLst[PageTtlNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	treeWgtsLst[PageHomePositionNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	treeWgtsLst[PagePidNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	treeWgtsLst[PageADNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	treeWgtsLst[PageCalibNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	treeWgtsLst[PageKeyNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	treeWgtsLst[PageDbgNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	treeWgtsLst[PageCyclicNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	treeWgtsLst[PageUserUnitsNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

#ifdef SERVICEMODE
	treeWgtsLst[PageStageNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
#endif

	treeWgtsLst[PagePositionerNameNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	treeWgtsLst[PageSliderSetupNum]->setFlags(isMultiaxis ? (Qt::NoItemFlags) : (Qt::ItemIsSelectable | Qt::ItemIsEnabled) );
	treeWgtsLst[PageRootStageConfigurationNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);


	controllerStgs->LoadControllerSettings();
	motorStgs->LoadFirstInfo();
	motorTypeChanged(motorStgs->entype.EngineType);
	AllPagesFromDeviceToClassToUi();
}

bool SettingsDlg::AllPagesFromDeviceToClassToUi(bool load_settings/* = true*/, bool load_all_settings/* = true*/)
{
	if (devinterface->getMode() == BOOTLOADER_MODE)
	{
		for (int i=0; i<treeWgtsLst.length(); i++) {
			treeWgtsLst[i]->setFlags(Qt::NoItemFlags);
		}
		treeWgtsLst[PageRootDeviceConfigurationNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		treeWgtsLst[PageRootServiceNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

		treeWgtsLst[PageADNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		treeWgtsLst[PageKeyNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		treeWgtsLst[PageDbgNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

		m_ui->treeWgt->setCurrentItem(treeWgtsLst[PageADNum]);
		return true;
	}
	else {
		for (int i=0; i<treeWgtsLst.length(); i++) {
			treeWgtsLst[i]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		}
		int entype = motorStgs->entype.EngineType;
		// Disable all unused engine type pages and switch away if needed
		motorTypeChanged(entype);
		if(entype == ENGINE_TYPE_STEP){
			if (treeWgtsLst[PageDCMotorNum]->isSelected() || treeWgtsLst[PageBLDCMotorNum]->isSelected())
				m_ui->treeWgt->setCurrentItem(treeWgtsLst[PageStepperMotorNum]);
		}
		if(entype == ENGINE_TYPE_DC){
			if (treeWgtsLst[PageBLDCMotorNum]->isSelected() || treeWgtsLst[PageStepperMotorNum]->isSelected())
				m_ui->treeWgt->setCurrentItem(treeWgtsLst[PageDCMotorNum]);
		}
		if (entype == ENGINE_TYPE_BRUSHLESS) {
			if (treeWgtsLst[PageDCMotorNum]->isSelected() || treeWgtsLst[PageStepperMotorNum]->isSelected())
				m_ui->treeWgt->setCurrentItem(treeWgtsLst[PageBLDCMotorNum]);
		}
		//Засериваем часть Program configuration в режиме Multiaxis
		if (isMultiaxis) {
			((PageSliderSetupWgt*)pageWgtsLst[PageSliderSetupNum])->SetDisabledPart(true);
			((PageUiConfigWgt*)pageWgtsLst[PageUiConfigNum])->SetDisabledAtten(true);
			treeWgtsLst[PageCyclicNum]->setFlags(Qt::NoItemFlags);
			treeWgtsLst[PageLogSettingsNum]->setFlags(Qt::NoItemFlags);
			treeWgtsLst[PageGraphNum]->setFlags(Qt::NoItemFlags);
			treeWgtsLst[PageIntrfSettingsNum]->setFlags(Qt::NoItemFlags);
		}
		// #9629, disable deprecated fields for new controllers
#ifndef SERVICEMODE
		((PageLimitsWgt*)pageWgtsLst[PageLimitsNum])->disableCriticalUSB(controllerStgs->treatAs8SMC5());
		
		((PageStepperMotorWgt*)pageWgtsLst[PageStepperMotorNum])->disableDifferentialFeedback(!controllerStgs->treatAs8SMC5());
		((PageDCMotorWgt*)pageWgtsLst[PageDCMotorNum])->disableDifferentialFeedback(!controllerStgs->treatAs8SMC5());
		((PageBLDCMotorWgt*)pageWgtsLst[PageBLDCMotorNum])->disableDifferentialFeedback(!controllerStgs->treatAs8SMC5());

		// # 22484, disable BLDC motor type for 8SMC4
		((PageMotorTypeWgt*)pageWgtsLst[PageMotorTypeNum])->disableBLDC(controllerStgs->isFirmware8SMC4OrYounger());
		// ((PageUartWgt*)pageWgtsLst[PageUartNum])->setDisabled(TRUE);
		// ((PageUartWgt*)pageWgtsLst[PageUartNum])->setVisible(FALSE);
		treeWgtsLst[PageUartNum]->setFlags(Qt::NoItemFlags);
#endif
	}

	if(load_settings){
		MotorSettings tmpStgs(devinterface);
		FromUiToClass(&tmpStgs);

		motorStgs->need_edges_load = !(motorStgs->edges == tmpStgs.edges);
		motorStgs->need_secure_load = !(motorStgs->secure == tmpStgs.secure);
		motorStgs->need_engine_load = !(motorStgs->engine == tmpStgs.engine);
		motorStgs->need_entype_load = !(motorStgs->entype == tmpStgs.entype);
		motorStgs->need_move_load = !(motorStgs->move == tmpStgs.move);
		motorStgs->need_feedback_load = !(motorStgs->feedback == tmpStgs.feedback);
		motorStgs->need_power_load = !(motorStgs->power == tmpStgs.power);
		motorStgs->need_brake_load = !(motorStgs->brake == tmpStgs.brake);
		motorStgs->need_control_load = !(motorStgs->control == tmpStgs.control);
		motorStgs->need_joystick_load = !(motorStgs->joystick == tmpStgs.joystick);
		motorStgs->need_ctp_load = !(motorStgs->ctp == tmpStgs.ctp);
		motorStgs->need_home_load = !(motorStgs->home == tmpStgs.home);
		motorStgs->need_pid_load = !(motorStgs->pid == tmpStgs.pid);
		motorStgs->need_emf_load = !(motorStgs->emf == tmpStgs.emf);
		motorStgs->need_sync_in_load = !(motorStgs->sync_in == tmpStgs.sync_in);
		motorStgs->need_sync_out_load = !(motorStgs->sync_out == tmpStgs.sync_out);
		motorStgs->need_uart_load = !(motorStgs->uart == tmpStgs.uart);
		motorStgs->need_extio_load = !(motorStgs->extio == tmpStgs.extio);
		motorStgs->need_name_load = !(motorStgs->name == tmpStgs.name);

		motorStgs->FromDeviceToClass(load_all_settings);	//если load_all_settings == false, загружаются только измененные при последнем сохранении настройки,
		if(load_all_settings)						//параметры контроллера здесь не изменяются, поэтому при неполной загрузке они не загружаются
			controllerStgs->LoadControllerSettings();
	}
	((PageUserUnitsWgt*)pageWgtsLst[PageUserUnitsNum])->FromClassToUi(true); // must set new userunits before all other pages that depend on them
	curr_tmp_uuStgs = *uuStgs; // should go after pageuserunits class to ui

	// Motor type page Ui load should happen before individual pages, because it defines which motor type page is disabled
	((PageMotorTypeWgt*)pageWgtsLst[PageMotorTypeNum])->FromClassToUi();

	// Motor page Ui load should happen first, because it might trigger encoder feedback change, which will recalculate all stepspinbox values on all pages
	// Load all 3 pages, but emit only on active page
	// TODO: should we emit at all here
	((PageDCMotorWgt*)pageWgtsLst[PageDCMotorNum])->FromClassToUi(! treeWgtsLst[PageDCMotorNum]->isDisabled());
	((PageBLDCMotorWgt*)pageWgtsLst[PageBLDCMotorNum])->FromClassToUi(! treeWgtsLst[PageBLDCMotorNum]->isDisabled());
	((PageStepperMotorWgt*)pageWgtsLst[PageStepperMotorNum])->FromClassToUi(! treeWgtsLst[PageStepperMotorNum]->isDisabled());

	((PageAboutDeviceWgt*)pageWgtsLst[PageADNum])->FromClassToUi();
	((PageBordersWgt*)pageWgtsLst[PageBordersNum])->FromClassToUi();
	((PageLimitsWgt*)pageWgtsLst[PageLimitsNum])->FromClassToUi();

	//Load information about attenuator
	((PageIntrfSettingsWgt*)pageWgtsLst[PageIntrfSettingsNum])->FromClassToUi();

	//about type of using skin: attenuator or general
	((PageUiConfigWgt*)pageWgtsLst[PageUiConfigNum])->FromClassToUi();
	((PageSliderSetupWgt*)pageWgtsLst[PageSliderSetupNum])->FromClassToUi();
	SkinType cur_skin = intStgs->skinType;
	if (cur_skin == GeneralSkin)
		treeWgtsLst[PageIntrfSettingsNum]->setFlags(Qt::NoItemFlags);
	else
		treeWgtsLst[PageSliderSetupNum]->setFlags(Qt::NoItemFlags);

	((PageHomePositionWgt*)pageWgtsLst[PageHomePositionNum])->FromClassToUi();
	((PagePidWgt*)pageWgtsLst[PagePidNum])->FromClassToUi();
	((PageTtlWgt*)pageWgtsLst[PageTtlNum])->FromClassToUi();
	((PageUartWgt*)pageWgtsLst[PageUartNum])->FromClassToUi();
	((PageExtioWgt*)pageWgtsLst[PageExtioNum])->FromClassToUi();
	((PageCyclicWgt*)pageWgtsLst[PageCyclicNum])->FromClassToUi();
	((PageLogSettingsWgt*)pageWgtsLst[PageLogSettingsNum])->FromClassToUi();
	//Graph setup
	((PageGraphCommonWgt*)pageWgtsLst[PageGraphNum])->FromClassToUi();
	((PageGraphWgt*)pageWgtsLst[PageGraphPosNum])->FromClassToUi();
	((PageGraphWgt*)pageWgtsLst[PageGraphSpeedNum])->FromClassToUi();
	((PageGraphWgt*)pageWgtsLst[PageGraphEncoderNum])->FromClassToUi();
	((PageGraphWgt*)pageWgtsLst[PageGraphFlagsNum])->FromClassToUi();
	((PageGraphWgt*)pageWgtsLst[PageGraphEnVoltNum])->FromClassToUi();
	((PageGraphWgt*)pageWgtsLst[PageGraphEnCurrNum])->FromClassToUi();
	((PageGraphWgt*)pageWgtsLst[PageGraphPwrVoltNum])->FromClassToUi();
	((PageGraphWgt*)pageWgtsLst[PageGraphPwrCurrNum])->FromClassToUi();
	((PageGraphWgt*)pageWgtsLst[PageGraphUsbVoltNum])->FromClassToUi();
	((PageGraphWgt*)pageWgtsLst[PageGraphUsbCurrNum])->FromClassToUi();

	((PageGraphWgt*)pageWgtsLst[PageGraphVoltANum])->FromClassToUi();
	((PageGraphWgt*)pageWgtsLst[PageGraphVoltBNum])->FromClassToUi();
	((PageGraphWgt*)pageWgtsLst[PageGraphVoltCNum])->FromClassToUi();
	((PageGraphWgt*)pageWgtsLst[PageGraphCurrANum])->FromClassToUi();
	((PageGraphWgt*)pageWgtsLst[PageGraphCurrBNum])->FromClassToUi();
	((PageGraphWgt*)pageWgtsLst[PageGraphCurrCNum])->FromClassToUi();

	((PagePowerWgt*)pageWgtsLst[PagePowerNum])->FromClassToUi();
	((PageBrakeWgt*)pageWgtsLst[PageBrakeNum])->FromClassToUi();
	((PageCTLWgt*)pageWgtsLst[PageCTLNum])->FromClassToUi();
	((PageControlPositionWgt*)pageWgtsLst[PageCTPNum])->FromClassToUi();
	
	((PageGraphWgt*)pageWgtsLst[PageGraphPwmNum])->FromClassToUi();
	((PageGraphWgt*)pageWgtsLst[PageGraphTempNum])->FromClassToUi();
	((PageGraphWgt*)pageWgtsLst[PageGraphJoyNum])->FromClassToUi();
	((PageGraphWgt*)pageWgtsLst[PageGraphPotNum])->FromClassToUi();
	return true;
}

void SettingsDlg::AllPagesFromUiToClassToDevice()
{
#ifdef SERVICEMODE
	// Service
	((PageStep3Wgt*)pageWgtsLst[PageKeyNum])->FromUiToClass();
#endif
	
	//save userunits before new changes are applied
	saved_uuStgs = *uuStgs;

	((PageSliderSetupWgt*)pageWgtsLst[PageSliderSetupNum])->FromUiToClass();
	((PageCyclicWgt*)pageWgtsLst[PageCyclicNum])->FromUiToClass();
	((PageUserUnitsWgt*)pageWgtsLst[PageUserUnitsNum])->FromUiToClass();
	((PageLogSettingsWgt*)pageWgtsLst[PageLogSettingsNum])->FromUiToClass();
	//Graph setup
	((PageGraphCommonWgt*)pageWgtsLst[PageGraphNum])->FromUiToClass();
	((PageGraphWgt*)pageWgtsLst[PageGraphPosNum])->FromUiToClass();
	((PageGraphWgt*)pageWgtsLst[PageGraphSpeedNum])->FromUiToClass();
	((PageGraphWgt*)pageWgtsLst[PageGraphEncoderNum])->FromUiToClass();
	((PageGraphWgt*)pageWgtsLst[PageGraphFlagsNum])->FromUiToClass();
	((PageGraphWgt*)pageWgtsLst[PageGraphEnVoltNum])->FromUiToClass();
	((PageGraphWgt*)pageWgtsLst[PageGraphEnCurrNum])->FromUiToClass();
	((PageGraphWgt*)pageWgtsLst[PageGraphPwrVoltNum])->FromUiToClass();
	((PageGraphWgt*)pageWgtsLst[PageGraphPwrCurrNum])->FromUiToClass();
	((PageGraphWgt*)pageWgtsLst[PageGraphUsbVoltNum])->FromUiToClass();
	((PageGraphWgt*)pageWgtsLst[PageGraphUsbCurrNum])->FromUiToClass();

	((PageGraphWgt*)pageWgtsLst[PageGraphVoltANum])->FromUiToClass();
	((PageGraphWgt*)pageWgtsLst[PageGraphVoltBNum])->FromUiToClass();
	((PageGraphWgt*)pageWgtsLst[PageGraphVoltCNum])->FromUiToClass();
	((PageGraphWgt*)pageWgtsLst[PageGraphCurrANum])->FromUiToClass();
	((PageGraphWgt*)pageWgtsLst[PageGraphCurrBNum])->FromUiToClass();
	((PageGraphWgt*)pageWgtsLst[PageGraphCurrCNum])->FromUiToClass();

	((PageGraphWgt*)pageWgtsLst[PageGraphPwmNum])->FromUiToClass();
	((PageGraphWgt*)pageWgtsLst[PageGraphTempNum])->FromUiToClass();
	((PageGraphWgt*)pageWgtsLst[PageGraphJoyNum])->FromUiToClass();
	((PageGraphWgt*)pageWgtsLst[PageGraphPotNum])->FromUiToClass();

	CheckForChanges();


	MotorSettings tmpStgs = *motorStgs;
	StageSettings tspStgs = *stageStgs;
	FromUiToClass(&tmpStgs);
	FromUiToClass(&tspStgs);

	motorStgs->need_edges_save = !(motorStgs->edges == tmpStgs.edges);
	motorStgs->need_secure_save = !(motorStgs->secure == tmpStgs.secure);
	motorStgs->need_engine_save = !(motorStgs->engine == tmpStgs.engine);
	motorStgs->need_entype_save = !(motorStgs->entype == tmpStgs.entype);
	motorStgs->need_move_save = !(motorStgs->move == tmpStgs.move);
	if (!motorStgs->need_move_save)
		motorStgs->need_move_save = !(motorStgs->move.MoveFlags == tmpStgs.move.MoveFlags);
	motorStgs->need_feedback_save = !(motorStgs->feedback == tmpStgs.feedback);
	motorStgs->need_power_save = !(motorStgs->power == tmpStgs.power);
	motorStgs->need_brake_save = !(motorStgs->brake == tmpStgs.brake);
	motorStgs->need_control_save = !(motorStgs->control == tmpStgs.control);
	motorStgs->need_joystick_save = !(motorStgs->joystick == tmpStgs.joystick);
	motorStgs->need_ctp_save = !(motorStgs->ctp == tmpStgs.ctp);
	motorStgs->need_home_save = !(motorStgs->home == tmpStgs.home);
	motorStgs->need_pid_save = !(motorStgs->pid == tmpStgs.pid);
	motorStgs->need_emf_save = !(motorStgs->emf == tmpStgs.emf);
	motorStgs->need_sync_in_save = !(motorStgs->sync_in == tmpStgs.sync_in);
	motorStgs->need_sync_out_save = !(motorStgs->sync_out == tmpStgs.sync_out);
	motorStgs->need_uart_save = !(motorStgs->uart == tmpStgs.uart);
	motorStgs->need_extio_save = !(motorStgs->extio == tmpStgs.extio);
	motorStgs->need_name_save = !(motorStgs->name == tmpStgs.name);
	if (force_save_all) {
		motorStgs->set_save_true();
		force_save_all = false;
	}

	bool crit_params_changed = false;
	crit_params_changed |= !(motorStgs->entype.EngineType == tmpStgs.entype.EngineType);
	crit_params_changed |= !(motorStgs->entype.DriverType == tmpStgs.entype.DriverType);
	crit_params_changed |= !(motorStgs->engine.MicrostepMode == tmpStgs.engine.MicrostepMode);
	crit_params_changed |= !((motorStgs->engine.EngineFlags & ENGINE_REVERSE) == (tmpStgs.engine.EngineFlags & ENGINE_REVERSE));
	crit_params_changed |= !(motorStgs->feedback.FeedbackType == tmpStgs.feedback.FeedbackType);
	if (crit_params_changed)
		emit SgnChangeCritSettings();
    
	

	motorStgs->edges = tmpStgs.edges;
	motorStgs->secure = tmpStgs.secure;
	motorStgs->engine = tmpStgs.engine;

    //check correct steps per turn 
	int turn  = tmpStgs.engine.StepsPerRev;
	bool correct_settings = true;
	if(!turn) {
		correct_settings = false;
	}

    // Program configuration
	((PageUiConfigWgt*)pageWgtsLst[PageUiConfigNum])->FromUiToClass();

	if(!isMultiaxis){
		((PageIntrfSettingsWgt*)pageWgtsLst[PageIntrfSettingsNum])->FromUiToClass();
		if(intStgs->skinType == GeneralSkin)
			emit clickedChangeInterface(*intStgs, correct_settings);
		else if (intStgs->skinType == AttenuatorSkin) 
			emit clickedChangeInterface(*intStgs, correct_settings);
	}


	motorStgs->entype = tmpStgs.entype;
	motorStgs->move = tmpStgs.move;
	motorStgs->feedback = tmpStgs.feedback;
	motorStgs->power = tmpStgs.power;
	motorStgs->brake = tmpStgs.brake;
	motorStgs->control = tmpStgs.control;
	motorStgs->joystick = tmpStgs.joystick;
	motorStgs->ctp = tmpStgs.ctp;
	motorStgs->home = tmpStgs.home;
	motorStgs->pid = tmpStgs.pid;
	motorStgs->emf = tmpStgs.emf;
	motorStgs->sync_in = tmpStgs.sync_in;
	motorStgs->sync_out = tmpStgs.sync_out;
	motorStgs->uart = tmpStgs.uart;
	motorStgs->extio = tmpStgs.extio;
	motorStgs->name = tmpStgs.name;

	emit SgnUpdateCalibration(*uuStgs, motorStgs->engine.MicrostepMode);

	stageStgs->need_stage_name_save = !(stageStgs->stage_name == tspStgs.stage_name);
	stageStgs->need_stage_information_save = !(stageStgs->stage_information == tspStgs.stage_information);
	stageStgs->need_stage_settings_save = !(stageStgs->stage_settings == tspStgs.stage_settings);
	stageStgs->need_motor_information_save = !(stageStgs->motor_information == tspStgs.motor_information);
	stageStgs->need_motor_settings_save = !(stageStgs->motor_settings == tspStgs.motor_settings);
	stageStgs->need_encoder_information_save = !(stageStgs->encoder_information == tspStgs.encoder_information);
	stageStgs->need_encoder_settings_save = !(stageStgs->encoder_settings == tspStgs.encoder_settings);
	stageStgs->need_hallsensor_information_save = !(stageStgs->hallsensor_information == tspStgs.hallsensor_information);
	stageStgs->need_hallsensor_settings_save = !(stageStgs->hallsensor_settings == tspStgs.hallsensor_settings);
	stageStgs->need_gear_information_save = !(stageStgs->gear_information == tspStgs.gear_information);
	stageStgs->need_gear_settings_save = !(stageStgs->gear_settings == tspStgs.gear_settings);
	stageStgs->need_accessories_settings_save = !(stageStgs->accessories_settings == tspStgs.accessories_settings);

	stageStgs->stage_name = tspStgs.stage_name;
	stageStgs->stage_information = tspStgs.stage_information;
	stageStgs->stage_settings = tspStgs.stage_settings;
	stageStgs->motor_information = tspStgs.motor_information;
	stageStgs->motor_settings = tspStgs.motor_settings;
	stageStgs->encoder_information = tspStgs.encoder_information;
	stageStgs->encoder_settings = tspStgs.encoder_settings;
	stageStgs->hallsensor_information = tspStgs.hallsensor_information;
	stageStgs->hallsensor_settings = tspStgs.hallsensor_settings;
	stageStgs->gear_information = tspStgs.gear_information;
	stageStgs->gear_settings = tspStgs.gear_settings;
	stageStgs->accessories_settings = tspStgs.accessories_settings;

	motorStgs->FromClassToDevice();
 	if ((devinterface->cs->status().Flags & STATE_EEPROM_CONNECTED) && devinterface->cs->connect())
		stageStgs->FromClassToDevice();

	QPalette pt;
	pt.setColor(QPalette::Normal, QPalette::WindowText, QColor(0,0,0,255));
	m_ui->configLbl->setPalette(pt);
	m_ui->configLbl->setToolTip("");

	emit SgnChangeSettings();
}

void SettingsDlg::StageSettingsFromDeviceToClassToUi()
{
	stageStgs->FromDeviceToClass();
	((PagePositionerNameWgt*)pageWgtsLst[PagePositionerNameNum])->FromClassToUi();

#ifdef SERVICEMODE
	((PageStageWgt*)pageWgtsLst[PageStageNum])->FromClassToUi();
	((PageStageDCMotorWgt*)pageWgtsLst[PageStageDCMotorNum])->FromClassToUi();
	((PageStageEncoderWgt*)pageWgtsLst[PageStageEncoderNum])->FromClassToUi();
	((PageStageHallsensorWgt*)pageWgtsLst[PageStageHallsensorNum])->FromClassToUi();
	((PageStageGearWgt*)pageWgtsLst[PageStageGearNum])->FromClassToUi();
	((PageStageAccessoriesWgt*)pageWgtsLst[PageStageAccessoriesNum])->FromClassToUi();
#endif
}

void SettingsDlg::OnPrecedenceChecked(bool checked)
{
	m_ui->saveFlashBtn->setAutoFillBackground(true);
	m_ui->saveFlashBtn->setStyleSheet(checked ? "color: rgb(255,0,0);" : "color: rgb(0,0,0);");
}

void SettingsDlg::CheckForChanges()
{
	if (temp_config_file.isEmpty()) // return so that XSettings doesn't complain about missing file
		return; // it's not like there was anything to compare in this case anyway

	QTemporaryFile tmp_file;
	if (!tmp_file.open()) { // if we can't open temp file then fail, complain and do nothing
		QMessageBox::warning(this, "Error", "Cannot write to temporary file.", QMessageBox::Ok);
		return;
	}

	XSettings second_settings(tmp_file.fileName(), QSettings::IniFormat, QIODevice::ReadWrite);
	AllPagesFromUiToSettings(&second_settings);
	second_settings.sync();
	QStringList keys2 = second_settings.allKeys();

	XSettings first_settings(temp_config_file, QSettings::IniFormat, QIODevice::ReadOnly);
	QStringList keys1 = first_settings.allKeys();

	bool changes_found = false;
	Q_FOREACH (QString key, keys2) {
		if (first_settings.value(key) != second_settings.value(key)) {
			changes_found = true;
			break;
		}
	}

	if (changes_found){
		if((!m_ui->configLbl->text().startsWith("*")) && (!m_ui->configLbl->text().isEmpty()))
			m_ui->configLbl->setText("*"+m_ui->configLbl->text());
	} else {
		if((m_ui->configLbl->text().startsWith("*")))
			m_ui->configLbl->setText(m_ui->configLbl->text().mid(1));
	}
	firstLoadConfig = false;
}

void SettingsDlg::FromUiToClass(MotorSettings *stgs)
{
	((PageAboutDeviceWgt*)pageWgtsLst[PageADNum])->FromUiToClass(stgs);
	((PageBordersWgt*)pageWgtsLst[PageBordersNum])->FromUiToClass(stgs);
	((PageLimitsWgt*)pageWgtsLst[PageLimitsNum])->FromUiToClass(stgs);
	((PageMotorTypeWgt*)pageWgtsLst[PageMotorTypeNum])->FromUiToClass(stgs);

	if (((QTreeWidgetItem*)treeWgtsLst[PageDCMotorNum])->flags().testFlag(Qt::ItemIsEnabled)) {
		((PageDCMotorWgt*)pageWgtsLst[PageDCMotorNum])->FromUiToClass(stgs);
	}
	if (((QTreeWidgetItem*)treeWgtsLst[PageBLDCMotorNum])->flags().testFlag(Qt::ItemIsEnabled)) {
		((PageBLDCMotorWgt*)pageWgtsLst[PageBLDCMotorNum])->FromUiToClass(stgs);
	}
	if (((QTreeWidgetItem*)treeWgtsLst[PageStepperMotorNum])->flags().testFlag(Qt::ItemIsEnabled)) {
		((PageStepperMotorWgt*)pageWgtsLst[PageStepperMotorNum])->FromUiToClass(stgs);
		stgs->engine.NomVoltage = motorStgs->engine.NomVoltage; // steppers don't have NomVoltage, so we copy it so that compare doesn't fail
	}

	((PageHomePositionWgt*)pageWgtsLst[PageHomePositionNum])->FromUiToClass(stgs);
	((PagePidWgt*)pageWgtsLst[PagePidNum])->FromUiToClass(stgs);
	((PageTtlWgt*)pageWgtsLst[PageTtlNum])->FromUiToClass(stgs);
	((PageUartWgt*)pageWgtsLst[PageUartNum])->FromUiToClass(stgs);
	((PageExtioWgt*)pageWgtsLst[PageExtioNum])->FromUiToClass(stgs);

	((PagePowerWgt*)pageWgtsLst[PagePowerNum])->FromUiToClass(stgs);
	((PageBrakeWgt*)pageWgtsLst[PageBrakeNum])->FromUiToClass(stgs);
	((PageCTLWgt*)pageWgtsLst[PageCTLNum])->FromUiToClass(stgs);
	((PageControlPositionWgt*)pageWgtsLst[PageCTPNum])->FromUiToClass(stgs);
}

void SettingsDlg::FromUiToClass(StageSettings *stgs)
{
	//StageConfiguration
	((PagePositionerNameWgt*)pageWgtsLst[PagePositionerNameNum])->FromUiToClass(stgs);

#ifdef SERVICEMODE
	((PageStageWgt*)pageWgtsLst[PageStageNum])->FromUiToClass(stgs);
	((PageStageDCMotorWgt*)pageWgtsLst[PageStageDCMotorNum])->FromUiToClass(stgs);
	((PageStageEncoderWgt*)pageWgtsLst[PageStageEncoderNum])->FromUiToClass(stgs);
	((PageStageHallsensorWgt*)pageWgtsLst[PageStageHallsensorNum])->FromUiToClass(stgs);
	((PageStageGearWgt*)pageWgtsLst[PageStageGearNum])->FromUiToClass(stgs);
	((PageStageAccessoriesWgt*)pageWgtsLst[PageStageAccessoriesNum])->FromUiToClass(stgs);
#endif
}

void SettingsDlg::InitControls()
{
    QTreeWidgetItem *parentItem, *parentItem2;
    QTreeWidgetItem *childItem;
    
    QWidget *parentWgt;
	QWidget *childWgt;

	int i=0;

	//Device configuration
	PageRootDeviceConfigurationNum=i++;
    parentItem = new QTreeWidgetItem(m_ui->treeWgt);
    parentItem->setText(0, tr("Device"));
    m_ui->treeWgt->setCurrentItem(parentItem);
	parentWgt = new QWidget();
    treeWgtsLst.push_back(parentItem);
    pageWgtsLst.push_back(parentWgt);
    m_ui->scrollAreaWidgetContents->layout()->addWidget(parentWgt);

	int w = parentWgt->minimumSizeHint().width();
	QLayout* ly = m_ui->scrollAreaWidgetContents->layout();
	int w1 = ly->margin();
	int w2 = ly->spacing();
	w += 2*(w1 + w2) + 10;
	m_ui->scrollArea->setMinimumWidth( w );

	//Device configuration->Borders
	PageBordersNum=i++;
    childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("Borders"));
    treeWgtsLst.push_back(childItem);
    pageWgtsLst.push_back((QWidget*)new PageBordersWgt(NULL, motorStgs));

	//Device configuration->Max ratings
	PageLimitsNum=i++;
    childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("Maximum ratings"));
    treeWgtsLst.push_back(childItem);
    pageWgtsLst.push_back((QWidget*)new PageLimitsWgt(NULL, motorStgs));

	//Device configuration->Motor type
	PageMotorTypeNum=i++;
    childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("Motor & Driver type"));
    treeWgtsLst.push_back(childItem);
    pageWgtsLst.push_back((QWidget*)new PageMotorTypeWgt(NULL, motorStgs));

	//Device configuration->DC motor
	PageDCMotorNum=i++;
    childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("DC motor"));
    treeWgtsLst.push_back(childItem);
    pageWgtsLst.push_back((QWidget*)new PageDCMotorWgt(NULL, motorStgs));

	//Device configuration->BLDC motor
	PageBLDCMotorNum=i++;
	childItem = new QTreeWidgetItem(parentItem);
	childItem->setText(0, tr("BLDC motor"));
	treeWgtsLst.push_back(childItem);
	pageWgtsLst.push_back((QWidget*)new PageBLDCMotorWgt(NULL, motorStgs));

	//Device configuration->Stepper motor
	PageStepperMotorNum=i++;
	childItem = new QTreeWidgetItem(parentItem);
	childItem->setText(0, tr("Stepper motor"));
	treeWgtsLst.push_back(childItem);
    pageWgtsLst.push_back((QWidget*)new PageStepperMotorWgt(NULL, motorStgs));

	//Device configuration->Power management
	PagePowerNum=i++;
    childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("Power management"));
    treeWgtsLst.push_back(childItem);
    pageWgtsLst.push_back((QWidget*)new PagePowerWgt(NULL, motorStgs));

	//Device configuration->Brake control
	PageBrakeNum=i++;
    childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("Brake control"));
    treeWgtsLst.push_back(childItem);
    pageWgtsLst.push_back((QWidget*)new PageBrakeWgt(NULL, motorStgs));

	//Device configuration->Control
	PageCTLNum=i++;
    childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("Control"));
    treeWgtsLst.push_back(childItem);
    pageWgtsLst.push_back((QWidget*)new PageCTLWgt(NULL, motorStgs, updateThread));

	//Device configuration->Position control
	PageCTPNum=i++;
    childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("Position control"));
    treeWgtsLst.push_back(childItem);
    pageWgtsLst.push_back((QWidget*)new PageControlPositionWgt(NULL, motorStgs));
	QObject::connect(pageWgtsLst[PageStepperMotorNum], SIGNAL(SgnChangeFrac(unsigned int)), this, SLOT(OnChangeFrac(unsigned int)));
	QObject::connect(pageWgtsLst[PageStepperMotorNum], SIGNAL(SgnSwitchCTP(unsigned int)), this, SLOT(OnSwitchCTP(unsigned int)));
	QObject::connect(pageWgtsLst[PageDCMotorNum], SIGNAL(SgnSwitchCTP(unsigned int)), this, SLOT(OnSwitchCTP(unsigned int)));
	QObject::connect(pageWgtsLst[PageBLDCMotorNum], SIGNAL(SgnSwitchCTP(unsigned int)), this, SLOT(OnSwitchCTP(unsigned int)));

	//Device configuration->Home position
	PageHomePositionNum=i++;
    childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("Home position"));
    treeWgtsLst.push_back(childItem);
    pageWgtsLst.push_back((QWidget*)new PageHomePositionWgt(NULL, motorStgs));

	//Device configuration->PID control
	PagePidNum=i++;
    childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("PID control"));
    treeWgtsLst.push_back(childItem);
	PagePidWgt * pagepid = new PagePidWgt(NULL, motorStgs, devinterface, controllerStgs->serialnumber); // serial number is necessary for user units in PID tuning window
	pageWgtsLst.push_back((QWidget*)pagepid);

	QObject::connect(pagepid, SIGNAL(Apply()), this, SLOT(AllPagesFromUiToClassToDevice()));
	QObject::connect(pagepid, SIGNAL(Apply()), this, SLOT(OnChangeUU()));

	//Device configuration->TTL sync
	PageTtlNum=i++;
    childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("TTL sync"));
    treeWgtsLst.push_back(childItem);
    pageWgtsLst.push_back((QWidget*)new PageTtlWgt(NULL, motorStgs));

	//Device configuration->UART settings
	PageUartNum=i++;
    childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("UART"));
    treeWgtsLst.push_back(childItem);
    pageWgtsLst.push_back((QWidget*)new PageUartWgt(NULL, motorStgs));

	//Device configuration->EXTIO settings
	PageExtioNum=i++;
    childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("EXTIO"));
    treeWgtsLst.push_back(childItem);
    pageWgtsLst.push_back((QWidget*)new PageExtioWgt(NULL, motorStgs));

	//Device configuration->About device
	PageADNum=i++;
    childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("About device"));
	treeWgtsLst.push_back(childItem);
	childWgt = (QWidget*)new PageAboutDeviceWgt(NULL, motorStgs, controllerStgs, firmwareUpdate, devinterface, &default_firmware_path);
	pageWgtsLst.push_back(childWgt);
	QObject::connect((PageAboutDeviceWgt*)childWgt, SIGNAL(precedenceCheckedSgn(bool)), this, SLOT(OnPrecedenceChecked(bool))); // should go before FromClassToUi
	((PageAboutDeviceWgt*)childWgt)->FromClassToUi();
	QObject::connect(m_ui->defaultLocationChk, SIGNAL(toggled(bool)), (PageAboutDeviceWgt*)childWgt, SLOT(defaultLocationChanged(bool)));
	((PageAboutDeviceWgt*)childWgt)->defaultLocationChanged(m_ui->defaultLocationChk->isChecked());
	QObject::connect((PageAboutDeviceWgt*)childWgt, SIGNAL(networkResultSgn(QString, bool, int)), this, SLOT(OnNetResult(QString, bool, int)));
 
#ifdef SERVICEMODE
	//Service
	PageRootServiceNum=i++;
    parentItem = new QTreeWidgetItem(m_ui->treeWgt);
    parentItem->setText(0, tr("Service"));
	parentWgt = new QWidget();
    treeWgtsLst.push_back(parentItem);
    pageWgtsLst.push_back(parentWgt);

	//Service->Calibration
	PageCalibNum=i++;
    childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("Calibration"));
    treeWgtsLst.push_back(childItem);
    pageWgtsLst.push_back((QWidget*)new PageCalibWgt(NULL, updateThread->QSdev_name, devinterface));
 
	//Service->Key & SerialNumber
	PageKeyNum=i++;
    childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("Key & Serial number"));
    treeWgtsLst.push_back(childItem);
    pageWgtsLst.push_back((QWidget*)new PageStep3Wgt(NULL, updateThread, controllerStgs, devinterface));
	connect(pageWgtsLst[PageKeyNum], SIGNAL( serialUpdatedSgn() ), pageWgtsLst[PageADNum], SLOT( OnUpdateSerialFinished() ));

	//Service->Debug
	PageDbgNum=i++;
    childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("Debug"));
    treeWgtsLst.push_back(childItem);
    pageWgtsLst.push_back((QWidget*)new PageDebugWgt(NULL, motorStgs, updateThread->device_name, devinterface));
	connect(pageWgtsLst[PageDbgNum], SIGNAL(EESV()), this, SLOT(AllPagesFromUiToClassToDevice()), Qt::DirectConnection);
	connect(pageWgtsLst[PageDbgNum], SIGNAL(EERD()), this, SLOT(AllPagesFromDeviceToClassToUi()), Qt::DirectConnection);
#endif

	//Program configuration
	PageProgramConfigNum=i++;
	parentItem = new QTreeWidgetItem(m_ui->treeWgt);
    parentItem->setText(0, tr("Program"));
	parentWgt = new QWidget();
    treeWgtsLst.push_back(parentItem);
    pageWgtsLst.push_back(parentWgt);

	//Program configuration->Ui setup
	PageUiConfigNum=i++;
	parentItem2 = new QTreeWidgetItem(parentItem);
	parentItem2->setText(0, tr("Interface"));
	treeWgtsLst.push_back(parentItem2);
	pageWgtsLst.push_back((QWidget*)new PageUiConfigWgt(NULL, intStgs));

		//Program configuration->General motor
		PageSliderSetupNum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("General motor"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageSliderSetupWgt(NULL, sliderStgs));
	 
		//Program configuration->Interface Settings
		PageIntrfSettingsNum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("Attenuator"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageIntrfSettingsWgt(NULL, intStgs));

	//Program configuration->Cyclic motion
	PageCyclicNum=i++;
	childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("Cyclic motion"));
    treeWgtsLst.push_back(childItem);
    pageWgtsLst.push_back((QWidget*)new PageCyclicWgt(NULL, cyclicStgs));

	//Program configuration->Log settings
	PageLogSettingsNum=i++;
	childItem = new QTreeWidgetItem(parentItem);
	childItem->setText(0, tr("Log"));
	treeWgtsLst.push_back(childItem);
	pageWgtsLst.push_back((QWidget*)new PageLogSettingsWgt(NULL, logStgs));

	//Program configuration->Graph setup
	PageGraphNum=i++;
	parentItem2 = new QTreeWidgetItem(parentItem);
	parentItem2->setText(0, tr("Graph"));
	treeWgtsLst.push_back(parentItem2);
	pageWgtsLst.push_back((QWidget*)new PageGraphCommonWgt(NULL, graphStgs->commonStgs));

		//Program configuration->Graph setup->Position
		PageGraphPosNum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("Position"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageGraphWgt(NULL, graphStgs->positionStgs, PageGraphPosStr));
		((PageGraphWgt*)pageWgtsLst.back())->SetCaption("Position curve setup");

		//Program configuration->Graph setup->Speed
		PageGraphSpeedNum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("Speed"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageGraphWgt(NULL, graphStgs->speedStgs, PageGraphSpeedStr));
		((PageGraphWgt*)pageWgtsLst.back())->SetCaption("Speed curve setup");

		//Program configuration->Graph setup->Encoder
		PageGraphEncoderNum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("Encoder"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageGraphWgt(NULL, graphStgs->encoderStgs, PageGraphEncoderStr));
		((PageGraphWgt*)pageWgtsLst.back())->SetCaption("Encoder curve setup");

		//Program configuration->Graph setup->Flags
		PageGraphFlagsNum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("Flags"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageGraphWgt(NULL, graphStgs->flagsStgs, PageGraphFlagsStr));
		((PageGraphWgt*)pageWgtsLst.back())->SetCaption("Flags curve setup");
		((PageGraphWgt*)pageWgtsLst.back())->HideColor();

		//Program configuration->Graph setup->EnVoltage
		PageGraphEnVoltNum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("Engine voltage"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageGraphWgt(NULL, graphStgs->envoltageStgs, PageGraphEnVoltStr));
		((PageGraphWgt*)pageWgtsLst.back())->SetCaption("Eng. voltage curve setup");

		//Program configuration->Graph setup->PwrVoltage
		PageGraphPwrVoltNum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("Power voltage"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageGraphWgt(NULL, graphStgs->pwrvoltageStgs, PageGraphPwrVoltStr));
		((PageGraphWgt*)pageWgtsLst.back())->SetCaption("Power voltage curve setup");

		//Program configuration->Graph setup->UsbVoltage
		PageGraphUsbVoltNum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("USB voltage"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageGraphWgt(NULL, graphStgs->usbvoltageStgs, PageGraphUsbVoltStr));
		((PageGraphWgt*)pageWgtsLst.back())->SetCaption("USB voltage curve setup");

		//Program configuration->Graph setup->Winding A voltage
		PageGraphVoltANum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("Winding A voltage"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageGraphWgt(NULL, graphStgs->voltAStgs, PageGraphVoltAStr));
		((PageGraphWgt*)pageWgtsLst.back())->SetCaption("Winding A voltage curve setup");

		//Program configuration->Graph setup->Winding B voltage
		PageGraphVoltBNum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("Winding B voltage"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageGraphWgt(NULL, graphStgs->voltBStgs, PageGraphVoltBStr));
		((PageGraphWgt*)pageWgtsLst.back())->SetCaption("Winding B voltage curve setup");

		//Program configuration->Graph setup->Winding C voltage
		PageGraphVoltCNum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("Winding C voltage"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageGraphWgt(NULL, graphStgs->voltCStgs, PageGraphVoltCStr));
		((PageGraphWgt*)pageWgtsLst.back())->SetCaption("Winding C voltage curve setup");

		//Program configuration->Graph setup->EnCurrent
		PageGraphEnCurrNum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("Engine current"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageGraphWgt(NULL, graphStgs->encurrentStgs, PageGraphEnCurrStr));
		((PageGraphWgt*)pageWgtsLst.back())->SetCaption("Eng. current curve setup");

		//Program configuration->Graph setup->PwrCurrent
		PageGraphPwrCurrNum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("Power current"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageGraphWgt(NULL, graphStgs->pwrcurrentStgs, PageGraphPwrCurrStr));
		((PageGraphWgt*)pageWgtsLst.back())->SetCaption("Power current curve setup");

		//Program configuration->Graph setup->UsbCurrent
		PageGraphUsbCurrNum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("USB current"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageGraphWgt(NULL, graphStgs->usbcurrentStgs, PageGraphUsbCurrStr));
		((PageGraphWgt*)pageWgtsLst.back())->SetCaption("Usb current curve setup");

		//Program configuration->Graph setup->Winding A current
		PageGraphCurrANum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("Winding A current"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageGraphWgt(NULL, graphStgs->currAStgs, PageGraphCurrAStr));
		((PageGraphWgt*)pageWgtsLst.back())->SetCaption("Winding A current curve setup");

		//Program configuration->Graph setup->Winding B current
		PageGraphCurrBNum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("Winding B current"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageGraphWgt(NULL, graphStgs->currBStgs, PageGraphCurrBStr));
		((PageGraphWgt*)pageWgtsLst.back())->SetCaption("Winding B current curve setup");

		//Program configuration->Graph setup->Winding C current
		PageGraphCurrCNum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("Winding C current"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageGraphWgt(NULL, graphStgs->currCStgs, PageGraphCurrCStr));
		((PageGraphWgt*)pageWgtsLst.back())->SetCaption("Winding C current curve setup");

		//Program configuration->Graph setup->Pwm
		PageGraphPwmNum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("PWM"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageGraphWgt(NULL, graphStgs->pwmStgs, PageGraphPwmStr));
		((PageGraphWgt*)pageWgtsLst.back())->SetCaption("Pwm curve setup");

		//Program configuration->Graph setup->Temperature
		PageGraphTempNum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("Temperature"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageGraphWgt(NULL, graphStgs->temperatureStgs, PageGraphTempStr));
		((PageGraphWgt*)pageWgtsLst.back())->SetCaption("Temperature curve setup");

		//Program configuration->Graph setup->Joy
		PageGraphJoyNum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("Joystick"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageGraphWgt(NULL, graphStgs->joyStgs, PageGraphJoyStr));
		((PageGraphWgt*)pageWgtsLst.back())->SetCaption("Joystick curve setup");

		//Program configuration->Graph setup->Pot
		PageGraphPotNum=i++;
		childItem = new QTreeWidgetItem(parentItem2);
		childItem->setText(0, tr("Analog in"));
		treeWgtsLst.push_back(childItem);
		pageWgtsLst.push_back((QWidget*)new PageGraphWgt(NULL, graphStgs->potStgs, PageGraphPotStr));
		((PageGraphWgt*)pageWgtsLst.back())->SetCaption("Analog input curve setup");

	//Device configuration->User units settings
	PageUserUnitsNum=i++;
    childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("User units"));
    treeWgtsLst.push_back(childItem);
	pageWgtsLst.push_back((QWidget*)new PageUserUnitsWgt(NULL, uuStgs, devinterface));
	QObject::connect(pageWgtsLst[PageUserUnitsNum], SIGNAL(SgnChangeUU()), this, SLOT(OnChangeUU()));

	//Program configuration->About
	PageAboutNum=i++;
	childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("About"));
	treeWgtsLst.push_back(childItem);
	pageWgtsLst.push_back((QWidget*)new PageAboutWgt(NULL));

	//Stage configuration
	PageRootStageConfigurationNum=i++;
	parentItem = new QTreeWidgetItem(m_ui->treeWgt);
    parentItem->setText(0, tr("Stage"));
	parentWgt = new QWidget();
    treeWgtsLst.push_back(parentItem);
    pageWgtsLst.push_back(parentWgt);

	//PositionerName configuration
	PagePositionerNameNum=i++;
	childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("Positioner name"));
    treeWgtsLst.push_back(childItem);
    pageWgtsLst.push_back((QWidget*)new PagePositionerNameWgt(NULL, stageStgs));


#ifdef SERVICEMODE

	//Stage configuration->stage
	PageStageNum=i++;
	childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("Stage"));
    treeWgtsLst.push_back(childItem);
    pageWgtsLst.push_back((QWidget*)new PageStageWgt(NULL, stageStgs));

	//Stage configuration->DC Motor
	PageStageDCMotorNum=i++;
	childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("Motor"));
	treeWgtsLst.push_back(childItem);
	pageWgtsLst.push_back((QWidget*)new PageStageDCMotorWgt(NULL, stageStgs));

	//Stage configuration->Encoder
	PageStageEncoderNum=i++;
	childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("Encoder"));
	treeWgtsLst.push_back(childItem);
	pageWgtsLst.push_back((QWidget*)new PageStageEncoderWgt(NULL, stageStgs));

	//Stage configuration->Hallsensor
	PageStageHallsensorNum=i++;
	childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("Hall sensor"));
	treeWgtsLst.push_back(childItem);
	pageWgtsLst.push_back((QWidget*)new PageStageHallsensorWgt(NULL, stageStgs));

	//Stage configuration->Gear
	PageStageGearNum=i++;
	childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("Gear"));
	treeWgtsLst.push_back(childItem);
	pageWgtsLst.push_back((QWidget*)new PageStageGearWgt(NULL, stageStgs));

	//Stage configuration->Accessories
	PageStageAccessoriesNum=i++;
	childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, tr("Accessories"));
	treeWgtsLst.push_back(childItem);
	pageWgtsLst.push_back((QWidget*)new PageStageAccessoriesWgt(NULL, stageStgs));
#endif

    m_ui->treeWgt->expandAll();
	treeWgtsLst[PageGraphNum]->setExpanded(false);
	treeWgtsLst[PageUiConfigNum]->setExpanded(false);
	m_ui->treeWgt->setCurrentItem(treeWgtsLst[PageADNum]);
	OnCurItemChanged(treeWgtsLst[PageADNum], m_ui->treeWgt->currentItem());

}

void SettingsDlg::OnCurItemChanged(QTreeWidgetItem *curItem, QTreeWidgetItem *prevItem)
{
    int prevIndex = treeWgtsLst.indexOf(prevItem);
    int curIndex = treeWgtsLst.indexOf(curItem);

    QWidget *prevWgt = pageWgtsLst[prevIndex];
    QWidget *curWgt = pageWgtsLst[curIndex];

    m_ui->scrollAreaWidgetContents->layout()->removeWidget(prevWgt);
    prevWgt->setVisible(false);

	int w = curWgt->minimumSizeHint().width();
	QLayout* ly = m_ui->scrollAreaWidgetContents->layout();
	int w1 = ly->margin();
	int w2 = ly->spacing();
	w += 2*(w1 + w2) + 10;
	m_ui->scrollArea->setMinimumWidth( w );
	m_ui->scrollAreaWidgetContents->layout()->addWidget(curWgt);

	curWgt->setVisible(true);

#ifdef SERVICEMODE
	devinterface->cs->setAnalogEnabled(curIndex == PageCalibNum);
#endif
}

void SettingsDlg::OnBtnBoxClicked(QAbstractButton* button)
{
    switch (m_ui->buttonBox->standardButton(button))
    {
    case QDialogButtonBox::Ok       :   //m_ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
										AllPagesFromUiToClassToDevice();
										config_file = temp_config_file;
										// Bug #65219 This code removes the ability to edit the microstep mode when user units are enabled.
										if (((PageUserUnitsWgt*)pageWgtsLst[PageUserUnitsNum])->enableChkisChecked())
											((PageStepperMotorWgt*)pageWgtsLst[PageStepperMotorNum])->stepmodeBoxDisable();
										else
											((PageStepperMotorWgt*)pageWgtsLst[PageStepperMotorNum])->stepmodeBoxEnable();
                                        break;
    case QDialogButtonBox::Apply    :   //m_ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
										AllPagesFromUiToClassToDevice();
										config_file = temp_config_file;
										// Bug #65219 This code removes the ability to edit the microstep mode when user units are enabled.
										if (((PageUserUnitsWgt*)pageWgtsLst[PageUserUnitsNum])->enableChkisChecked())
											((PageStepperMotorWgt*)pageWgtsLst[PageStepperMotorNum])->stepmodeBoxDisable();
										else
											((PageStepperMotorWgt*)pageWgtsLst[PageStepperMotorNum])->stepmodeBoxEnable();
                                        break;
    case QDialogButtonBox::Reset    :   //m_ui->buttonBox->button(QDialogButtonBox::Reset)->setEnabled(false);
										m_ui->configLbl->setText(getFileName(config_file));
										AllPagesFromDeviceToClassToUi(false);
										temp_config_file = config_file;
                                        break;
    case QDialogButtonBox::Cancel    :  m_ui->configLbl->setText(getFileName(config_file));
										AllPagesFromDeviceToClassToUi(false); // reset interface to device contents because we read FeedbackType from there
										temp_config_file = config_file;
                                        break;
    default:;
    }
}

void SettingsDlg::OnRestoreFlashBtnClicked()
{
	if(!m_ui->configLbl->text().startsWith("*"))
		m_ui->configLbl->setText("*"+m_ui->configLbl->text());

	devinterface->command_read_settings();
    AllPagesFromDeviceToClassToUi();
}

void SettingsDlg::OnSaveFlashBtnClicked()
{
    AllPagesFromUiToClassToDevice();
    AllPagesFromDeviceToClassToUi();
    devinterface->command_save_settings();

	if (motorStgs->name.CtrlFlags & EEPROM_PRECEDENCE)
		emit SgnLog("\"Save to flash\" is useless when EEPROM precedence flag is set: data will be overwritten on EEPROM connection.", SOURCE_XILAB, LOGLEVEL_WARNING);
}

void SettingsDlg::OnSaveFileBtnClicked()
{
	QString filename, save_path;
	if((default_save_path == "") || (m_ui->defaultLocationChk->isChecked()))
		save_path = getDefaultProfilesPath();
	else
		save_path = default_save_path;

	filename = QFileDialog::getSaveFileName(this, tr("Save config file"), save_path + temp_config_file, tr("Config files (*.cfg);;All files (*.*)"));
	
	if(!filename.isEmpty()){
		if(!filename.endsWith(".cfg", Qt::CaseInsensitive)){
            filename+=".cfg";
        }

		QPalette pt;
		pt.setColor(QPalette::Normal, QPalette::WindowText, QColor(0,0,0,255));
		m_ui->configLbl->setPalette(pt);
		m_ui->configLbl->setText(getFileName(filename));

		QFile file(filename);
		file.open(QIODevice::ReadWrite); // because linux won't autocreate on resize
		file.close();
		if (!QFile::resize(filename, 0)) {
			InfoBox infoBox;
			infoBox.setIcon(QMessageBox::Information);
			infoBox.setButtons(QDialogButtonBox::Ok);
			infoBox.setMovieVisible(false);
			infoBox.setText("Can not open file for writing.\nPlease save to another file.");
			infoBox.exec();
			return;
		}
		XSettings settings(filename, QSettings::IniFormat, QIODevice::ReadWrite);
		AllPagesFromUiToSettings(&settings);
		MotorSettings tmpStgs = *motorStgs;
		FromUiToClass(&tmpStgs);
		tmpStgs.FromClassToSettings(&settings);
#ifdef SERVICEMODE
		StageSettings tspStgs = *stageStgs;
		FromUiToClass(&tspStgs);
		tspStgs.FromClassToSettings(&settings);
#endif
		settings.sync();

		//обновляем дефолтные пути
		if(!m_ui->defaultLocationChk->isChecked()){
			QDir dir;
			dir.setPath(filename);
			dir.cdUp();
			default_save_path = dir.path();
		}
	}
}

bool SettingsDlg::VersionValid(QSettings *settings, version_t testing, QString groupname, QList<VersionRange> ok_ranges)
{
	bool is_valid = true;
	settings->beginGroup(groupname);
	int size = settings->childKeys().size();

	for (int i = 1; i <= size; ++i) { // 1-indexed
		QString string = settings->value(QString("Range") + toStr(i)).toString();
		QRegExp rx("^(\\d*(?=\\.|-)|)(\\.\\d*|)(\\.\\d*|)(-)(\\d*(?=\\.|)|)(\\.\\d*|)(\\.\\d*|)$");
		if (!rx.exactMatch(string))
			continue; // skip this line because we couldn't parse it
		QStringList list = rx.capturedTexts();
		VersionRange range;
		QString s;
		s = list.at(1);
		range.start.major = s.isEmpty() ? 0 : s.toUInt();
		s = list.at(2);
		range.start.minor = s.isEmpty() ? 0 : s.remove(0,1).toUInt();
		s = list.at(3);
		range.start.release = s.isEmpty() ? 0 : s.remove(0,1).toUInt();
		s = list.at(4);
		// skip the dash
		s = list.at(5);
		range.end.major = s.isEmpty() ? INT_MAX : s.toUInt();
		s = list.at(6);
		range.end.minor = s.isEmpty() ? INT_MAX : s.remove(0,1).toUInt();
		s = list.at(7);
		range.end.release = s.isEmpty() ? INT_MAX : s.remove(0,1).toUInt();
		ok_ranges.append(range);
		is_valid &= is_three_asc_order(range.start, testing, range.end);
	}
	settings->endGroup();

	return is_valid;

}

bool SettingsDlg::HardwareVersionValid(QSettings *settings, device_information_t device_info)
{
	version_t v;
	v.major = device_info.Major;
	v.minor = device_info.Minor;
	v.release = device_info.Release;
	return VersionValid(settings, v, QString("Supported_hardware"), hw_ok_ranges);
}

bool SettingsDlg::FirmwareVersionValid(QSettings *settings, firmware_version_t fw_info)
{
	version_t v;
	v.major = fw_info.major; 
	v.minor = fw_info.minor;
	v.release = fw_info.release;
	return VersionValid(settings, v, QString("Supported_firmware"), fw_ok_ranges);
}

bool SettingsDlg::CheckCompatibility(QString xilab_ver, firmware_version_t fw_info)
{
	bool res = false;
	unsigned int minor = xilab_ver.split(".")[1].toInt();
	if ((fw_info.major == 3) && (minor <= 14)) res = true;
	if ((fw_info.major == 4) && (minor >= 17)) res = true;
	return res;
}

void SettingsDlg::CheckProfileFwHwVersions(XSettings *settings)
{
	// First we should check if the profile is compatible with current hardware and firmware versions
	QString err = QString("This profile is incompatible with the current version of controller ");
	if (!HardwareVersionValid(settings, controllerStgs->device_info))
		emit SgnLog(err + "hardware.", SOURCE_XILAB, LOGLEVEL_ERROR);
		//emit SgnLog("Hardware version check FAILED.", SOURCE_XILAB, LOGLEVEL_ERROR);
	if (!FirmwareVersionValid(settings, controllerStgs->firmware_version))
		emit SgnLog(err + "firmware.", SOURCE_XILAB, LOGLEVEL_ERROR);
		//emit SgnLog("Hardware version check FAILED.", SOURCE_XILAB, LOGLEVEL_ERROR);
}

void SettingsDlg::CheckProfileFwHwVersions()
{
	if (temp_config_file.isEmpty()) // return so that XSettings doesn't complain about missing file
		return; //forget about checking
	XSettings settings(temp_config_file, QSettings::IniFormat, QIODevice::ReadWrite);
	CheckProfileFwHwVersions(&settings);
}

void SettingsDlg::OnRestoreFileBtnClicked()
{
	QString restoreErrors;
	QString filename, load_path;
	if((default_load_path.isEmpty()) || (m_ui->defaultLocationChk->isChecked()))
		load_path = getDefaultProfilesPath();
	else
		load_path = default_load_path;
	
	filename = QFileDialog::getOpenFileName(this, tr("Open config file"), load_path, tr("Config files (*.cfg);;All files (*.*)"));
	
	//bool err_vers = !CheckCompatibility(QString(XILAB_VERSION), controllerStgs->firmware_version);
	//bool err_path = !filename.contains(load_path, Qt::CaseInsensitive);
	//int ret_messg = QMessageBox::Ok;

	if (!filename.isEmpty()) {
		
			temp_config_file = filename;
			firstLoadConfig = true;
			QPalette pt;
			pt.setColor(QPalette::Normal, QPalette::WindowText, QColor(127, 127, 127, 255));
			m_ui->configLbl->setPalette(pt);
			m_ui->configLbl->setText(getFileName(filename));
			m_ui->configLbl->setToolTip("Settings will be saved into the device when you click OK or Apply button");

			XSettings settings(filename, QSettings::IniFormat, QIODevice::ReadOnly);
			CheckProfileFwHwVersions(&settings);

			for (int i = 0; i < 2; i++) { // We repeat this block twice in a hacky attempt to avoid recalc errors on potential simultaneous change in userunits and feedback when loading settings from file
				// These "1-2-3-4" lines save a copy of motorStgs, load settings from cfg file into motorStgs, load Ui from motorStgs and restore motorStgs to initial state
				MotorSettings saveStgs = *motorStgs; // 1
				motorStgs->FromSettingsToClass(&settings, &restoreErrors); // 2

				((PagePositionerNameWgt*)pageWgtsLst[PagePositionerNameNum])->FromClassToUi();

#ifdef SERVICEMODE
				StageSettings saveStageStgs = *stageStgs;
				stageStgs->FromSettingsToClass(&settings, &restoreErrors);
				
				((PageStageWgt*)pageWgtsLst[PageStageNum])->FromClassToUi();
				((PageStageDCMotorWgt*)pageWgtsLst[PageStageDCMotorNum])->FromClassToUi();
				((PageStageEncoderWgt*)pageWgtsLst[PageStageEncoderNum])->FromClassToUi();
				((PageStageHallsensorWgt*)pageWgtsLst[PageStageHallsensorNum])->FromClassToUi();
				((PageStageGearWgt*)pageWgtsLst[PageStageGearNum])->FromClassToUi();
				((PageStageAccessoriesWgt*)pageWgtsLst[PageStageAccessoriesNum])->FromClassToUi();
#endif
				AllPagesFromDeviceToClassToUi(false, false); // 3 -- note: contains UserUnits->FromClassToUi (emit ?)
				((PageUserUnitsWgt*)pageWgtsLst[PageUserUnitsNum])->SetChecked();

				*motorStgs = saveStgs; // 4
#ifdef SERVICEMODE
				*stageStgs = saveStageStgs;
#endif
				//Interface settings
				restoreErrors += ((PageUserUnitsWgt*)pageWgtsLst[PageUserUnitsNum])->FromSettingsToUi(&settings); // load the userunits first
				setStyles(false); // this is required for settings load after the switch to userunits everywhere in settings
			} // block end

			restoreErrors += ((PageIntrfSettingsWgt*)pageWgtsLst[PageIntrfSettingsNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageCyclicWgt*)pageWgtsLst[PageCyclicNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageUiConfigWgt*)pageWgtsLst[PageUiConfigNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageSliderSetupWgt*)pageWgtsLst[PageSliderSetupNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageLogSettingsWgt*)pageWgtsLst[PageLogSettingsNum])->FromSettingsToUi(&settings);

			//Graph setup
			restoreErrors += ((PageGraphCommonWgt*)pageWgtsLst[PageGraphNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageGraphWgt*)pageWgtsLst[PageGraphPosNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageGraphWgt*)pageWgtsLst[PageGraphSpeedNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageGraphWgt*)pageWgtsLst[PageGraphEncoderNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageGraphWgt*)pageWgtsLst[PageGraphFlagsNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageGraphWgt*)pageWgtsLst[PageGraphEnVoltNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageGraphWgt*)pageWgtsLst[PageGraphEnCurrNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageGraphWgt*)pageWgtsLst[PageGraphPwrVoltNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageGraphWgt*)pageWgtsLst[PageGraphPwrCurrNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageGraphWgt*)pageWgtsLst[PageGraphUsbVoltNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageGraphWgt*)pageWgtsLst[PageGraphUsbCurrNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageGraphWgt*)pageWgtsLst[PageGraphVoltANum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageGraphWgt*)pageWgtsLst[PageGraphVoltBNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageGraphWgt*)pageWgtsLst[PageGraphVoltCNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageGraphWgt*)pageWgtsLst[PageGraphCurrANum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageGraphWgt*)pageWgtsLst[PageGraphCurrBNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageGraphWgt*)pageWgtsLst[PageGraphCurrCNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageGraphWgt*)pageWgtsLst[PageGraphPwmNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageGraphWgt*)pageWgtsLst[PageGraphTempNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageGraphWgt*)pageWgtsLst[PageGraphJoyNum])->FromSettingsToUi(&settings);
			restoreErrors += ((PageGraphWgt*)pageWgtsLst[PageGraphPotNum])->FromSettingsToUi(&settings);

			if (restoreErrors != ""){
				RestoreErrorsDialog *dialog = new RestoreErrorsDialog(this, restoreErrors.split("\n"));
				dialog->show();// >exec();
				//while (dialog->isEnabled())
				//	;
			}

			if (!m_ui->defaultLocationChk->isChecked()){
				//обновляем дефолтные пути
				QDir dir;
				dir.setPath(filename);
				dir.cdUp();
				default_load_path = dir.path();
			}
	}
}

void SettingsDlg::OnCompareFileBtnClicked()
{
	QString load_path;
	if((default_load_path.isEmpty()) || (m_ui->defaultLocationChk->isChecked()))
		load_path = getDefaultPath();
	else
		load_path = default_load_path;

	QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Open two config files"), load_path, tr("Config files (*.cfg);;All files (*.*)"));
	if (!filenames.isEmpty()) {
		if (filenames.count() != 2) {
			QMessageBox::warning(this, "Error", "Please select exactly two settings files to compare.", QMessageBox::Ok);
		}
		else {
			CompareSettingsDialog *dialog = new CompareSettingsDialog(this, filenames);
			dialog->exec();
		}
	}
}	

void SettingsDlg::FromUiToSettings(QSettings *settings)
{
	//геометрия окна
	settings->beginGroup("settingsWindow_params");
	settings->setValue("position", pos());
	settings->setValue("size", size());
	settings->endGroup();

	//дефолтные пути
	settings->beginGroup("default_path");
	settings->setValue("defaultFileLocation", m_ui->defaultLocationChk->isChecked());
	settings->setValue("load_config_path", default_load_path);
	settings->setValue("save_config_path", default_save_path);
	settings->setValue("update_firmware_path", default_firmware_path);
	settings->endGroup();
}

void SettingsDlg::FromSettingsToUi(QSettings *my_settings, QSettings *default_stgs)
{
	QSettings *settings;
	if (my_settings != NULL)
		settings = my_settings;
	else if (default_stgs != NULL)
		settings = default_stgs;
	else
		return;

	int desktop_width = QApplication::desktop()->width();
	int desktop_height = QApplication::desktop()->height();

	//геометрия окна
	settings->beginGroup("settingsWindow_params");

	QPoint point = QPoint(0,0);
	if (settings->contains("position"))
		point = settings->value("position").toPoint();
	move(point);

	QSize size = QSize(600, 920);
	if(settings->contains("size")) {
		size = settings->value("size").toSize();
		size.setWidth(min(size.width(), desktop_width-point.x()));
		size.setHeight(min(size.height(), desktop_height-point.y()));
	}
	resize(size);

	settings->endGroup();

	//дефолтные пути
	settings->beginGroup("default_path");

	if(settings->contains("defaultFileLocation"))
		m_ui->defaultLocationChk->setChecked(settings->value("defaultFileLocation", true).toBool());

	if(settings->contains("load_config_path"))
		default_load_path = settings->value("load_config_path", "").toString();

	if(settings->contains("save_config_path"))
		default_save_path = settings->value("save_config_path", "").toString();

	if(settings->contains("update_firmware_path"))
		default_firmware_path = settings->value("update_firmware_path", "").toString();

	settings->endGroup();
}

//вызывается при закрытии окна
void SettingsDlg::saveWindowParams()
{
	if (!isMultiaxis) {
		//сохраняем положение и размеры окна
		XSettings settings(MakeConfigFilename(), QSettings::IniFormat, QIODevice::ReadWrite);
		FromUiToSettings(&settings);
	}
}

//вызывается при переключении радиобаттонов в motorTypeWgt
//изменения в интерфейсе, зависящие от типа мотора
void SettingsDlg::motorTypeChanged(unsigned int entype)
{
	emit SgnHardwareChanged(entype);
	unsigned int feedback_type;
	feedback_type = ((PageStepperMotorWgt*)pageWgtsLst[PageStepperMotorNum])->OnRadio();
	if (entype == ENGINE_TYPE_DC || entype == ENGINE_TYPE_BRUSHLESS)
	{
		((PageControlPositionWgt*)pageWgtsLst[PageCTPNum])->setPartEnabled(false);
		treeWgtsLst[PagePidNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	}
	else
	{
		treeWgtsLst[PagePidNum]->setFlags((feedback_type == FEEDBACK_ENCODER/* || feedback_type == FEEDBACK_EMF*/) ? (Qt::ItemIsSelectable | Qt::ItemIsEnabled) : (Qt::NoItemFlags));
	}

	treeWgtsLst[PageDCMotorNum]->setFlags((entype == ENGINE_TYPE_DC) ? (Qt::ItemIsSelectable | Qt::ItemIsEnabled) : (Qt::NoItemFlags));
	treeWgtsLst[PageBLDCMotorNum]->setFlags((entype == ENGINE_TYPE_BRUSHLESS) ? (Qt::ItemIsSelectable | Qt::ItemIsEnabled) : (Qt::NoItemFlags));
	treeWgtsLst[PageStepperMotorNum]->setFlags((entype == ENGINE_TYPE_STEP) ? (Qt::ItemIsSelectable | Qt::ItemIsEnabled) : (Qt::NoItemFlags));

	//treeWgtsLst[PagePidNum]->setFlags((entype != ENGINE_TYPE_STEP) ? (Qt::ItemIsSelectable | Qt::ItemIsEnabled) : (Qt::NoItemFlags));
#ifdef SERVICEMODE  // refs to #23173
	treeWgtsLst[PagePidNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
#endif

	treeWgtsLst[PagePowerNum]->setFlags((entype == ENGINE_TYPE_STEP) ? (Qt::ItemIsSelectable | Qt::ItemIsEnabled) : (Qt::NoItemFlags));

	((PagePidWgt*)pageWgtsLst[PagePidNum])->SetMotorType(entype);

	if (entype == ENGINE_TYPE_DC || entype == ENGINE_TYPE_BRUSHLESS)
	{
		OnSwitchCTP(FEEDBACK_ENCODER);
//		((PageBLDCMotorWgt*)pageWgtsLst[PageBLDCMotorNum])->FromClassToUi(true);
//		setStyles();
	}
}

//вызывается при переключении радиобаттонов feedback в StepperMotorWgt
void SettingsDlg::StepperMotorChanged(unsigned int feedback_type)
{
	treeWgtsLst[PagePidNum]->setFlags((/*feedback_type == FEEDBACK_EMF ||*/ feedback_type == FEEDBACK_ENCODER) ? (Qt::ItemIsSelectable | Qt::ItemIsEnabled) : (Qt::NoItemFlags));
#ifdef SERVICEMODE  // refs to #23173
	treeWgtsLst[PagePidNum]->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
#endif
}

void SettingsDlg::SkinTypeChanged(SkinType skin_type){
	if(!isMultiaxis){
		treeWgtsLst[PageSliderSetupNum]->setFlags((skin_type == GeneralSkin) ? (Qt::ItemIsSelectable | Qt::ItemIsEnabled) : (Qt::NoItemFlags));
		treeWgtsLst[PageIntrfSettingsNum]->setFlags((skin_type == AttenuatorSkin) ? (Qt::ItemIsSelectable | Qt::ItemIsEnabled) : (Qt::NoItemFlags));

	}
}

void SettingsDlg::setStyles() // for the self-emitted signal
{
	setStyles(true);
}

void SettingsDlg::setStyles(bool permanent)
{
	multiplier_t old_multi, new_multi;
	old_multi.counts_per_turn = motorStgs->feedback.CountsPerTurn; // actually... (TODO: CHECK)
	old_multi.steps_per_turn = motorStgs->engine.StepsPerRev; // ...who cares about these two?
	old_multi.fbtype = old_feedback_type; // this one is important, though

	MotorSettings tempStgs = *motorStgs;

	save_tmp_uuStgs = *uuStgs;  // Save internal settings because the change might be not permanent and we should not touch them until we close settings
	prev_tmp_uuStgs = curr_tmp_uuStgs;  // Save previous settings before loading new ones from ui because we need up-to-date information each time
	((PageUserUnitsWgt*)pageWgtsLst[PageUserUnitsNum])->FromUiToClass(); // load potentially changed user units to uustgs class
	curr_tmp_uuStgs = *uuStgs; // copy uustgs class for comparison
	*uuStgs = save_tmp_uuStgs; // restore (potentially changed by ui load) user units settings back

	if (permanent) {// if this is a permanent change, then it might be one done on startup, load from class to ui then
		// Call all Ui loaders with emitters disabled, else they will also call setStyles through signal connections and mess everything up
		((PageDCMotorWgt*)pageWgtsLst[PageDCMotorNum])->FromClassToUi(false);
		((PageBLDCMotorWgt*)pageWgtsLst[PageBLDCMotorNum])->FromClassToUi(false);
		((PageStepperMotorWgt*)pageWgtsLst[PageStepperMotorNum])->FromClassToUi(false);
		((PageHomePositionWgt*)pageWgtsLst[PageHomePositionNum])->FromClassToUi();
		((PageCTLWgt*)pageWgtsLst[PageCTLNum])->FromClassToUi();
	}
	FromUiToClass(&tempStgs);
	new_multi.counts_per_turn = tempStgs.feedback.CountsPerTurn;
	new_multi.steps_per_turn = tempStgs.engine.StepsPerRev;
	new_multi.fbtype = tempStgs.feedback.FeedbackType;

	if (permanent) {
		StepSpinBox::setShowStyleAll(controllerStgs->serialnumber, permanent, saved_uuStgs, *uuStgs, old_multi, new_multi);
	} else {
		StepSpinBox::setShowStyleAll(controllerStgs->serialnumber, permanent, prev_tmp_uuStgs, curr_tmp_uuStgs, old_multi, new_multi);
	}
	//StepSpinBox::setStepFactorAll(controllerStgs->serialnumber, permanent, tempStgs.getStepFrac());

	// We do the resaving one more time because setShowStyle call above could have affected the userunits ui page through recalc via feedback change and we need most recent values in curr_tmp_uuStgs for the next setStyles call
	save_tmp_uuStgs = *uuStgs;  // Save internal settings
	prev_tmp_uuStgs = curr_tmp_uuStgs;  // Save previous settings
	((PageUserUnitsWgt*)pageWgtsLst[PageUserUnitsNum])->FromUiToClass(); // load potentially changed user units to uustgs class
	curr_tmp_uuStgs = *uuStgs; // copy uustgs class for comparison
	*uuStgs = save_tmp_uuStgs; // restore (potentially changed by ui load) user units settings back
}

void SettingsDlg::OnSwitchCTP(unsigned int new_feedback_type)
{
	((PageControlPositionWgt*)pageWgtsLst[PageCTPNum])->setPartEnabled(!isSpeedUnitRotation(new_feedback_type));
	if (old_feedback_type != new_feedback_type) {
		setStyles(false);
	}
	old_feedback_type = new_feedback_type;
}

void SettingsDlg::OnChangeUU()
{
	setStyles(false);
}

void SettingsDlg::OnChangeFrac(unsigned int value)
{
	StepSpinBox::setStepFactorAll(controllerStgs->serialnumber, false, value);
}

void SettingsDlg::OnNetResult(QString text, bool flag, int loglevel)
{
	Q_UNUSED(flag)
	emit SgnLog(text, SOURCE_XILAB, loglevel);
}

unsigned int SettingsDlg::getCurrentMotorType()
{
	return ((PageMotorTypeWgt*)pageWgtsLst[PageMotorTypeNum])->getCurrentMotorType();
}

void SettingsDlg::AllPagesFromUiToSettings(QSettings *settings)
{
	// Here when saving the order does not matter
	((PageUserUnitsWgt*)pageWgtsLst[PageUserUnitsNum])->FromUiToSettings(settings);
	((PageUiConfigWgt*)pageWgtsLst[PageUiConfigNum])->FromUiToSettings(settings);
	((PageSliderSetupWgt*)pageWgtsLst[PageSliderSetupNum])->FromUiToSettings(settings);
    
	((PageCyclicWgt*)pageWgtsLst[PageCyclicNum])->FromUiToSettings(settings);
	((PageLogSettingsWgt*)pageWgtsLst[PageLogSettingsNum])->FromUiToSettings(settings);
	//Graph setup
	((PageGraphCommonWgt*)pageWgtsLst[PageGraphNum])->FromUiToSettings(settings);
	((PageGraphWgt*)pageWgtsLst[PageGraphPosNum])->FromUiToSettings(settings);
	((PageGraphWgt*)pageWgtsLst[PageGraphSpeedNum])->FromUiToSettings(settings);
	((PageGraphWgt*)pageWgtsLst[PageGraphEncoderNum])->FromUiToSettings(settings);
	((PageGraphWgt*)pageWgtsLst[PageGraphFlagsNum])->FromUiToSettings(settings);
	((PageGraphWgt*)pageWgtsLst[PageGraphEnVoltNum])->FromUiToSettings(settings);
	((PageGraphWgt*)pageWgtsLst[PageGraphEnCurrNum])->FromUiToSettings(settings);
	((PageGraphWgt*)pageWgtsLst[PageGraphPwrVoltNum])->FromUiToSettings(settings);
	((PageGraphWgt*)pageWgtsLst[PageGraphPwrCurrNum])->FromUiToSettings(settings);
	((PageGraphWgt*)pageWgtsLst[PageGraphUsbVoltNum])->FromUiToSettings(settings);
	((PageGraphWgt*)pageWgtsLst[PageGraphUsbCurrNum])->FromUiToSettings(settings);
	((PageGraphWgt*)pageWgtsLst[PageGraphVoltANum])->FromUiToSettings(settings);
	((PageGraphWgt*)pageWgtsLst[PageGraphVoltBNum])->FromUiToSettings(settings);
	((PageGraphWgt*)pageWgtsLst[PageGraphVoltCNum])->FromUiToSettings(settings);
	((PageGraphWgt*)pageWgtsLst[PageGraphCurrANum])->FromUiToSettings(settings);
	((PageGraphWgt*)pageWgtsLst[PageGraphCurrBNum])->FromUiToSettings(settings);
	((PageGraphWgt*)pageWgtsLst[PageGraphCurrCNum])->FromUiToSettings(settings);
	((PageGraphWgt*)pageWgtsLst[PageGraphPwmNum])->FromUiToSettings(settings);
	((PageGraphWgt*)pageWgtsLst[PageGraphTempNum])->FromUiToSettings(settings);
	((PageGraphWgt*)pageWgtsLst[PageGraphJoyNum])->FromUiToSettings(settings);
	((PageGraphWgt*)pageWgtsLst[PageGraphPotNum])->FromUiToSettings(settings);
	((PageIntrfSettingsWgt*)pageWgtsLst[PageIntrfSettingsNum])->FromUiToSettings(settings);
}

void SettingsDlg::AllPagesFromSettingsToUi(QSettings *settings, QSettings *default_stgs)
{
	((PageUiConfigWgt*)pageWgtsLst[PageUiConfigNum])->FromSettingsToUi(settings, default_stgs);
	((PageSliderSetupWgt*)pageWgtsLst[PageSliderSetupNum])->FromSettingsToUi(settings, default_stgs);
	((PageCyclicWgt*)pageWgtsLst[PageCyclicNum])->FromSettingsToUi(settings, default_stgs);
	((PageUserUnitsWgt*)pageWgtsLst[PageUserUnitsNum])->FromSettingsToUi(settings, default_stgs); // must be loaded after cyclic and slider
	//setStyles(false);

	//GraphSetup
	((PageGraphCommonWgt*)pageWgtsLst[PageGraphNum])->FromSettingsToUi(settings, default_stgs);
	((PageGraphWgt*)pageWgtsLst[PageGraphPosNum])->FromSettingsToUi(settings, default_stgs);
	((PageGraphWgt*)pageWgtsLst[PageGraphSpeedNum])->FromSettingsToUi(settings, default_stgs);
	((PageGraphWgt*)pageWgtsLst[PageGraphEncoderNum])->FromSettingsToUi(settings, default_stgs);
	((PageGraphWgt*)pageWgtsLst[PageGraphFlagsNum])->FromSettingsToUi(settings, default_stgs);
	((PageGraphWgt*)pageWgtsLst[PageGraphEnVoltNum])->FromSettingsToUi(settings, default_stgs);
	((PageGraphWgt*)pageWgtsLst[PageGraphEnCurrNum])->FromSettingsToUi(settings, default_stgs);
	((PageGraphWgt*)pageWgtsLst[PageGraphPwrVoltNum])->FromSettingsToUi(settings, default_stgs);
	((PageGraphWgt*)pageWgtsLst[PageGraphPwrCurrNum])->FromSettingsToUi(settings, default_stgs);
	((PageGraphWgt*)pageWgtsLst[PageGraphUsbVoltNum])->FromSettingsToUi(settings, default_stgs);
	((PageGraphWgt*)pageWgtsLst[PageGraphUsbCurrNum])->FromSettingsToUi(settings, default_stgs);
	((PageGraphWgt*)pageWgtsLst[PageGraphCurrANum])->FromSettingsToUi(settings, default_stgs);
	((PageGraphWgt*)pageWgtsLst[PageGraphCurrBNum])->FromSettingsToUi(settings, default_stgs);
	((PageGraphWgt*)pageWgtsLst[PageGraphCurrCNum])->FromSettingsToUi(settings, default_stgs);
	((PageGraphWgt*)pageWgtsLst[PageGraphVoltANum])->FromSettingsToUi(settings, default_stgs);
	((PageGraphWgt*)pageWgtsLst[PageGraphVoltBNum])->FromSettingsToUi(settings, default_stgs);
	((PageGraphWgt*)pageWgtsLst[PageGraphVoltCNum])->FromSettingsToUi(settings, default_stgs);
	((PageGraphWgt*)pageWgtsLst[PageGraphPwmNum])->FromSettingsToUi(settings, default_stgs);
	((PageGraphWgt*)pageWgtsLst[PageGraphTempNum])->FromSettingsToUi(settings, default_stgs);
	((PageGraphWgt*)pageWgtsLst[PageGraphJoyNum])->FromSettingsToUi(settings, default_stgs);
	((PageGraphWgt*)pageWgtsLst[PageGraphPotNum])->FromSettingsToUi(settings, default_stgs);
	((PageIntrfSettingsWgt*)pageWgtsLst[PageIntrfSettingsNum])->FromSettingsToUi(settings, default_stgs);
}

QString SettingsDlg::MakeConfigFilename()
{
	QString prefix = controllerStgs->device_info.ManufacturerId;
	uint32_t sn = controllerStgs->serialnumber;
	return getDefaultPath() + "/" + prefix + toStr(sn)+".cfg";
}

void SettingsDlg::UpdateCalibration()
{
	emit SgnUpdateCalibration(*uuStgs, motorStgs->engine.MicrostepMode);
}

QString SettingsDlg::getStepFracString(int value1, int value2, bool pad)
{
	int digits = 1;
	if (motorStgs->getStepFrac() > 99)
		digits = 3;
	else if (motorStgs->getStepFrac() > 9)
		digits = 2;

	if(motorStgs->getStepFrac() == 1)
		return QString("%1").arg(value1);
	else if (!pad)
		return QString("%1 %2/%3").arg(value1).arg(value2).arg(motorStgs->getStepFrac());
	else if (value1 < 0)
		return QString("%1 -%2/%3").arg(value1).arg(abs(value2), digits, 10, QChar('0')).arg(motorStgs->getStepFrac());
	else
		return QString("%1 %2/%3").arg(value1).arg(value2, (value2 < 0) ? digits+1 : digits, 10, QChar('0')).arg(motorStgs->getStepFrac());
}

QString SettingsDlg::getFormattedPosition()
{
	QString value;
	Cactus *cs = devinterface->cs;
	if (cs->useCalibration()) {
		value = QString("%1 %2").arg(cs->statusCalb().CurPosition, 0, 'f', uuStgs->precision).arg(uuStgs->unitName);
	} else {
		if (!isSpeedUnitRotation(motorStgs->feedback.FeedbackType)) {
			value = getStepFracString(cs->status().CurPosition, cs->status().uCurPosition, true);
		} else {
			value = QString("%1").arg(cs->status().CurPosition);
		}
	}
	return value;
}

QString SettingsDlg::getFormattedSpeed()
{
	QString str;
	Cactus *cs = devinterface->cs;
	QString speed_unit = uuStgs->getSuffix(UserUnitSettings::TYPE_SPEED, motorStgs->feedback.FeedbackType);
	if (cs->useCalibration()) {
		float speed = cs->statusCalb().CurSpeed;
		if (isSpeedUnitRotation(motorStgs->feedback.FeedbackType)) {
			speed *= (float)motorStgs->feedback.CountsPerTurn / 60.0F;
		}
		// 
		if (((cs->statusCalb().MvCmdSts & MVCMD_RUNNING) != 0) && (speed == 0))
			str = QString("~%1 %2").arg(speed, 0, 'f', uuStgs->precision).arg(speed_unit);
		else
			str = QString("%1 %2").arg(speed, 0, 'f', uuStgs->precision).arg(speed_unit);
	} else {
		int speed = cs->status().CurSpeed;
		if (isSpeedUnitRotation(motorStgs->feedback.FeedbackType)) {
			// 
			if (((cs->status().MvCmdSts & MVCMD_RUNNING) != 0) && (speed == 0))
				str = QString("~%1 %2").arg(speed).arg(speed_unit);
			else
				str = QString("%1 %2").arg(speed).arg(speed_unit);
		} else {
			str = getStepFracString(speed, cs->status().uCurSpeed, false) + speed_unit;
		}
	}
	return str;
}
