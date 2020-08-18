#ifndef AXISOBJECT_H
#define AXISOBJECT_H

#include <QObject>
#include "deviceinterface.h"

class AxisObject : public QObject
{
    Q_OBJECT

public:
    AxisObject(DeviceInterface* _devinterface);

signals:
	void log_signal(QString) const;

public slots:
	void command_reset() const;
	void command_right() const;
	void command_left() const;
	void command_stop() const;
	void command_sstp() const;
	void command_move(int pos, int upos = 0) const;
	void command_movr(int pos, int upos = 0) const;
	void command_move_calb(float fpos, calibration_t calb) const;
	void command_movr_calb(float fpos, calibration_t calb) const;
	void command_zero() const;
	void command_home() const;
	void command_clear_fram() const;
	void command_loft() const;
	void command_power_off() const;
	status_t get_status() const;
	status_calb_t get_status_calb(calibration_t calb) const;
	void wait_for_stop(int refresh_period) const;
	void wait_for_stop() const;
	void command_wait_for_stop(int refresh_period) const;
	void command_homezero() const;	

void set_feedback_settings(feedback_settings_t s) const;
feedback_settings_t get_feedback_settings() const;
void set_home_settings(home_settings_t s) const;
void set_home_settings_calb(home_settings_calb_t s, calibration_t calb) const;
home_settings_t get_home_settings() const;
home_settings_calb_t get_home_settings_calb(calibration_t calb) const;
void set_move_settings(move_settings_t s) const;
void set_move_settings_calb(move_settings_calb_t s, calibration_t calb) const;
move_settings_t get_move_settings() const;
move_settings_calb_t get_move_settings_calb(calibration_t calb) const;
void set_engine_settings(engine_settings_t s) const;
void set_engine_settings_calb(engine_settings_calb_t s, calibration_t calb) const;
engine_settings_t get_engine_settings() const;
engine_settings_calb_t get_engine_settings_calb(calibration_t calb) const;
void set_entype_settings(entype_settings_t s) const;
entype_settings_t get_entype_settings() const;
void set_power_settings(power_settings_t s) const;
power_settings_t get_power_settings() const;
void set_secure_settings(secure_settings_t s) const;
secure_settings_t get_secure_settings() const;
void set_edges_settings(edges_settings_t s) const;
void set_edges_settings_calb(edges_settings_calb_t s, calibration_t calb) const;
edges_settings_t get_edges_settings() const;
edges_settings_calb_t get_edges_settings_calb(calibration_t calb) const;
void set_pid_settings(pid_settings_t s) const;
pid_settings_t get_pid_settings() const;
void set_sync_in_settings(sync_in_settings_t s) const;
void set_sync_in_settings_calb(sync_in_settings_calb_t s, calibration_t calb) const;
sync_in_settings_t get_sync_in_settings() const;
sync_in_settings_calb_t get_sync_in_settings_calb(calibration_t calb) const;
void set_sync_out_settings(sync_out_settings_t s) const;
void set_sync_out_settings_calb(sync_out_settings_calb_t s, calibration_t calb) const;
sync_out_settings_t get_sync_out_settings() const;
sync_out_settings_calb_t get_sync_out_settings_calb(calibration_t calb) const;
void set_extio_settings(extio_settings_t s) const;
extio_settings_t get_extio_settings() const;
void set_brake_settings(brake_settings_t s) const;
brake_settings_t get_brake_settings() const;
void set_control_settings(control_settings_t s) const;
void set_control_settings_calb(control_settings_calb_t s, calibration_t calb) const;
control_settings_t get_control_settings() const;
control_settings_calb_t get_control_settings_calb(calibration_t calb) const;
void set_joystick_settings(joystick_settings_t s) const;
joystick_settings_t get_joystick_settings() const;
void set_ctp_settings(ctp_settings_t s) const;
ctp_settings_t get_ctp_settings() const;
void set_uart_settings(uart_settings_t s) const;
uart_settings_t get_uart_settings() const;
void set_controller_name(controller_name_t s) const;
controller_name_t get_controller_name() const;
get_position_t get_position() const;
get_position_calb_t get_position_calb(calibration_t calb) const;
void set_position(set_position_t s) const;
void set_position_calb(set_position_calb_t s, calibration_t calb) const;
chart_data_t get_chart_data() const;
analog_data_t get_analog_data() const;
debug_read_t get_debug_read() const;
void set_stage_name(stage_name_t s) const;
stage_name_t get_stage_name() const;
void set_stage_information(stage_information_t s) const;
stage_information_t get_stage_information() const;
void set_stage_settings(stage_settings_t s) const;
stage_settings_t get_stage_settings() const;
void set_motor_information(motor_information_t s) const;
motor_information_t get_motor_information() const;
void set_motor_settings(motor_settings_t s) const;
motor_settings_t get_motor_settings() const;
void set_encoder_information(encoder_information_t s) const;
encoder_information_t get_encoder_information() const;
void set_encoder_settings(encoder_settings_t s) const;
encoder_settings_t get_encoder_settings() const;
void set_hallsensor_information(hallsensor_information_t s) const;
hallsensor_information_t get_hallsensor_information() const;
void set_hallsensor_settings(hallsensor_settings_t s) const;
hallsensor_settings_t get_hallsensor_settings() const;
void set_gear_information(gear_information_t s) const;
gear_information_t get_gear_information() const;
void set_gear_settings(gear_settings_t s) const;
gear_settings_t get_gear_settings() const;
void set_accessories_settings(accessories_settings_t s) const;
accessories_settings_t get_accessories_settings() const;
void command_save_settings() const;
void command_read_settings() const;
device_information_t get_device_information() const;

private:
	DeviceInterface* devinterface;
};

#endif
