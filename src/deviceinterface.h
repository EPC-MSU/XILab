#ifndef DEVICEINTERFACE_H
#define DEVICEINTERFACE_H

#include <QMutex>
#include <QObject>
#include <status.h>

#define UNKNOWN_MODE	0
#define BOOTLOADER_MODE	1
#define FIRMWARE_MODE	2

namespace libximc
{
#include <ximc.h>
}

using namespace libximc;

typedef enum{
	dtUnknown,
	dtSerial,
	dtVirtual,
	dtNet,
	dtUdp,
	dtTcp
} protocol_t;

typedef struct
	{
		status_t status;
		bool status_ok;
		chart_data_t chart_data;
		analog_data_t analog_data;
		status_calb_t status_calb;
		bool status_calb_ok;
		int step_factor;
		bool enabled_calb;
		feedback_settings_t fbs;
	} measurement_t;

class DeviceInterface: public QObject
{
	Q_OBJECT

public:
	DeviceInterface();
	~DeviceInterface();
	Cactus *cs;

	void open_device (const char* name);
	result_t close_device ();
	result_t load_calibration_table(const char* namefile);
	bool is_open ();
	result_t probe_device (const char* name);
	device_enumeration_t enumerate_devices (int probe_devices, const char* hints);
	result_t free_enumerate_devices (device_enumeration_t dev_enum);
	result_t free_enumerate_devices_plain (char* plain_names);
	result_t get_enumerate_device_serial (device_enumeration_t device_enumeration, int device_index, uint32_t* serial);
	result_t get_enumerate_device_information (device_enumeration_t device_enumeration, int device_index, device_information_t* device_information);
	result_t get_enumerate_device_controller_name (device_enumeration_t device_enumeration, int device_index, controller_name_t* controller_name);
	result_t get_enumerate_device_stage_name (device_enumeration_t device_enumeration, int device_index, stage_name_t* stage_name);

	// Группа команд настройки контроллера
	result_t get_feedback_settings (feedback_settings_t* feedback);
	result_t set_feedback_settings (const feedback_settings_t* feedback);
	result_t get_home_settings (home_settings_t* home_settings);
	result_t set_home_settings (const home_settings_t* home_settings);
	result_t get_move_settings (move_settings_t* move_settings);
	result_t set_move_settings (const move_settings_t* move_settings);
	result_t get_engine_settings (engine_settings_t* engine_settings);
	result_t set_engine_settings (const engine_settings_t* engine_settings);
	result_t get_entype_settings (entype_settings_t* entype_settings);
	result_t set_entype_settings (const entype_settings_t* entype_settings);
	result_t get_power_settings (power_settings_t* power_settings);
	result_t set_power_settings (const power_settings_t* power_settings);
	result_t get_secure_settings (secure_settings_t* secure_settings);
	result_t set_secure_settings (const secure_settings_t* secure_settings);
	result_t get_edges_settings (edges_settings_t* edges_settings);
	result_t set_edges_settings (const edges_settings_t* edges_settings);
	result_t get_pid_settings (pid_settings_t* pid_settings);
	result_t set_pid_settings (const pid_settings_t* pid_settings);
	result_t get_sync_in_settings (sync_in_settings_t* sync_in_settings);
	result_t set_sync_in_settings (const sync_in_settings_t* sync_in_settings);
	result_t get_sync_out_settings (sync_out_settings_t* sync_out_settings);
	result_t set_sync_out_settings (const sync_out_settings_t* sync_out_settings);
	result_t get_extio_settings (extio_settings_t *extio_settings);
	result_t set_extio_settings (const extio_settings_t *extio_settings);
	result_t get_brake_settings (brake_settings_t* brake_settings);
	result_t set_brake_settings (const brake_settings_t* brake_settings);
	result_t get_control_settings (control_settings_t* control_settings);
	result_t set_control_settings (const control_settings_t* control_settings);
	result_t get_joystick_settings (joystick_settings_t *joystick_settings);
	result_t set_joystick_settings (const joystick_settings_t *joystick_settings);
	result_t get_ctp_settings (ctp_settings_t* ctp_settings);
	result_t set_ctp_settings (const ctp_settings_t* ctp_settings);
	result_t get_uart_settings (uart_settings_t* uart_settings);
	result_t set_uart_settings (const uart_settings_t* uart_settings);
	result_t get_controller_name (controller_name_t* controller_name);
	result_t set_controller_name (const controller_name_t* controller_name);
	result_t get_nonvolatile_memory (nonvolatile_memory_t* nonvolatile_memory);
	result_t set_nonvolatile_memory (const nonvolatile_memory_t* nonvolatile_memory);
	result_t get_emf_settings(emf_settings_t* emf_settings);
	result_t set_emf_settings(const emf_settings_t* emf_settings);
    result_t get_network_settings(network_settings_t* network_settings);
    result_t set_network_settings(const network_settings_t* network_settings);
    result_t get_password_settings(password_settings_t* network_settings);
    result_t set_password_settings(const password_settings_t* network_settings);


