#ifndef MOTORSETTINGS_H
#define MOTORSETTINGS_H

#include <deviceinterface.h>
#include <QSettings>

const int RETRIES = 3;

class MotorSettings
{
public:
    MotorSettings(DeviceInterface *_devinterface);
    ~MotorSettings(void);

	void LoadFirstInfo();

	void FromDeviceToClass(bool load_all_settings = true);
	bool FromClassToDevice();

	void FromSettingsToClass(QSettings *settings, QString *errors);
	bool FromClassToSettings(QSettings *settings);

	void set_save_true();
	int getStepFrac();

	analog_data_t analog_data;
    engine_settings_t engine;
	entype_settings_t entype;
    pid_settings_t pid;
	emf_settings_t emf;
	sync_in_settings_t sync_in;
	sync_out_settings_t sync_out;
    edges_settings_t edges;
	secure_settings_t secure;
    move_settings_t move;
    feedback_settings_t feedback;
	home_settings_t home;
	power_settings_t power;
	brake_settings_t brake;
	control_settings_t control;
	joystick_settings_t joystick;
	ctp_settings_t ctp;
	uart_settings_t uart;
	extio_settings_t extio;
	controller_name_t name;

	bool need_edges_save;
	bool need_secure_save;
	bool need_engine_save;
	bool need_entype_save;
	bool need_move_save;
	bool need_feedback_save;
	bool need_power_save;
	bool need_brake_save;
	bool need_control_save;
	bool need_joystick_save;
	bool need_ctp_save;
	bool need_home_save;
	bool need_pid_save;
	bool need_sync_in_save;
	bool need_sync_out_save;
	bool need_uart_save;
	bool need_extio_save;
	bool need_name_save;
	bool need_emf_save;

	bool need_edges_load;
	bool need_secure_load;
	bool need_engine_load;
	bool need_entype_load;
	bool need_move_load;
	bool need_feedback_load;
	bool need_power_load;
	bool need_brake_load;
	bool need_control_load;
	bool need_joystick_load;
	bool need_ctp_load;
	bool need_home_load;
	bool need_pid_load;
	bool need_sync_in_load;
	bool need_sync_out_load;
	bool need_uart_load;
	bool need_extio_load;
	bool need_name_load;
	bool need_emf_load;

private:
	DeviceInterface *devinterface;
	result_t set_edges_result;
	result_t set_secure_result;
	result_t set_engine_result;
	result_t set_entype_result;
	result_t set_move_result;
	result_t set_feedback_result;
	result_t set_power_result;
	result_t set_brake_result;
	result_t set_control_result;
	result_t set_joystick_result;
	result_t set_ctp_result;
	result_t set_home_result;
	result_t set_pid_result;
	result_t set_sync_in_result;
	result_t set_sync_out_result;
	result_t set_uart_result;
	result_t set_extio_result;
	result_t set_name_result;
	result_t set_emf_result;

	QList<QPair<const char*, unsigned int> > feedbackTypeList, feedbackEncTypeList, homeFirstTypeList, homeSecondTypeList, engineTypeList, driverTypeList, extioInTypeList, extioOutTypeList, controlTypeList, uartParityTypeList;
};

#endif // MOTORSETTINGS_H
