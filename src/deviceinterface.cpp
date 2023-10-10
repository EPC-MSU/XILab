#include <QDebug>
#include <QApplication>
#include <deviceinterface.h>

#define GET_(WHAT, TYPE)\
	result_t DeviceInterface::get_##WHAT##_##TYPE (WHAT##_##TYPE##_t* WHAT)\
{\
	result = libximc::get_##WHAT##_##TYPE(id, WHAT);\
	if (result != result_ok){\
		char buffer[100];\
		strcpy(buffer, "get_");\
		strcat(buffer, #WHAT);\
		strcat(buffer, "_");\
		strcat(buffer, #TYPE);\
		emit errorSignal(buffer, result);\
	}\
	return result;\
}

#define GET_CALB_(WHAT, TYPE)\
	result_t DeviceInterface::get_##WHAT##_##TYPE##_calb (WHAT##_##TYPE##_calb_t* WHAT, calibration_t* calb)\
{\
	result = libximc::get_##WHAT##_##TYPE##_calb(id, WHAT, calb);\
	if (result != result_ok){\
		char buffer[100];\
		strcpy(buffer, "get_");\
		strcat(buffer, #WHAT);\
		strcat(buffer, "_");\
		strcat(buffer, #TYPE);\
		strcat(buffer, "_calb");\
		emit errorSignal(buffer, result);\
	}\
	return result;\
}

#define SET_(WHAT, TYPE)\
	result_t DeviceInterface::set_##WHAT##_##TYPE (const WHAT##_##TYPE##_t* WHAT)\
{\
	result = libximc::set_##WHAT##_##TYPE(id, WHAT);\
	char buffer[100];\
	strcpy(buffer, "set_");\
	strcat(buffer, #WHAT);\
	strcat(buffer, "_");\
	strcat(buffer, #TYPE);\
	if (result == result_value_error)\
		emit warningSignal(buffer, result);\
	else if (result != result_ok)\
		emit errorSignal(buffer, result);\
	return result;\
}

#define SET_CALB_(WHAT, TYPE)\
	result_t DeviceInterface::set_##WHAT##_##TYPE##_calb (const WHAT##_##TYPE##_calb_t* WHAT, calibration_t* calb)\
{\
	result = libximc::set_##WHAT##_##TYPE##_calb(id, WHAT, calb);\
	char buffer[100];\
	strcpy(buffer, "set_");\
	strcat(buffer, #WHAT);\
	strcat(buffer, "_");\
	strcat(buffer, #TYPE);\
	strcat(buffer, "_calb");\
	if (result == result_value_error)\
		emit warningSignal(buffer, result);\
	else if (result != result_ok)\
		emit errorSignal(buffer, result);\
	return result;\
}

#define GETSET(WHAT, TYPE)\
	GET_(WHAT, TYPE);\
	SET_(WHAT, TYPE);

#define GETSET_CALB(WHAT, TYPE)\
	GET_CALB_(WHAT, TYPE);\
	SET_CALB_(WHAT, TYPE);


DeviceInterface::DeviceInterface()
{
	device_mode = UNKNOWN_MODE;
	cs = new Cactus();
}

DeviceInterface::~DeviceInterface()
{
	delete cs;
}

void DeviceInterface::open_device (const char* name)
{
	id = libximc::open_device(name);
	device_mode = UNKNOWN_MODE;

	// parse uri scheme to determine protocol type. #86820
	const int max_scheme_len = 10;
	char scheme[max_scheme_len] = { '\0' };
	const char *scheme_beg = name;
	const char *scheme_end = strchr(name, ':');
	if (!scheme_end)
		return;
	if (scheme_end - scheme_beg >= max_scheme_len)
		return;
	memcpy(scheme, name, scheme_end - scheme_beg);
	if (strcmp(scheme,"xi-com") == 0)
		this->protocol = dtSerial;
	else if (strcmp(scheme, "xi-emu") == 0)
		this->protocol = dtVirtual;
	else if (strcmp(scheme, "xi-net") == 0)
		this->protocol = dtNet;
	else if (strcmp(scheme, "xi-udp") == 0)
		this->protocol = dtUdp;
	else if (strcmp(scheme, "xi-tcp") == 0)
		this->protocol = dtTcp;
	else
		this->protocol = dtUnknown;
}

result_t DeviceInterface::close_device ()
{
	result = libximc::close_device(&id);
	if (result != result_ok)
		emit errorSignal("close_device", result);
	device_mode = UNKNOWN_MODE;
	id = device_undefined;
	return result;
}

result_t DeviceInterface::load_calibration_table(const char* namefile)
{
	result = libximc::set_correction_table(id, namefile);
	if (result != result_ok)
		emit errorSignal("load_correction_table", result);
	return result;
}

bool DeviceInterface::is_open ()
{
	return (id != device_undefined);
}

result_t DeviceInterface::probe_device (const char* name)
{
	result = libximc::probe_device(name);
	if (result != result_ok)
		emit errorSignal("probe_device", result);
	return result;
}

libximc::device_enumeration_t DeviceInterface::enumerate_devices(int probe_devices, const char* hints)
{
	libximc::device_enumeration_t result = libximc::enumerate_devices(probe_devices, hints);
	if (result != result_ok)
		emit errorSignal("enumerate_devices", result);
	return result;
}

result_t DeviceInterface::free_enumerate_devices (device_enumeration_t dev_enum)
{
	result = libximc::free_enumerate_devices(dev_enum);
	if (result != result_ok)
		emit errorSignal("free_enumerate_devices", result);
	return result;
}

result_t DeviceInterface::get_enumerate_device_serial (device_enumeration_t device_enumeration, int device_index, uint32_t* serial)
{
	result = libximc::get_enumerate_device_serial(device_enumeration, device_index, serial);
	if (result != result_ok)
		emit errorSignal("get_enumerate_device_serial", result);
	return result;
}

result_t DeviceInterface::get_enumerate_device_information (device_enumeration_t device_enumeration, int device_index, device_information_t* device_information)
{
	result = libximc::get_enumerate_device_information(device_enumeration, device_index, device_information);
	if (result != result_ok)
		emit errorSignal("get_enumerate_device_information", result);
	return result;
}

result_t DeviceInterface::get_enumerate_device_controller_name (device_enumeration_t device_enumeration, int device_index, controller_name_t* controller_name)
{
	result = libximc::get_enumerate_device_controller_name(device_enumeration, device_index, controller_name);
	if (result != result_ok)
		emit errorSignal("get_enumerate_device_controller_name", result);
	return result;
}

result_t DeviceInterface::get_enumerate_device_stage_name (device_enumeration_t device_enumeration, int device_index, stage_name_t* stage_name)
{
	result = libximc::get_enumerate_device_stage_name(device_enumeration, device_index, stage_name);
	if (result != result_ok)
		emit errorSignal("get_enumerate_device_stage_name", result);
	return result;
}

	// Группа команд настройки контроллера
GETSET(feedback, settings);
GETSET(home, settings);
GETSET(move, settings);
GETSET(engine, settings);
GETSET(entype, settings);
GETSET(power, settings);
GETSET(secure, settings);
GETSET(edges, settings);
GETSET(pid, settings);
GETSET(emf, settings);
GETSET(network, settings);
GETSET(password, settings);
GETSET(sync_in, settings);
GETSET(sync_out, settings);
GETSET(extio, settings);
GETSET(brake, settings);
GETSET(control, settings);
GETSET(joystick, settings);
GETSET(ctp, settings);
GETSET(uart, settings);
GETSET(controller, name);
GETSET(nonvolatile, memory);


GETSET_CALB(home, settings);
GETSET_CALB(move, settings);
GETSET_CALB(engine, settings);
GETSET_CALB(edges, settings);
GETSET_CALB(sync_in, settings);
GETSET_CALB(sync_out, settings);
GETSET_CALB(control, settings);


	// Группа команд управления движением

//Команда PWOF
result_t DeviceInterface::command_power_off ()
{
	result = libximc::command_power_off(id);
	if (result != result_ok)
		emit errorSignal("command_power_off", result);
	return result;
}

//Команда STOP
result_t DeviceInterface::command_stop ()
{
	result = libximc::command_stop(id);
	if (result != result_ok)
		emit errorSignal("command_stop", result);
	return result;
}

//Команда SSTP
result_t DeviceInterface::command_sstp ()
{
	result = libximc::command_sstp(id);
	if (result != result_ok)
		emit errorSignal("command_sstp", result);
	return result;
}

//Команда MOVE
result_t DeviceInterface::command_move (int pos, int upos)
{
	result = libximc::command_move(id, pos, upos);
	if (result != result_ok)
		emit errorSignal("command_move", result);
	return result;
}

//Команда MOVE, калиброванная версия
result_t DeviceInterface::command_move_calb (float fpos, calibration_t calb)
{
	result = libximc::command_move_calb(id, fpos, &calb);
	if (result != result_ok)
		emit errorSignal("command_move_calb", result);
	return result;
}

//Команда MOVR, калиброванная версия
result_t DeviceInterface::command_movr_calb (float fpos, calibration_t calb)
{
	result = libximc::command_movr_calb(id, fpos, &calb);
	if (result != result_ok)
		emit errorSignal("command_movr_calb", result);
	return result;
}

//Команда MOVR
result_t DeviceInterface::command_movr (int offset, int uoffset)
{
	result = libximc::command_movr(id, offset, uoffset);
	if (result != result_ok)
		emit errorSignal("command_movr", result);
	return result;
}

//Команда HOME
result_t DeviceInterface::command_home ()
{
	result = libximc::command_home(id);
	if (result != result_ok)
		emit errorSignal("command_home", result);
	return result;
}

//Команда LEFT
result_t DeviceInterface::command_left ()
{
	result = libximc::command_left(id);
	if (result != result_ok)
		emit errorSignal("command_left", result);
	return result;
}

//Команда RIGT
result_t DeviceInterface::command_right ()
{
	result = libximc::command_right(id);
	if (result != result_ok)
		emit errorSignal("command_right", result);
	return result;
}

//Команда LOFT
result_t DeviceInterface::command_loft ()
{
	result = libximc::command_loft(id);
	if (result != result_ok)
		emit errorSignal("command_loft", result);
	return result;
}

//Команда wait_for_stop
result_t DeviceInterface::command_wait_for_stop (int refresh_period)
{
	result = libximc::command_wait_for_stop (id, refresh_period);
	if (result != result_ok)
		emit errorSignal("command_wait_for_stop", result);
	return result;
}

//Команда homezero
result_t DeviceInterface::command_homezero ()
{
	result = libximc::command_homezero(id);
	if (result != result_ok)
		emit errorSignal("command_homezero", result);
	return result;
}

	// Группа команд установки текущей позиции

//Команда SPOS
result_t DeviceInterface::set_position (const set_position_t* position)
{
	result = libximc::set_position(id, position);
	if (result != result_ok)
		emit errorSignal("set_position", result);
	return result;
}

//Команда GPOS
result_t DeviceInterface::get_position (get_position_t* position)
{
	result = libximc::get_position(id, position);
	if (result != result_ok)
		emit errorSignal("get_position", result);
	return result;
}

//Команда SPOS, калиброванная версия
result_t DeviceInterface::set_position_calb (const set_position_calb_t* position, const calibration_t* calb)
{
	result = libximc::set_position_calb(id, position, calb);
	if (result != result_ok)
		emit errorSignal("set_position_calb", result);
	return result;
}

//Команда GPOS, калиброванная версия
result_t DeviceInterface::get_position_calb (get_position_calb_t* position, const calibration_t* calb)
{
	result = libximc::get_position_calb(id, position, calb);
	if (result != result_ok)
		emit errorSignal("get_position_calb", result);
	return result;
}

//Команда ZERO
result_t DeviceInterface::command_zero ()
{
	result = libximc::command_zero(id);
	if (result != result_ok)
		emit errorSignal("command_zero", result);
	return result;
}

	// Группа команд сохранения и загрузки настроек

//Команда SAVE
result_t DeviceInterface::command_save_settings ()
{
	result = libximc::command_save_settings(id);
	if (result != result_ok)
		emit errorSignal("command_save_settings", result);
	return result;
}

//Команда READ
result_t DeviceInterface::command_read_settings ()
{
	result = libximc::command_read_settings(id);
	if (result != result_ok)
		emit errorSignal("command_read_settings", result);
	return result;
}

//Команда EESV
result_t DeviceInterface::command_eesave_settings ()
{
	result = libximc::command_eesave_settings(id);
	if (result != result_ok)
		emit errorSignal("command_eesave_settings", result);
	return result;
}

//Команда EERD
result_t DeviceInterface::command_eeread_settings ()
{
	result = libximc::command_eeread_settings(id);
	if (result != result_ok)
		emit errorSignal("command_eeread_settings", result);
	return result;
}

	// Группа команд получения статуса контроллера

//Команда GETS
result_t DeviceInterface::get_status (status_t* status)
{
	result = libximc::get_status(id, status);
	if (result != result_ok)
		emit errorSignal("get_status", result);
	return result;
}

//Команда GETS_CALB
result_t DeviceInterface::get_status_calb (status_calb_t* status_calb, calibration_t* calibration)
{
	result = libximc::get_status_calb(id, status_calb, calibration);
	if (result != result_ok)
		emit errorSignal("get_status_calb", result);
	return result;
}

//Команда GETC
result_t DeviceInterface::get_chart_data (chart_data_t* chart_data)
{
	result = libximc::get_chart_data(id, chart_data);
	if (result != result_ok)
		emit errorSignal("get_chart_data", result);
	return result;
}

//Команда GETI
result_t DeviceInterface::get_device_information (device_information_t* device_information)
{
	result = libximc::get_device_information(id, device_information);
	if (result != result_ok)
		emit errorSignal("get_device_information", result);
	return result;
}

//Команда GSER
result_t DeviceInterface::get_serial_number (uint32_t* serial)
{
	result = libximc::get_serial_number(id, serial);
	if (result != result_ok)
		emit errorSignal("get_serial_number", result);
	return result;
}

	// Группа команд для работы с прошивкой контроллера

//Команда GFWV
result_t DeviceInterface::get_firmware_version (unsigned int* major, unsigned int* minor, unsigned int* release)
{
	result = libximc::get_firmware_version(id, major, minor, release);
	if (result != result_ok)
		emit errorSignal("get_firmware_version", result);
	return result;
}

//Команда "update firmware"
result_t DeviceInterface::command_update_firmware (const char* name, const uint8_t* data, uint32_t data_size)
{
	result = libximc::command_update_firmware(name, data, data_size);
	if (result != result_ok)
		emit errorSignal("command_update_firmware", result);
	return result;
}

//Команда UPDF
result_t DeviceInterface::service_command_updf ()
{
	result = libximc::service_command_updf(id);
	if (result != result_ok)
		emit errorSignal("command_updf", result);
	return result;
}
	// Группа сервисных команд

//Команда SSER
result_t DeviceInterface::set_serial_number (const serial_number_t* serial_number)
{
	result = libximc::set_serial_number(id, serial_number);
	if (result != result_ok)
		emit errorSignal("set_serial_number", result);
	return result;
}

//Команда RDAN
result_t DeviceInterface::get_analog_data (analog_data_t* ad)
{
	result = libximc::get_analog_data(id, ad);
	if (result != result_ok)
		emit errorSignal("get_analog_data", result);
	return result;
}

//Команда DBGR
result_t DeviceInterface::get_debug_read (debug_read_t* debug_read)
{
	result = libximc::get_debug_read(id, debug_read);
	if (result != result_ok)
		emit errorSignal("get_debug_read", result);
	return result;
}

//Команда DBGW
result_t DeviceInterface::set_debug_write (debug_write_t* debug_write)
{
	result = libximc::set_debug_write(id, debug_write);
	if (result != result_ok)
		emit errorSignal("set_debug_write", result);
	return result;
}

//Команда CLFR
result_t DeviceInterface::command_clear_fram ()
{
	result = libximc::command_clear_fram(id);
	if (result != result_ok)
		emit errorSignal("command_clear_fram", result);
	return result;
}

//Команда start_measurements
result_t DeviceInterface::command_start_measurements()
{
	result = libximc::command_start_measurements(id);
	if (result != result_ok)
		emit errorSignal("command_start_measurements", result);
	return result;
}

//Команда get_measurements
result_t DeviceInterface::get_measurements(measurements_t* measurements)
{
	result = libximc::get_measurements(id, measurements);
	if (result != result_ok)
		emit errorSignal("get_measurements", result);

	return result;
}

	// Группа команда работы с EEPROM подвижки
	GETSET(stage, name);

	GETSET(stage, information);
	GETSET(stage, settings);
	GETSET(motor, information);
	GETSET(motor, settings);
	GETSET(encoder, information);
	GETSET(encoder, settings);
	GETSET(hallsensor, information);
	GETSET(hallsensor, settings);
	GETSET(gear, information);
	GETSET(gear, settings);
	GETSET(accessories, settings);

	// Команды загрузчика

//Команда GBLV
result_t DeviceInterface::get_bootloader_version (unsigned int* major, unsigned int* minor, unsigned int* release)
{
	result = libximc::get_bootloader_version(id, major, minor, release);
	if (result != result_ok)
		emit errorSignal("get_bootloader_version", result);
	return result;
}

//Команда IRND
result_t DeviceInterface::get_init_random (init_random_t* init_random)
{
	result = libximc::get_init_random(id, init_random);
	if (result != result_ok)
		emit errorSignal("get_init_random", result);
	return result;
}

//Команда GOFW
result_t DeviceInterface::goto_firmware (uint8_t* ret)
{
	result = libximc::goto_firmware(id, ret);
	if (result != result_ok)
		emit errorSignal("goto_firmware", result);
	return result;
}

//Команда HASF
result_t DeviceInterface::has_firmware (const char* name, uint8_t* ret)
{
	result = libximc::has_firmware(name, ret);
	if (result != result_ok)
		emit errorSignal("has_firmware", result);
	return result;
}

//Команда WKEY
result_t DeviceInterface::write_key (const char* name, uint8_t* key)
{
	result = libximc::write_key(name, key);
	if (result != result_ok)
		emit errorSignal("write_key", result);
	return result;
}

//Команда REST
result_t DeviceInterface::command_reset ()
{
	result = libximc::command_reset(id);
	if (result != result_ok)
		emit errorSignal("command_reset", result);
	return result;
}

int DeviceInterface::getMode()
{
	if (id == device_undefined)
		return UNKNOWN_MODE;

	if (device_mode == UNKNOWN_MODE)
	{
		if (this->get_device_information(&info) == result_ok)
		{
			if (QString(info.ProductDescription).contains("BOOT", Qt::CaseInsensitive))
				device_mode = BOOTLOADER_MODE;
			else
				device_mode = FIRMWARE_MODE;
		}
	}

	return device_mode;
}

protocol_t DeviceInterface::getProtocolType()
{
	return this->protocol;
}

device_t DeviceInterface::getDeviceId()
{
	return id;
}