	result_t get_home_settings_calb (home_settings_calb_t* home_settings, calibration_t* calibration);
	result_t set_home_settings_calb (const home_settings_calb_t* home_settings, calibration_t* calibration);
	result_t get_move_settings_calb (move_settings_calb_t* move_settings, calibration_t* calibration);
	result_t set_move_settings_calb (const move_settings_calb_t* move_settings, calibration_t* calibration);
	result_t get_engine_settings_calb (engine_settings_calb_t* engine_settings, calibration_t* calibration);
	result_t set_engine_settings_calb (const engine_settings_calb_t* engine_settings, calibration_t* calibration);
	result_t get_edges_settings_calb (edges_settings_calb_t* edges_settings, calibration_t* calibration);
	result_t set_edges_settings_calb (const edges_settings_calb_t* edges_settings, calibration_t* calibration);
	result_t get_sync_in_settings_calb (sync_in_settings_calb_t* sync_in_settings, calibration_t* calibration);
	result_t set_sync_in_settings_calb (const sync_in_settings_calb_t* sync_in_settings, calibration_t* calibration);
	result_t get_sync_out_settings_calb (sync_out_settings_calb_t* sync_out_settings, calibration_t* calibration);
	result_t set_sync_out_settings_calb (const sync_out_settings_calb_t* sync_out_settings, calibration_t* calibration);
	result_t get_control_settings_calb (control_settings_calb_t* control_settings, calibration_t* calibration);
	result_t set_control_settings_calb (const control_settings_calb_t* control_settings, calibration_t* calibration);
    

	// Группа команд управления движением
	result_t command_power_off ();
	result_t command_stop ();
	result_t command_sstp ();
	result_t command_move (int pos, int upos);
	result_t command_move_calb (float fpos, calibration_t calb);
	result_t command_movr_calb (float fpos, calibration_t calb);
	result_t command_movr (int offset, int uoffset);
	result_t command_home ();
	result_t command_left ();
	result_t command_right ();
	result_t command_loft ();
	result_t command_wait_for_stop (int refresh_period);
	result_t command_homezero ();

	// Группа команд установки текущей позиции
	result_t set_position (const set_position_t* position);
	result_t get_position (get_position_t* position);
	result_t set_position_calb (const set_position_calb_t* position, const calibration_t* calb);
	result_t get_position_calb (get_position_calb_t* position, const calibration_t* calb);
	result_t command_zero ();

	// Группа команд сохранения и загрузки настроек
	result_t command_save_settings ();
	result_t command_read_settings ();
	result_t command_eesave_settings ();
	result_t command_eeread_settings ();

	// Группа команд получения статуса контроллера
	result_t get_status (status_t* state);
	result_t get_status_calb (status_calb_t* status_calb, calibration_t* calibration);
	result_t get_chart_data (chart_data_t* chart_data);
	result_t get_device_information (device_information_t* device_information);
	result_t get_serial_number (uint32_t* serial);

	// Группа команд для работы с прошивкой контроллера
	result_t get_firmware_version (unsigned int* major, unsigned int* minor, unsigned int* release);
	result_t command_update_firmware (const char* name, const uint8_t* data, uint32_t data_size);
	result_t service_command_updf ();

	// Группа сервисных команд
	result_t set_serial_number (const serial_number_t* serial_number);
	result_t get_analog_data (analog_data_t* ad);
	result_t get_debug_read (debug_read_t* debug_read);
	result_t set_debug_write (debug_write_t* debug_write);
	result_t command_clear_fram ();
	result_t get_measurements(measurements_t* measurements);
	result_t command_start_measurements();

	// Группа команда работы с EEPROM подвижки
	result_t get_stage_name (stage_name_t *stage_name);
	result_t get_stage_information (stage_information_t *stage_information);
	result_t get_stage_settings (stage_settings_t *stage_settings);
	result_t get_motor_information (motor_information_t *motor_information);
	result_t get_motor_settings (motor_settings_t *motor_settings);
	result_t get_encoder_information (encoder_information_t *encoder_information);
	result_t get_encoder_settings (encoder_settings_t *encoder_settings);
	result_t get_hallsensor_information (hallsensor_information_t* hallsensor_information);
	result_t get_hallsensor_settings (hallsensor_settings_t* hallsensor_settings);
	result_t get_gear_information (gear_information_t *gear_information);
	result_t get_gear_settings (gear_settings_t *gear_settings);
	result_t get_accessories_settings (accessories_settings_t* accessories_settings);

	result_t set_stage_name (const stage_name_t *stage_name);
	result_t set_stage_information (const stage_information_t *stage_information);
	result_t set_stage_settings (const stage_settings_t *stage_settings);
	result_t set_motor_information (const motor_information_t *motor_information);
	result_t set_motor_settings (const motor_settings_t *motor_settings);
	result_t set_encoder_information (const encoder_information_t *encoder_information);
	result_t set_encoder_settings (const encoder_settings_t *encoder_settings);
	result_t set_hallsensor_information (const hallsensor_information_t* hallsensor_information);
	result_t set_hallsensor_settings (const hallsensor_settings_t* hallsensor_settings);
	result_t set_gear_information (const gear_information_t *gear_information);
	result_t set_gear_settings (const gear_settings_t *gear_settings);
	result_t set_accessories_settings (const accessories_settings_t* accessories_settings);

	// Команды загрузчика
	result_t get_bootloader_version (unsigned int* major, unsigned int* minor, unsigned int* release);
	result_t get_init_random (init_random_t* init_random);
	result_t goto_firmware (uint8_t* ret);
	result_t has_firmware (const char* name, uint8_t* ret);
	result_t write_key (const char* name, uint8_t* key);
	result_t command_reset ();

	int getMode();
	protocol_t getProtocolType();
	device_t getDeviceId();

private:
	result_t result;
	device_t id;
	protocol_t protocol;
	int device_mode;
	device_information_t info;

signals:
	void errorSignal(const char*, result_t);
	void warningSignal(const char*, result_t);
};

extern DeviceInterface* devinterface;

#endif