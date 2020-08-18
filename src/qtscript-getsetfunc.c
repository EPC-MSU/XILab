// WRITER
void AxisObject::set_feedback_settings(feedback_settings_t s) const
{
	devinterface->set_feedback_settings(&s);
}
// READER
feedback_settings_t AxisObject::get_feedback_settings() const
{
	feedback_settings_t s;
	devinterface->get_feedback_settings(&s);
	return s;
}
// WRITER
void AxisObject::set_home_settings(home_settings_t s) const
{
	devinterface->set_home_settings(&s);
}
// WRITER CALB
void AxisObject::set_home_settings_calb(home_settings_calb_t s, calibration_t calb) const
{
	devinterface->set_home_settings_calb(&s, &calb);
}
// READER
home_settings_t AxisObject::get_home_settings() const
{
	home_settings_t s;
	devinterface->get_home_settings(&s);
	return s;
}
// READER CALB
home_settings_calb_t AxisObject::get_home_settings_calb(calibration_t calb) const
{
	home_settings_calb_t s;
	devinterface->get_home_settings_calb(&s, &calb);
	return s;
}
// WRITER
void AxisObject::set_move_settings(move_settings_t s) const
{
	devinterface->set_move_settings(&s);
}
// WRITER CALB
void AxisObject::set_move_settings_calb(move_settings_calb_t s, calibration_t calb) const
{
	devinterface->set_move_settings_calb(&s, &calb);
}
// READER
move_settings_t AxisObject::get_move_settings() const
{
	move_settings_t s;
	devinterface->get_move_settings(&s);
	return s;
}
// READER CALB
move_settings_calb_t AxisObject::get_move_settings_calb(calibration_t calb) const
{
	move_settings_calb_t s;
	devinterface->get_move_settings_calb(&s, &calb);
	return s;
}
// WRITER
void AxisObject::set_engine_settings(engine_settings_t s) const
{
	devinterface->set_engine_settings(&s);
}
// WRITER CALB
void AxisObject::set_engine_settings_calb(engine_settings_calb_t s, calibration_t calb) const
{
	devinterface->set_engine_settings_calb(&s, &calb);
}
// READER
engine_settings_t AxisObject::get_engine_settings() const
{
	engine_settings_t s;
	devinterface->get_engine_settings(&s);
	return s;
}
// READER CALB
engine_settings_calb_t AxisObject::get_engine_settings_calb(calibration_t calb) const
{
	engine_settings_calb_t s;
	devinterface->get_engine_settings_calb(&s, &calb);
	return s;
}
// WRITER
void AxisObject::set_entype_settings(entype_settings_t s) const
{
	devinterface->set_entype_settings(&s);
}
// READER
entype_settings_t AxisObject::get_entype_settings() const
{
	entype_settings_t s;
	devinterface->get_entype_settings(&s);
	return s;
}
// WRITER
void AxisObject::set_power_settings(power_settings_t s) const
{
	devinterface->set_power_settings(&s);
}
// READER
power_settings_t AxisObject::get_power_settings() const
{
	power_settings_t s;
	devinterface->get_power_settings(&s);
	return s;
}
// WRITER
void AxisObject::set_secure_settings(secure_settings_t s) const
{
	devinterface->set_secure_settings(&s);
}
// READER
secure_settings_t AxisObject::get_secure_settings() const
{
	secure_settings_t s;
	devinterface->get_secure_settings(&s);
	return s;
}
// WRITER
void AxisObject::set_edges_settings(edges_settings_t s) const
{
	devinterface->set_edges_settings(&s);
}
// WRITER CALB
void AxisObject::set_edges_settings_calb(edges_settings_calb_t s, calibration_t calb) const
{
	devinterface->set_edges_settings_calb(&s, &calb);
}
// READER
edges_settings_t AxisObject::get_edges_settings() const
{
	edges_settings_t s;
	devinterface->get_edges_settings(&s);
	return s;
}
// READER CALB
edges_settings_calb_t AxisObject::get_edges_settings_calb(calibration_t calb) const
{
	edges_settings_calb_t s;
	devinterface->get_edges_settings_calb(&s, &calb);
	return s;
}
// WRITER
void AxisObject::set_pid_settings(pid_settings_t s) const
{
	devinterface->set_pid_settings(&s);
}
// READER
pid_settings_t AxisObject::get_pid_settings() const
{
	pid_settings_t s;
	devinterface->get_pid_settings(&s);
	return s;
}
// WRITER
void AxisObject::set_sync_in_settings(sync_in_settings_t s) const
{
	devinterface->set_sync_in_settings(&s);
}
// WRITER CALB
void AxisObject::set_sync_in_settings_calb(sync_in_settings_calb_t s, calibration_t calb) const
{
	devinterface->set_sync_in_settings_calb(&s, &calb);
}
// READER
sync_in_settings_t AxisObject::get_sync_in_settings() const
{
	sync_in_settings_t s;
	devinterface->get_sync_in_settings(&s);
	return s;
}
// READER CALB
sync_in_settings_calb_t AxisObject::get_sync_in_settings_calb(calibration_t calb) const
{
	sync_in_settings_calb_t s;
	devinterface->get_sync_in_settings_calb(&s, &calb);
	return s;
}
// WRITER
void AxisObject::set_sync_out_settings(sync_out_settings_t s) const
{
	devinterface->set_sync_out_settings(&s);
}
// WRITER CALB
void AxisObject::set_sync_out_settings_calb(sync_out_settings_calb_t s, calibration_t calb) const
{
	devinterface->set_sync_out_settings_calb(&s, &calb);
}
// READER
sync_out_settings_t AxisObject::get_sync_out_settings() const
{
	sync_out_settings_t s;
	devinterface->get_sync_out_settings(&s);
	return s;
}
// READER CALB
sync_out_settings_calb_t AxisObject::get_sync_out_settings_calb(calibration_t calb) const
{
	sync_out_settings_calb_t s;
	devinterface->get_sync_out_settings_calb(&s, &calb);
	return s;
}
// WRITER
void AxisObject::set_extio_settings(extio_settings_t s) const
{
	devinterface->set_extio_settings(&s);
}
// READER
extio_settings_t AxisObject::get_extio_settings() const
{
	extio_settings_t s;
	devinterface->get_extio_settings(&s);
	return s;
}
// WRITER
void AxisObject::set_brake_settings(brake_settings_t s) const
{
	devinterface->set_brake_settings(&s);
}
// READER
brake_settings_t AxisObject::get_brake_settings() const
{
	brake_settings_t s;
	devinterface->get_brake_settings(&s);
	return s;
}
// WRITER
void AxisObject::set_control_settings(control_settings_t s) const
{
	devinterface->set_control_settings(&s);
}
// WRITER CALB
void AxisObject::set_control_settings_calb(control_settings_calb_t s, calibration_t calb) const
{
	devinterface->set_control_settings_calb(&s, &calb);
}
// READER
control_settings_t AxisObject::get_control_settings() const
{
	control_settings_t s;
	devinterface->get_control_settings(&s);
	return s;
}
// READER CALB
control_settings_calb_t AxisObject::get_control_settings_calb(calibration_t calb) const
{
	control_settings_calb_t s;
	devinterface->get_control_settings_calb(&s, &calb);
	return s;
}
// WRITER
void AxisObject::set_joystick_settings(joystick_settings_t s) const
{
	devinterface->set_joystick_settings(&s);
}
// READER
joystick_settings_t AxisObject::get_joystick_settings() const
{
	joystick_settings_t s;
	devinterface->get_joystick_settings(&s);
	return s;
}
// WRITER
void AxisObject::set_ctp_settings(ctp_settings_t s) const
{
	devinterface->set_ctp_settings(&s);
}
// READER
ctp_settings_t AxisObject::get_ctp_settings() const
{
	ctp_settings_t s;
	devinterface->get_ctp_settings(&s);
	return s;
}
// WRITER
void AxisObject::set_uart_settings(uart_settings_t s) const
{
	devinterface->set_uart_settings(&s);
}
// READER
uart_settings_t AxisObject::get_uart_settings() const
{
	uart_settings_t s;
	devinterface->get_uart_settings(&s);
	return s;
}
// WRITER
void AxisObject::set_controller_name(controller_name_t s) const
{
	devinterface->set_controller_name(&s);
}
// READER
controller_name_t AxisObject::get_controller_name() const
{
	controller_name_t s;
	devinterface->get_controller_name(&s);
	return s;
}
// READER
get_position_t AxisObject::get_position() const
{
	get_position_t s;
	devinterface->get_position(&s);
	return s;
}
// READER CALB
get_position_calb_t AxisObject::get_position_calb(calibration_t calb) const
{
	get_position_calb_t s;
	devinterface->get_position_calb(&s, &calb);
	return s;
}
// WRITER
void AxisObject::set_position(set_position_t s) const
{
	devinterface->set_position(&s);
}
// WRITER CALB
void AxisObject::set_position_calb(set_position_calb_t s, calibration_t calb) const
{
	devinterface->set_position_calb(&s, &calb);
}
// READER
chart_data_t AxisObject::get_chart_data() const
{
	chart_data_t s;
	devinterface->get_chart_data(&s);
	return s;
}
// READER
analog_data_t AxisObject::get_analog_data() const
{
	analog_data_t s;
	devinterface->get_analog_data(&s);
	return s;
}
// READER
debug_read_t AxisObject::get_debug_read() const
{
	debug_read_t s;
	devinterface->get_debug_read(&s);
	return s;
}
// WRITER
void AxisObject::set_stage_name(stage_name_t s) const
{
	devinterface->set_stage_name(&s);
}
// READER
stage_name_t AxisObject::get_stage_name() const
{
	stage_name_t s;
	devinterface->get_stage_name(&s);
	return s;
}
// WRITER
void AxisObject::set_stage_information(stage_information_t s) const
{
	devinterface->set_stage_information(&s);
}
// READER
stage_information_t AxisObject::get_stage_information() const
{
	stage_information_t s;
	devinterface->get_stage_information(&s);
	return s;
}
// WRITER
void AxisObject::set_stage_settings(stage_settings_t s) const
{
	devinterface->set_stage_settings(&s);
}
// READER
stage_settings_t AxisObject::get_stage_settings() const
{
	stage_settings_t s;
	devinterface->get_stage_settings(&s);
	return s;
}
// WRITER
void AxisObject::set_motor_information(motor_information_t s) const
{
	devinterface->set_motor_information(&s);
}
// READER
motor_information_t AxisObject::get_motor_information() const
{
	motor_information_t s;
	devinterface->get_motor_information(&s);
	return s;
}
// WRITER
void AxisObject::set_motor_settings(motor_settings_t s) const
{
	devinterface->set_motor_settings(&s);
}
// READER
motor_settings_t AxisObject::get_motor_settings() const
{
	motor_settings_t s;
	devinterface->get_motor_settings(&s);
	return s;
}
// WRITER
void AxisObject::set_encoder_information(encoder_information_t s) const
{
	devinterface->set_encoder_information(&s);
}
// READER
encoder_information_t AxisObject::get_encoder_information() const
{
	encoder_information_t s;
	devinterface->get_encoder_information(&s);
	return s;
}
// WRITER
void AxisObject::set_encoder_settings(encoder_settings_t s) const
{
	devinterface->set_encoder_settings(&s);
}
// READER
encoder_settings_t AxisObject::get_encoder_settings() const
{
	encoder_settings_t s;
	devinterface->get_encoder_settings(&s);
	return s;
}
// WRITER
void AxisObject::set_hallsensor_information(hallsensor_information_t s) const
{
	devinterface->set_hallsensor_information(&s);
}
// READER
hallsensor_information_t AxisObject::get_hallsensor_information() const
{
	hallsensor_information_t s;
	devinterface->get_hallsensor_information(&s);
	return s;
}
// WRITER
void AxisObject::set_hallsensor_settings(hallsensor_settings_t s) const
{
	devinterface->set_hallsensor_settings(&s);
}
// READER
hallsensor_settings_t AxisObject::get_hallsensor_settings() const
{
	hallsensor_settings_t s;
	devinterface->get_hallsensor_settings(&s);
	return s;
}
// WRITER
void AxisObject::set_gear_information(gear_information_t s) const
{
	devinterface->set_gear_information(&s);
}
// READER
gear_information_t AxisObject::get_gear_information() const
{
	gear_information_t s;
	devinterface->get_gear_information(&s);
	return s;
}
// WRITER
void AxisObject::set_gear_settings(gear_settings_t s) const
{
	devinterface->set_gear_settings(&s);
}
// READER
gear_settings_t AxisObject::get_gear_settings() const
{
	gear_settings_t s;
	devinterface->get_gear_settings(&s);
	return s;
}
// WRITER
void AxisObject::set_accessories_settings(accessories_settings_t s) const
{
	devinterface->set_accessories_settings(&s);
}
// READER
accessories_settings_t AxisObject::get_accessories_settings() const
{
	accessories_settings_t s;
	devinterface->get_accessories_settings(&s);
	return s;
}

