#include "axisobject.h"

AxisObject::AxisObject(DeviceInterface* _devinterface)
{
	devinterface = _devinterface;
}

void AxisObject::command_reset() const
{
	devinterface->command_reset();
}

void AxisObject::command_right() const
{
	devinterface->command_right();
}

void AxisObject::command_left() const
{
	devinterface->command_left();
}

void AxisObject::command_stop() const
{
	devinterface->command_stop();
}

void AxisObject::command_sstp() const
{
	devinterface->command_sstp();
}

void AxisObject::command_move(int pos, int upos) const
{
	devinterface->command_move(pos, upos);
}

void AxisObject::command_movr(int pos, int upos) const
{
	devinterface->command_movr(pos, upos);
}

void AxisObject::command_move_calb(float fpos, calibration_t calb) const
{
	devinterface->command_move_calb(fpos, calb);
}

void AxisObject::command_movr_calb(float fpos, calibration_t calb) const
{
	devinterface->command_movr_calb(fpos, calb);
}

void AxisObject::command_zero() const
{
	devinterface->command_zero();
}

void AxisObject::command_home() const
{
	devinterface->command_home();
}

void AxisObject::command_clear_fram() const
{
	devinterface->command_clear_fram();
}

void AxisObject::command_loft() const
{
	devinterface->command_loft();
}

void AxisObject::command_power_off() const
{
	devinterface->command_power_off();
}

status_t AxisObject::get_status() const
{
	status_t status;
	devinterface->get_status(&status);
	return status;
}

status_calb_t AxisObject::get_status_calb(calibration_t calb) const
{
	status_calb_t status_calb;
	devinterface->get_status_calb(&status_calb, &calb);
	return status_calb;
}

device_information_t AxisObject::get_device_information() const
{
	device_information_t device_information;
	devinterface->get_device_information(&device_information);
	return device_information;
}

void AxisObject::command_wait_for_stop(int refresh_period) const
{
	devinterface->command_wait_for_stop(refresh_period);
}

void AxisObject::wait_for_stop(int refresh_period) const
{
	emit log_signal("\"wait_for_stop\" script function is deprecated, please use \"command_wait_for_stop\" instead");
	this->command_wait_for_stop(refresh_period);
}

void AxisObject::wait_for_stop() const
{
	this->wait_for_stop(50);
}

void AxisObject::command_homezero() const
{
	devinterface->command_homezero();
}

void AxisObject::command_save_settings() const
{
	devinterface->command_save_settings();
}

void AxisObject::command_read_settings() const
{
	devinterface->command_read_settings();
}

#include <qtscript-getsetfunc.c>
