#include <motorsettings.h>
#include <functions.h>
#include <stdio.h>

#define RETRY_SEND(STRUCT) \
		i = 0; \
		do { result = devinterface->get_##STRUCT##_settings(&STRUCT); i++; } \
		while (result != result_ok && i < RETRIES); \
		if (result != result_ok) { \
			memset(&STRUCT, '\0', sizeof(STRUCT));\
		}

#define GET(WHAT)\
	if(need_##WHAT##_load || load_all_settings)\
	{\
		RETRY_SEND(WHAT);\
		need_##WHAT##_load = false;\
	}

#define SET(WHAT)\
	if(need_##WHAT##_save)\
	{\
		set_##WHAT##_result = devinterface->set_##WHAT##_settings(&WHAT);\
		if (set_##WHAT##_result == result_value_error)\
			need_##WHAT##_load = true;\
	}

#define RETRY_SEND_X(STRUCT, FR_NAME) \
		i = 0; \
		do { result = devinterface->get_##FR_NAME(&STRUCT); i++; } \
		while (result != result_ok && i < RETRIES); \
		if (result != result_ok) { \
			memset(&STRUCT, '\0', sizeof(STRUCT));\
		}

#define GET_X(WHAT, FR_NAME)\
	if(need_##WHAT##_load || load_all_settings)\
	{\
		RETRY_SEND_X(WHAT, FR_NAME);\
		need_##WHAT##_load = false;\
	}

#define SET_X(WHAT, FR_NAME)\
	if(need_##WHAT##_save)\
	{\
		set_##WHAT##_result = devinterface->set_##FR_NAME(&WHAT);\
		if (set_##WHAT##_result == result_value_error)\
			need_##WHAT##_load = true;\
	}

MotorSettings::MotorSettings(DeviceInterface *_devinterface)
{
	devinterface = _devinterface;
	//инициализация флагов
	engine.EngineFlags = 0;
	move.MoveFlags = 0;
	sync_in.SyncInFlags = 0;
	sync_out.SyncOutFlags = 0;
	edges.BorderFlags = 0;
	edges.EnderFlags = 0;
	secure.Flags = 0;
	feedback.FeedbackFlags = 0;
	home.HomeFlags = 0;
	power.PowerFlags = 0;
	brake.BrakeFlags = 0;
	control.Flags = 0;
	ctp.CTPFlags = 0;
	entype.EngineType = 0;
	entype.DriverType = 0;
	uart.UARTSetupFlags = 0;
	joystick.JoyFlags = 0;
	extio.EXTIOModeFlags = 0;
	extio.EXTIOSetupFlags = 0;

	need_edges_save = true;
	need_secure_save = true;
	need_engine_save = true;
	need_entype_save = true;
	need_move_save = true;
	need_feedback_save = true;
	need_power_save = true;
	need_brake_save = true;
	need_control_save = true;
	need_joystick_save = true;
	need_ctp_save = true;
	need_home_save = true;
	need_pid_save = true;
	need_sync_in_save = true;
	need_sync_out_save = true;
	need_uart_save = true;
	need_extio_save = true;
	need_name_save = true;
	need_emf_save = true;

	need_edges_load = false;
	need_secure_load = false;
	need_engine_load = false;
	need_entype_load = false;
	need_move_load = false;
	need_feedback_load = false;
	need_power_load = false;
	need_brake_load = false;
	need_control_load = false;
	need_joystick_load = false;
	need_ctp_load = false;
	need_home_load = false;
	need_pid_load = false;
	need_sync_in_load = false;
	need_sync_out_load = false;
	need_uart_load = false;
	need_extio_load = false;
	need_name_load = false;
	need_emf_load = false;

	name.ControllerName[0] = '\0';

	feedbackTypeList
		<< pair("ENCODER", FEEDBACK_ENCODER)
		<< pair("ENCODER_MEDIATED", FEEDBACK_ENCODER_MEDIATED)
		<< pair("EMF", FEEDBACK_EMF)
		<< pair("NONE", FEEDBACK_NONE)
		;
	
	feedbackEncTypeList
		<< pair("AUTO", FEEDBACK_ENC_TYPE_AUTO)
		<< pair("SINGLE_ENDED", FEEDBACK_ENC_TYPE_SINGLE_ENDED)
		<< pair("DIFFERENTIAL", FEEDBACK_ENC_TYPE_DIFFERENTIAL)
		;

	homeFirstTypeList
		<< pair("REV", HOME_STOP_FIRST_REV)
		<< pair("SYN", HOME_STOP_FIRST_SYN)
		<< pair("LIM", HOME_STOP_FIRST_LIM)
		;

	homeSecondTypeList
		<< pair("REV", HOME_STOP_SECOND_REV)
		<< pair("SYN", HOME_STOP_SECOND_SYN)
		<< pair("LIM", HOME_STOP_SECOND_LIM)
		;

	engineTypeList
		<< pair("DC", ENGINE_TYPE_DC)
		<< pair("2DC", ENGINE_TYPE_2DC)
		<< pair("STEP", ENGINE_TYPE_STEP)
		<< pair("BRUSHLESS", ENGINE_TYPE_BRUSHLESS)
		;

	driverTypeList
		<< pair("DISCRETE_FET", DRIVER_TYPE_DISCRETE_FET)
		<< pair("INTEGRATE", DRIVER_TYPE_INTEGRATE)
		<< pair("EXTERNAL", DRIVER_TYPE_EXTERNAL)
		;
	
	extioInTypeList
		<< pair("IN_NOP", EXTIO_SETUP_MODE_IN_NOP)
		<< pair("IN_STOP", EXTIO_SETUP_MODE_IN_STOP)
		<< pair("IN_PWOF", EXTIO_SETUP_MODE_IN_PWOF)
		<< pair("IN_MOVR", EXTIO_SETUP_MODE_IN_MOVR)
		<< pair("IN_HOME", EXTIO_SETUP_MODE_IN_HOME)
		<< pair("IN_ALARM", EXTIO_SETUP_MODE_IN_ALARM)
		;

	extioOutTypeList
		<< pair("OUT_OFF", EXTIO_SETUP_MODE_OUT_OFF)
		<< pair("OUT_ON", EXTIO_SETUP_MODE_OUT_ON)
		<< pair("OUT_MOVING", EXTIO_SETUP_MODE_OUT_MOVING)
		<< pair("OUT_ALARM", EXTIO_SETUP_MODE_OUT_ALARM)
		<< pair("OUT_MOTOR_ON", EXTIO_SETUP_MODE_OUT_MOTOR_ON)
		;

	controlTypeList
		<< pair("OFF", CONTROL_MODE_OFF)
		<< pair("JOY", CONTROL_MODE_JOY)
		<< pair("LR", CONTROL_MODE_LR)
	;

	uartParityTypeList
		<< pair("EVEN", UART_PARITY_BIT_EVEN)
		<< pair("ODD", UART_PARITY_BIT_ODD)
		<< pair("SPACE", UART_PARITY_BIT_SPACE)
		<< pair("MARK", UART_PARITY_BIT_MARK)
	;

}

MotorSettings::~MotorSettings(void)
{
}

void MotorSettings::LoadFirstInfo()
{
	if (devinterface->getMode() == FIRMWARE_MODE)
	{
		int i;
		result_t result;
		//считываем из контроллера настройки, которые необходимы для гуи:
		//engine.flags & ENGINE_FINISHING - для состояния кнопки Cyclic (активна только при точной доводке)
		//engine.microstep_mode - для отображения деления шага
		RETRY_SEND(engine);
		//при инициализации гуи необходимо знать тип мотора
		RETRY_SEND(entype);
		// границы нужны для отображения на слайдере (Load limits from device) и для режима Cyclic BTB
		RETRY_SEND(edges);
		// для того чтобы узнать, нужно ли отображать энкодер, нужно считать feedback settings
		RETRY_SEND(feedback);
		// ну и конечно имя контроллера надо считать
		RETRY_SEND_X(name, controller_name);
	}
}

void MotorSettings::FromDeviceToClass(bool load_all_settings/* = true*/)
{
	if (devinterface->getMode() != FIRMWARE_MODE)
		return;

	bool temp_state = devinterface->cs->updatesEnabled();
	devinterface->cs->setUpdatesEnabled(false);

    result_t result;
	int i;

	GET(entype);
	GET(edges);
	GET(secure);
	GET(engine);
	GET(move);
	GET(feedback);
	GET(power);
	GET(brake);
	GET(control);
	GET(joystick);
	GET(ctp);
	GET(home);
	GET(pid);
	GET(sync_in);
	GET(sync_out);
	GET(uart);
	GET(extio);
	GET(emf);
	GET_X(name, controller_name)

	devinterface->cs->setUpdatesEnabled(temp_state);
}

bool MotorSettings::FromClassToDevice(void)
{
	if (devinterface->getMode() != FIRMWARE_MODE)
		return false;

	bool temp_state = devinterface->cs->updatesEnabled();
	devinterface->cs->setUpdatesEnabled(false);

	bool ret = true;

	// Feature #1876 - Команда смены типа двигателя должна отсылаться первой
	SET(entype);
	SET(edges);
	SET(secure);
	SET(engine);
	SET(move);
	SET(feedback);
	SET(power);
	SET(brake);
	SET(control);
	SET(joystick);
	SET(ctp);
	SET(home);
	SET(pid);
	SET(sync_in);
	SET(sync_out);
	SET(uart);
	SET(extio);
	SET(emf)
	SET_X(name, controller_name);

	devinterface->cs->setUpdatesEnabled(temp_state);

	return ret;
}

void MotorSettings::FromSettingsToClass(QSettings *settings, QString *errors)
{
	edges.BorderFlags = 0;
	edges.EnderFlags = 0;
	// Border [edges]
	settings->beginGroup("Borders");
	LoadHelperFlag(settings, errors, "Border_is_encoder", &edges.BorderFlags, BORDER_IS_ENCODER);
	LoadHelperSInt(settings, errors, "Left_border", &edges.LeftBorder);
	LoadHelperSInt(settings, errors, "Left_border_usteps", &edges.uLeftBorder);
	LoadHelperSInt(settings, errors, "Right_border", &edges.RightBorder);
	LoadHelperSInt(settings, errors, "Right_border_usteps", &edges.uRightBorder);
	LoadHelperFlag(settings, errors, "Stop_at_left_border", &edges.BorderFlags, BORDER_STOP_LEFT);
	LoadHelperFlag(settings, errors, "Stop_at_right_border", &edges.BorderFlags, BORDER_STOP_RIGHT);
	LoadHelperFlag(settings, errors, "Borders_swap_misset_detection", &edges.BorderFlags, BORDERS_SWAP_MISSET_DETECTION);
	LoadHelperFlag(settings, errors, "Limit_switch_1_pushed_is_closed", &edges.EnderFlags, ENDER_SW1_ACTIVE_LOW);
	LoadHelperFlag(settings, errors, "Limit_switch_2_pushed_is_closed", &edges.EnderFlags, ENDER_SW2_ACTIVE_LOW);
	LoadHelperFlag(settings, errors, "Limit_switch_ender_swap", &edges.EnderFlags, ENDER_SWAP);
	settings->endGroup();
// -----------------------------------------
	secure.Flags = 0;
//	Limits [secure]
	settings->beginGroup("Maximum_ratings");
	LoadHelperFlag(settings, errors, "Low_voltage_protection", &secure.Flags, LOW_UPWR_PROTECTION);
	LoadHelperUInt(settings, errors, "Low_voltage_off", &secure.LowUpwrOff);
	LoadHelperUInt(settings, errors, "Critical_current", &secure.CriticalIpwr);
	LoadHelperUInt(settings, errors, "Critical_voltage", &secure.CriticalUpwr);
	LoadHelperUInt(settings, errors, "Critical_usb_current", &secure.CriticalIusb);
	LoadHelperUInt(settings, errors, "Critical_usb_voltage", &secure.CriticalUusb);
	LoadHelperUInt(settings, errors, "Minimum_usb_voltage", &secure.MinimumUusb);
	LoadHelperUInt(settings, errors, "Critical_temperature", &secure.CriticalT);
	LoadHelperFlag(settings, errors, "Shutdown_on_overheat", &secure.Flags, ALARM_ON_DRIVER_OVERHEATING);
	LoadHelperFlag(settings, errors, "H_bridge_alert", &secure.Flags, H_BRIDGE_ALERT);
	LoadHelperFlag(settings, errors, "Alarm_on_borders_swap_misset", &secure.Flags, ALARM_ON_BORDERS_SWAP_MISSET);
	LoadHelperFlag(settings, errors, "Alarm_flags_sticking", &secure.Flags, ALARM_FLAGS_STICKING);
	LoadHelperFlag(settings, errors, "Usb_break_reconnect", &secure.Flags, USB_BREAK_RECONNECT);
	LoadHelperFlag(settings, errors, "Alarm_winding_mismatch", &secure.Flags, ALARM_WINDING_MISMATCH);
	LoadHelperFlag(settings, errors, "Alarm_engine_response", &secure.Flags, ALARM_ENGINE_RESPONSE);
	settings->endGroup();
// ----------------------
	settings->beginGroup("Motor_type");
	LoadHelperList(settings, errors, "type", &entype.EngineType, engineTypeList);
	settings->endGroup();

	settings->beginGroup("Driver_type");
	LoadHelperList(settings, errors, "type", &entype.DriverType, driverTypeList);
	settings->endGroup();
// --------------------------------------
	engine.EngineFlags = 0;
	move.MoveFlags = 0;
	feedback.FeedbackFlags = 0;
	// Stepper or DC
	settings->beginGroup("Engine");
	LoadHelperFlag(settings, errors, "Max_voltage_enable", &engine.EngineFlags, ENGINE_LIMIT_VOLT); // DC only
	LoadHelperFlag(settings, errors, "Max_current_enable", &engine.EngineFlags, ENGINE_LIMIT_CURR); // DC only
	LoadHelperFlag(settings, errors, "Limit_speed_enable", &engine.EngineFlags, ENGINE_LIMIT_RPM);
	LoadHelperFlag(settings, errors, "Play_compensation_enable", &engine.EngineFlags, ENGINE_ANTIPLAY);
	LoadHelperUInt(settings, errors, "Rated_voltage", &engine.NomVoltage);
	LoadHelperUInt(settings, errors, "Rated_current", &engine.NomCurrent);
	LoadHelperUInt(settings, errors, "Max_speed_steps", &engine.NomSpeed);
	LoadHelperUInt(settings, errors, "Max_speed_usteps", &engine.uNomSpeed);
	LoadHelperSInt(settings, errors, "Play_compensation", &engine.Antiplay);
	LoadHelperFlag(settings, errors, "Reverse_enable", &engine.EngineFlags, ENGINE_REVERSE);
	LoadHelperFlag(settings, errors, "Use_max_speed", &engine.EngineFlags, ENGINE_MAX_SPEED);
	LoadHelperFlag(settings, errors, "Acceleration_enable", &engine.EngineFlags, ENGINE_ACCEL_ON);
	LoadHelperFlag(settings, errors, "Current_as_RMS_enable", &engine.EngineFlags, ENGINE_CURRENT_AS_RMS);
	LoadHelperUInt(settings, errors, "Steps_per_turn", &engine.StepsPerRev);
	LoadHelperUInt(settings, errors, "Microstep_mode", &engine.MicrostepMode);

	LoadHelperUInt(settings, errors, "Speed_steps", &move.Speed);
	LoadHelperUInt(settings, errors, "Speed_usteps", &move.uSpeed);
	LoadHelperUInt(settings, errors, "Antiplay_speed_steps", &move.AntiplaySpeed);
	LoadHelperUInt(settings, errors, "Antiplay_speed_usteps", &move.uAntiplaySpeed);
	LoadHelperUInt(settings, errors, "Acceleration", &move.Accel);
	LoadHelperUInt(settings, errors, "Deceleration", &move.Decel);
	LoadHelperFlag(settings, errors, "Divider_RPM", &move.MoveFlags, RPM_DIV_1000);

	LoadHelperList(settings, errors, "Feedback_type", &feedback.FeedbackType, feedbackTypeList);
	LoadHelperList(settings, errors, "Feedback_enc_type", &feedback.FeedbackFlags, feedbackEncTypeList); // should happen before flag loads below
	LoadHelperUInt(settings, errors, "Encoder_CPT", &feedback.CountsPerTurn);
	copyOrZero(feedback.CountsPerTurn, &feedback.IPS);
	LoadHelperFlag(settings, errors, "Encoder_reverse", &feedback.FeedbackFlags, FEEDBACK_ENC_REVERSE);
	settings->endGroup();
// ---------------------------------------
	home.HomeFlags = 0;
	// Home
	settings->beginGroup("Home_position");
	LoadHelperFlag(settings, errors, "1st_move_direction_right", &home.HomeFlags, HOME_DIR_FIRST);
	LoadHelperUInt(settings, errors, "1st_move_speed", &home.FastHome);
	LoadHelperUInt(settings, errors, "1st_move_speed_usteps", &home.uFastHome);
	LoadHelperFlag(settings, errors, "2nd_move_direction_right", &home.HomeFlags, HOME_DIR_SECOND);
	LoadHelperUInt(settings, errors, "2nd_move_speed", &home.SlowHome);
	LoadHelperUInt(settings, errors, "2nd_move_speed_usteps", &home.uSlowHome);
	LoadHelperSInt(settings, errors, "standoff", &home.HomeDelta);
	LoadHelperSInt(settings, errors, "standoff_usteps", &home.uHomeDelta);
	LoadHelperFlag(settings, errors, "use_second_phase", &home.HomeFlags, HOME_MV_SEC_EN);
	LoadHelperFlag(settings, errors, "use_half_movement", &home.HomeFlags, HOME_HALF_MV);
	LoadHelperFlag(settings, errors, "use_fast_home", &home.HomeFlags, HOME_USE_FAST);

	unsigned int first = 0;
	LoadHelperList(settings, errors, "first_stop_after", &first, homeFirstTypeList);
	home.HomeFlags |= (first & HOME_STOP_FIRST_BITS);

	unsigned int second = 0;
	LoadHelperList(settings, errors, "second_stop_after", &second, homeSecondTypeList);
	home.HomeFlags |= (second & HOME_STOP_SECOND_BITS);

	settings->endGroup();
// -------------------------------------
	// PID
	settings->beginGroup("PID_control");
	LoadHelperUInt(settings, errors, "Voltage_Kp", &pid.KpU);
	LoadHelperUInt(settings, errors, "Voltage_Ki", &pid.KiU);
	LoadHelperUInt(settings, errors, "Voltage_Kd", &pid.KdU);
	LoadHelperFlt(settings, errors, "Voltage_Kp_float", &pid.Kpf);
	LoadHelperFlt(settings, errors, "Voltage_Ki_float", &pid.Kif);
	LoadHelperFlt(settings, errors, "Voltage_Kd_float", &pid.Kdf);
	settings->endGroup();
// ----------------------------------
	// EMF
	settings->beginGroup("EMF_control");
	LoadHelperFlt(settings, errors, "Inductance_L", &emf.L);
	LoadHelperFlt(settings, errors, "Resistance_R", &emf.R);
	LoadHelperFlt(settings, errors, "EMF_Km", &emf.Km);
	LoadHelperUInt(settings, errors, "BackEMFFlags", &emf.BackEMFFlags);
	settings->endGroup();
// ----------------------------------
	sync_in.SyncInFlags = 0;
	sync_out.SyncOutFlags = 0;
	// TTL [sync_in, sync_out]
	settings->beginGroup("TTL_sync");
	LoadHelperUInt(settings, errors, "Clutter_time", &sync_in.ClutterTime);
	LoadHelperSInt(settings, errors, "Position", &sync_in.Position);
	LoadHelperSInt(settings, errors, "uPosition", &sync_in.uPosition);
	LoadHelperUInt(settings, errors, "Speed", &sync_in.Speed);
	LoadHelperUInt(settings, errors, "uSpeed", &sync_in.uSpeed);
	LoadHelperFlag(settings, errors, "Syncin_enabled", &sync_in.SyncInFlags, SYNCIN_ENABLED);
	LoadHelperFlag(settings, errors, "Syncin_invert", &sync_in.SyncInFlags, SYNCIN_INVERT);
	LoadHelperFlag(settings, errors, "Syncin_gotoposition", &sync_in.SyncInFlags, SYNCIN_GOTOPOSITION);
	LoadHelperFlag(settings, errors, "Syncout_enabled", &sync_out.SyncOutFlags, SYNCOUT_ENABLED);
	LoadHelperFlag(settings, errors, "Syncout_fixed_is_high", &sync_out.SyncOutFlags, SYNCOUT_STATE);
	LoadHelperFlag(settings, errors, "Syncout_invert", &sync_out.SyncOutFlags, SYNCOUT_INVERT);
	LoadHelperFlag(settings, errors, "Syncout_count_in_steps", &sync_out.SyncOutFlags, SYNCOUT_IN_STEPS);
	LoadHelperFlag(settings, errors, "Syncout_onstart_enabled", &sync_out.SyncOutFlags, SYNCOUT_ONSTART);
	LoadHelperFlag(settings, errors, "Syncout_onstop_enabled", &sync_out.SyncOutFlags, SYNCOUT_ONSTOP);
	LoadHelperFlag(settings, errors, "Syncout_onperiod_enabled", &sync_out.SyncOutFlags, SYNCOUT_ONPERIOD);
	LoadHelperUInt(settings, errors, "Syncout_pulse_steps", &sync_out.SyncOutPulseSteps);
	LoadHelperUInt(settings, errors, "Syncout_period", &sync_out.SyncOutPeriod);
	LoadHelperUInt(settings, errors, "Accuracy", &sync_out.Accuracy);
	LoadHelperUInt(settings, errors, "uAccuracy", &sync_out.uAccuracy);
	settings->endGroup();
// ------------------------------
	uart.UARTSetupFlags = 0;
	// Uart [uart]
	settings->beginGroup("Uart");
	LoadHelperUInt(settings, errors, "Speed", &uart.Speed);
	LoadHelperFlag(settings, errors, "Use_parity", &uart.UARTSetupFlags, UART_PARITY_BIT_USE);
	LoadHelperFlag(settings, errors, "One_stop_bit", &uart.UARTSetupFlags, UART_STOP_BIT);

	unsigned int parity_type = 0;
	LoadHelperList(settings, errors, "Parity_type", &parity_type, uartParityTypeList);
	uart.UARTSetupFlags |= (parity_type & UART_PARITY_BITS);

	settings->endGroup();
// -------------------------------
	extio.EXTIOModeFlags = 0;
	extio.EXTIOSetupFlags = 0;
	// Extio [extio]
	settings->beginGroup("Extio");
	LoadHelperFlag(settings, errors, "Extio_as_output", &extio.EXTIOSetupFlags, EXTIO_SETUP_OUTPUT);
	LoadHelperFlag(settings, errors, "Extio_invert", &extio.EXTIOSetupFlags, EXTIO_SETUP_INVERT);

	unsigned int mode_in = 0;
	LoadHelperList(settings, errors, "Mode_in", &mode_in, extioInTypeList);
	extio.EXTIOModeFlags |= (mode_in & EXTIO_SETUP_MODE_IN_BITS);

	unsigned int mode_out = 0;
	LoadHelperList(settings, errors, "Mode_out", &mode_out, extioOutTypeList);
	extio.EXTIOModeFlags |= (mode_out & EXTIO_SETUP_MODE_OUT_BITS);

	settings->endGroup();
// -------------------------------
	// Power [power]
	power.PowerFlags = 0;
	settings->beginGroup("Power");
	LoadHelperUInt(settings, errors, "Hold_current", &power.HoldCurrent);
	LoadHelperUInt(settings, errors, "Current_reduction_delay", &power.CurrReductDelay);
	LoadHelperUInt(settings, errors, "Power_off_delay", &power.PowerOffDelay);
	LoadHelperUInt(settings, errors, "Current_set_time", &power.CurrentSetTime);
	LoadHelperFlag(settings, errors, "Current_reduction_enabled", &power.PowerFlags, POWER_REDUCT_ENABLED);
	LoadHelperFlag(settings, errors, "Power_off_enabled", &power.PowerFlags, POWER_OFF_ENABLED);
	LoadHelperFlag(settings, errors, "Smooth_current_enabled", &power.PowerFlags, POWER_SMOOTH_CURRENT);
	settings->endGroup();
// -------------------------------
	brake.BrakeFlags = 0;
	// Brake [brake]
	settings->beginGroup("Brake");
	LoadHelperFlag(settings, errors, "Brake_enabled", &brake.BrakeFlags, BRAKE_ENABLED);
	LoadHelperFlag(settings, errors, "Power_off_enabled", &brake.BrakeFlags, BRAKE_ENG_PWROFF);
	LoadHelperUInt(settings, errors, "t1", &brake.t1);
	LoadHelperUInt(settings, errors, "t2", &brake.t2);
	LoadHelperUInt(settings, errors, "t3", &brake.t3);
	LoadHelperUInt(settings, errors, "t4", &brake.t4);
	settings->endGroup();
// ---------------------------------
	joystick.JoyFlags = 0;
	control.Flags = 0;
	// CTL [ctl, joystick]
	settings->beginGroup("Control");
	LoadHelperUInt(settings, errors, "Joy_low_end", &joystick.JoyLowEnd);
	LoadHelperUInt(settings, errors, "Joy_center", &joystick.JoyCenter);
	LoadHelperUInt(settings, errors, "Joy_high_end", &joystick.JoyHighEnd);
	LoadHelperUInt(settings, errors, "Exp_factor", &joystick.ExpFactor);
	LoadHelperUInt(settings, errors, "Dead_zone", &joystick.DeadZone);
	LoadHelperFlag(settings, errors, "Joystick_reverse", &joystick.JoyFlags, JOY_REVERSE);

	char* str = new char[16];
	for(int i=0; i<9; i++){
		sprintf (str, "timeout_%d", i+1);
		LoadHelperUInt(settings, errors, str, &control.Timeout[i]);
	}
	for(int i=0; i<10; i++){
		sprintf (str, "speed_%d_steps", i+1);
		LoadHelperUInt(settings, errors, str, &control.MaxSpeed[i]);
		sprintf (str, "speed_%d_usteps", i+1);
		LoadHelperUInt(settings, errors, str, &control.uMaxSpeed[i]);
	}
	delete[] str;

	unsigned int control_mode = 0;
	LoadHelperList(settings, errors, "control_mode", &control_mode, controlTypeList);
	control.Flags |= (control_mode & CONTROL_MODE_BITS);

	LoadHelperFlag(settings, errors, "left_button_pushed", &control.Flags, CONTROL_BTN_LEFT_PUSHED_OPEN);
	LoadHelperFlag(settings, errors, "right_button_pushed", &control.Flags, CONTROL_BTN_RIGHT_PUSHED_OPEN);
	LoadHelperUInt(settings, errors, "MaxClickTime", &control.MaxClickTime);
	LoadHelperSInt(settings, errors, "DeltaPosition", &control.DeltaPosition);
	LoadHelperSInt(settings, errors, "uDeltaPosition", &control.uDeltaPosition);
	settings->endGroup();
// ------------------------------------------
	ctp.CTPFlags = 0;
	// ControlPosition [ctp]
	settings->beginGroup("Control_position");
	LoadHelperFlag(settings, errors, "Position_control_enabled", &ctp.CTPFlags, CTP_ENABLED);
	LoadHelperFlag(settings, errors, "Alarm_on_error_enabled", &ctp.CTPFlags, CTP_ALARM_ON_ERROR);
	LoadHelperFlag(settings, errors, "Error_correction_enabled", &ctp.CTPFlags, CTP_ERROR_CORRECTION);
	LoadHelperFlag(settings, errors, "Rev_sens_inv_enabled", &ctp.CTPFlags, REV_SENS_INV);
	LoadHelperFlag(settings, errors, "Based_on_rev_sens", &ctp.CTPFlags, CTP_BASE);
	LoadHelperUInt(settings, errors, "Min_error", &ctp.CTPMinError);
	settings->endGroup();

// ------------------------------------------
	// Controller name [nmf]
	name.CtrlFlags = 0;
	settings->beginGroup("Controller_name");
	// We do not load controller name from settings
	//LoadHelperStr(settings, errors, "Name", &name.ControllerName[0]);
	LoadHelperFlag(settings, errors, "EEPROM_precedence", &name.CtrlFlags, EEPROM_PRECEDENCE);
	settings->endGroup();

}

bool MotorSettings::FromClassToSettings(QSettings *settings)
{
	QString s;
	// Border [edges]
	settings->beginGroup("Borders");
	settings->setValue("Border_is_encoder", (edges.BorderFlags & BORDER_IS_ENCODER) != 0);
	settings->setValue("Left_border", edges.LeftBorder);
	settings->setValue("Left_border_usteps", edges.uLeftBorder);
	settings->setValue("Right_border", edges.RightBorder);
	settings->setValue("Right_border_usteps", edges.uRightBorder);
	settings->setValue("Stop_at_left_border", (edges.BorderFlags & BORDER_STOP_LEFT) != 0);
	settings->setValue("Stop_at_right_border", (edges.BorderFlags & BORDER_STOP_RIGHT) != 0);
	settings->setValue("Borders_swap_misset_detection", (edges.BorderFlags & BORDERS_SWAP_MISSET_DETECTION) != 0);
	settings->setValue("Limit_switch_1_pushed_is_closed", (edges.EnderFlags & ENDER_SW1_ACTIVE_LOW) != 0);
	settings->setValue("Limit_switch_2_pushed_is_closed", (edges.EnderFlags & ENDER_SW2_ACTIVE_LOW) != 0);
	settings->setValue("Limit_switch_ender_swap", (edges.EnderFlags & ENDER_SWAP) != 0);
	settings->endGroup();

	// Limits [secure]
	settings->beginGroup("Maximum_ratings");
	settings->setValue("Low_voltage_protection", (secure.Flags & LOW_UPWR_PROTECTION) != 0);
	settings->setValue("Low_voltage_off", secure.LowUpwrOff);
	settings->setValue("Critical_current", secure.CriticalIpwr);
	settings->setValue("Critical_voltage", secure.CriticalUpwr);
	settings->setValue("Critical_usb_current", secure.CriticalIusb);
	settings->setValue("Critical_usb_voltage", secure.CriticalUusb);
	settings->setValue("Minimum_usb_voltage", secure.MinimumUusb);
	settings->setValue("Critical_temperature", secure.CriticalT);
	settings->setValue("Shutdown_on_overheat", (secure.Flags & ALARM_ON_DRIVER_OVERHEATING) != 0);
	settings->setValue("H_bridge_alert", (secure.Flags & H_BRIDGE_ALERT) != 0);
	settings->setValue("Alarm_on_borders_swap_misset", (secure.Flags & ALARM_ON_BORDERS_SWAP_MISSET) != 0);
	settings->setValue("Alarm_flags_sticking", (secure.Flags & ALARM_FLAGS_STICKING) != 0);
	settings->setValue("Usb_break_reconnect", (secure.Flags & USB_BREAK_RECONNECT) != 0);
	settings->setValue("Alarm_winding_mismatch", (secure.Flags & ALARM_WINDING_MISMATCH) != 0);
	settings->setValue("Alarm_engine_response", (secure.Flags & ALARM_ENGINE_RESPONSE) != 0);
	settings->endGroup();

	// Motor type and Driver type [entype]
	settings->beginGroup("Motor_type");
	SAVE_KEY(engineTypeList, entype.EngineType, INT_MAX, "type");
	settings->endGroup();

	settings->beginGroup("Driver_type");
	SAVE_KEY(driverTypeList, entype.DriverType, INT_MAX, "type");
	settings->endGroup();

	// StepperMotor [engine, move, feedback]
	settings->beginGroup("Engine");
	settings->setValue("Max_voltage_enable", (engine.EngineFlags & ENGINE_LIMIT_VOLT) != 0); // DC only
	settings->setValue("Max_current_enable", (engine.EngineFlags & ENGINE_LIMIT_CURR) != 0); // DC only
	settings->setValue("Limit_speed_enable", (engine.EngineFlags & ENGINE_LIMIT_RPM) != 0);
	settings->setValue("Play_compensation_enable", (engine.EngineFlags & ENGINE_ANTIPLAY) != 0);
	settings->setValue("Rated_voltage", engine.NomVoltage);
	settings->setValue("Rated_current", engine.NomCurrent);
	settings->setValue("Max_speed_steps", engine.NomSpeed);
	settings->setValue("Max_speed_usteps", engine.uNomSpeed);
	settings->setValue("Play_compensation", engine.Antiplay);
	settings->setValue("Reverse_enable", (engine.EngineFlags & ENGINE_REVERSE) != 0);
	settings->setValue("Use_max_speed", (engine.EngineFlags & ENGINE_MAX_SPEED) != 0);
	settings->setValue("Acceleration_enable", (engine.EngineFlags & ENGINE_ACCEL_ON) != 0);
	settings->setValue("Current_as_RMS_enable", (engine.EngineFlags & ENGINE_CURRENT_AS_RMS) != 0);
	settings->setValue("Steps_per_turn", engine.StepsPerRev);
	settings->setValue("Microstep_mode", engine.MicrostepMode);

	settings->setValue("Speed_steps", move.Speed);
	settings->setValue("Speed_usteps",move.uSpeed);
	settings->setValue("Antiplay_speed_steps", move.AntiplaySpeed);
	settings->setValue("Antiplay_speed_usteps", move.uAntiplaySpeed);
	settings->setValue("Acceleration", move.Accel);
	settings->setValue("Deceleration", move.Decel);
	settings->setValue("Divider_RPM", (move.MoveFlags & RPM_DIV_1000) != 0);

	SAVE_KEY(feedbackTypeList, feedback.FeedbackType, INT_MAX, "Feedback_type");
	settings->setValue("Encoder_CPT", feedback.CountsPerTurn);
	settings->setValue("Encoder_reverse", (feedback.FeedbackFlags & FEEDBACK_ENC_REVERSE) != 0);
	SAVE_KEY(feedbackEncTypeList, feedback.FeedbackFlags, FEEDBACK_ENC_TYPE_BITS, "Feedback_enc_type");
	settings->endGroup();

	// HomePosition [home]
	settings->beginGroup("Home_position");
	settings->setValue("1st_move_direction_right", (home.HomeFlags & HOME_DIR_FIRST) != 0);
	settings->setValue("1st_move_speed", home.FastHome);
	settings->setValue("1st_move_speed_usteps", home.uFastHome);
	settings->setValue("2nd_move_direction_right", (home.HomeFlags & HOME_DIR_SECOND) != 0);
	settings->setValue("2nd_move_speed", home.SlowHome);
	settings->setValue("2nd_move_speed_usteps", home.uSlowHome);
	settings->setValue("standoff", home.HomeDelta);
	settings->setValue("standoff_usteps", home.uHomeDelta);
	settings->setValue("use_second_phase", (home.HomeFlags & HOME_MV_SEC_EN) != 0);
	settings->setValue("use_half_movement", (home.HomeFlags & HOME_HALF_MV) != 0);
	settings->setValue("use_fast_home", (home.HomeFlags & HOME_USE_FAST) != 0);
	SAVE_KEY(homeFirstTypeList, home.HomeFlags, HOME_STOP_FIRST_BITS, "first_stop_after");
	SAVE_KEY(homeSecondTypeList, home.HomeFlags, HOME_STOP_SECOND_BITS, "second_stop_after");
	settings->endGroup();

	// Pid [pid]
	settings->beginGroup("PID_control");
	settings->setValue("Voltage_Kp", pid.KpU);
	settings->setValue("Voltage_Ki", pid.KiU);
	settings->setValue("Voltage_Kd", pid.KdU);
	settings->setValue("Voltage_Kp_float", (double)pid.Kpf);
	settings->setValue("Voltage_Ki_float", (double)pid.Kif);
	settings->setValue("Voltage_Kd_float", (double)pid.Kdf);
	settings->endGroup();

	// EMF
	settings->beginGroup("EMF_control");
	settings->setValue("Inductance_L", (double)emf.L);
	settings->setValue("Resistance_R", (double)emf.R);
	settings->setValue("EMF_Km", (double)emf.Km);
	settings->setValue("BackEMFFlags", emf.BackEMFFlags);
	settings->endGroup();

	// Ttl [sync]
	settings->beginGroup("TTL_sync");
	settings->setValue("Clutter_time", sync_in.ClutterTime);
	settings->setValue("Position", sync_in.Position);
	settings->setValue("uPosition", sync_in.uPosition);
	settings->setValue("Speed", sync_in.Speed);
	settings->setValue("uSpeed", sync_in.uSpeed);
	settings->setValue("Syncin_enabled", (sync_in.SyncInFlags & SYNCIN_ENABLED) != 0);
	settings->setValue("Syncin_invert", (sync_in.SyncInFlags & SYNCIN_INVERT) != 0);
	settings->setValue("Syncin_gotoposition", (sync_in.SyncInFlags & SYNCIN_GOTOPOSITION) != 0);
	settings->setValue("Syncout_enabled", (sync_out.SyncOutFlags & SYNCOUT_ENABLED) != 0);
	settings->setValue("Syncout_fixed_is_high", (sync_out.SyncOutFlags & SYNCOUT_STATE) != 0);
	settings->setValue("Syncout_invert", (sync_out.SyncOutFlags & SYNCOUT_INVERT) != 0);
	settings->setValue("Syncout_count_in_steps", (sync_out.SyncOutFlags & SYNCOUT_IN_STEPS) != 0);
	settings->setValue("Syncout_onstart_enabled", (sync_out.SyncOutFlags & SYNCOUT_ONSTART) != 0);
	settings->setValue("Syncout_onstop_enabled", (sync_out.SyncOutFlags & SYNCOUT_ONSTOP) != 0);
	settings->setValue("Syncout_onperiod_enabled", (sync_out.SyncOutFlags & SYNCOUT_ONPERIOD) != 0);
	settings->setValue("Syncout_pulse_steps", sync_out.SyncOutPulseSteps);
	settings->setValue("Syncout_period", sync_out.SyncOutPeriod);
	settings->setValue("Accuracy", sync_out.Accuracy);
	settings->setValue("uAccuracy", sync_out.uAccuracy);
	settings->endGroup();

	// Uart [uart]
	settings->beginGroup("Uart");
	settings->setValue("Speed", uart.Speed);
	settings->setValue("Use_parity", (uart.UARTSetupFlags & UART_PARITY_BIT_USE) != 0);
	SAVE_KEY(uartParityTypeList, uart.UARTSetupFlags, UART_PARITY_BITS, "Parity_type");
	settings->setValue("One_stop_bit", (uart.UARTSetupFlags & UART_STOP_BIT) != 0);
	settings->endGroup();

	// Extio [extio]
	settings->beginGroup("Extio");
	settings->setValue("Extio_as_output", (extio.EXTIOSetupFlags & EXTIO_SETUP_OUTPUT) != 0);
	settings->setValue("Extio_invert", (extio.EXTIOSetupFlags & EXTIO_SETUP_INVERT) != 0);
	SAVE_KEY(extioInTypeList, extio.EXTIOModeFlags, EXTIO_SETUP_MODE_IN_BITS, "Mode_in");
	SAVE_KEY(extioOutTypeList, extio.EXTIOModeFlags, EXTIO_SETUP_MODE_OUT_BITS, "Mode_out");
	settings->endGroup();

	// Power [power]
	settings->beginGroup("Power");
	settings->setValue("Hold_current", power.HoldCurrent);
	settings->setValue("Current_reduction_delay", power.CurrReductDelay);
	settings->setValue("Power_off_delay", power.PowerOffDelay);
	settings->setValue("Current_set_time", power.CurrentSetTime);
	settings->setValue("Current_reduction_enabled", (power.PowerFlags & POWER_REDUCT_ENABLED) != 0);
	settings->setValue("Power_off_enabled", (power.PowerFlags & POWER_OFF_ENABLED) != 0);
	settings->setValue("Smooth_current_enabled", (power.PowerFlags & POWER_SMOOTH_CURRENT) != 0);
	settings->endGroup();

	// Brake [brake]
	settings->beginGroup("Brake");
	settings->setValue("Brake_enabled", (brake.BrakeFlags & BRAKE_ENABLED) != 0);
	settings->setValue("Power_off_enabled", (brake.BrakeFlags & BRAKE_ENG_PWROFF) != 0);
	settings->setValue("t1", brake.t1);
	settings->setValue("t2", brake.t2);
	settings->setValue("t3", brake.t3);
	settings->setValue("t4", brake.t4);
	settings->endGroup();

	// Control [ctl, joystick]
	settings->beginGroup("Control");
	settings->setValue("Joy_low_end", joystick.JoyLowEnd);
	settings->setValue("Joy_center", joystick.JoyCenter);
	settings->setValue("Joy_high_end",  joystick.JoyHighEnd);
	settings->setValue("Exp_factor", joystick.ExpFactor);
	settings->setValue("Dead_zone", joystick.DeadZone);
	settings->setValue("Joystick_reverse", (joystick.JoyFlags & JOY_REVERSE) != 0);

	for(int i=0; i<9; i++){
		settings->setValue(QString("timeout_%1").arg(i+1), control.Timeout[i]);
	}
	for(int i=0; i<10; i++){
		settings->setValue(QString("speed_%1_steps").arg(i+1), control.MaxSpeed[i]);
		settings->setValue(QString("speed_%1_usteps").arg(i+1), control.uMaxSpeed[i]);
	}

	SAVE_KEY(controlTypeList, control.Flags, CONTROL_MODE_BITS, "control_mode");
	settings->setValue("left_button_pushed", (control.Flags & CONTROL_BTN_LEFT_PUSHED_OPEN) != 0);
	settings->setValue("right_button_pushed", (control.Flags & CONTROL_BTN_RIGHT_PUSHED_OPEN) != 0);
	settings->setValue("MaxClickTime", control.MaxClickTime);
	settings->setValue("DeltaPosition", control.DeltaPosition);
	settings->setValue("uDeltaPosition", control.uDeltaPosition);
	settings->endGroup();

	// ControlPosition [ctp]
	settings->beginGroup("Control_position");
	settings->setValue("Position_control_enabled", (ctp.CTPFlags & CTP_ENABLED) != 0);
	settings->setValue("Alarm_on_error_enabled", (ctp.CTPFlags & CTP_ALARM_ON_ERROR) != 0);
	settings->setValue("Error_correction_enabled", (ctp.CTPFlags & CTP_ERROR_CORRECTION) != 0);
	settings->setValue("Rev_sens_inv_enabled", (ctp.CTPFlags & REV_SENS_INV) != 0);
	settings->setValue("Based_on_rev_sens", (ctp.CTPFlags & CTP_BASE) != 0);
	settings->setValue("Min_error", ctp.CTPMinError);
	settings->endGroup();

	// Controller name [nmf]
	settings->beginGroup("Controller_name");
	// We also do not save controller name to settings
	//settings->setValue("Name", name.ControllerName);
	settings->setValue("EEPROM_precedence", (name.CtrlFlags & EEPROM_PRECEDENCE) != 0);
	settings->endGroup();

	return true;
}

int MotorSettings::getStepFrac()
{
	switch(engine.MicrostepMode){
		case MICROSTEP_MODE_FULL: return 1;
		case MICROSTEP_MODE_FRAC_2: return 2;
		case MICROSTEP_MODE_FRAC_4: return 4;
		case MICROSTEP_MODE_FRAC_8: return 8;
		case MICROSTEP_MODE_FRAC_16: return 16;
		case MICROSTEP_MODE_FRAC_32: return 32;
		case MICROSTEP_MODE_FRAC_64: return 64;
		case MICROSTEP_MODE_FRAC_128: return 128;
		case MICROSTEP_MODE_FRAC_256: return 256;
		default: return 1;
	}
}

void MotorSettings::set_save_true()
{
	need_edges_save = true;
	need_secure_save = true;
	need_engine_save = true;
	need_entype_save = true;
	need_move_save = true;
	need_feedback_save = true;
	need_power_save = true;
	need_brake_save = true;
	need_control_save = true;
	need_joystick_save = true;
	need_ctp_save = true;
	need_home_save = true;
	need_pid_save = true;
	need_emf_save = true;
	need_sync_in_save = true;
	need_sync_out_save = true;
	need_uart_save = true;
	need_extio_save = true;
	need_name_save = true;
}
