#include <stagesettings.h>
#include <functions.h>

#define GET(WHAT)\
	{\
		result |= devinterface->get_##WHAT(&WHAT); \
	}

#define SET(WHAT)\
	if(need_##WHAT##_save)\
	{\
		devinterface->set_##WHAT(&WHAT);\
	}

#define ZERO(WHAT)\
	memset(&WHAT, '\0', sizeof WHAT);

StageSettings::StageSettings(DeviceInterface *_devinterface)
{
	devinterface = _devinterface;

	need_stage_name_save = true;
	need_stage_information_save = true;
	need_stage_settings_save = true;
	need_motor_information_save = true;
	need_motor_settings_save = true;
	need_encoder_information_save = true;
	need_encoder_settings_save = true;
	need_hallsensor_information_save = true;
	need_hallsensor_settings_save = true;
	need_gear_information_save = true;
	need_gear_settings_save = true;
	need_accessories_settings_save = true;

	need_load = false;

	motorTypeList
		<< pair("UNKNOWN", MOTOR_TYPE_UNKNOWN)
		<< pair("STEP", MOTOR_TYPE_STEP)
		<< pair("DC", MOTOR_TYPE_DC)
		<< pair("BLDC", MOTOR_TYPE_BLDC)
		;
	tsTypeList
		<< pair("UNKNOWN", TS_TYPE_UNKNOWN)
		<< pair("THERMOCOUPLE", TS_TYPE_THERMOCOUPLE)
		<< pair("SEMICONDUCTOR", TS_TYPE_SEMICONDUCTOR)
		;

	ZERO(stage_name);
	ZERO(stage_information);
	ZERO(stage_settings);
	ZERO(motor_information);
	ZERO(motor_settings);
	ZERO(encoder_information);
	ZERO(encoder_settings);
	ZERO(hallsensor_information);
	ZERO(hallsensor_settings);
	ZERO(gear_information);
	ZERO(gear_settings);
	ZERO(accessories_settings);
}

StageSettings::~StageSettings()
{
}

void StageSettings::FromDeviceToClass()
{
	if (devinterface->getMode() != FIRMWARE_MODE)
		return;

	result_t result = result_ok;

	GET(stage_name);
	GET(stage_information);
	GET(stage_settings);
	GET(motor_information);
	GET(motor_settings);
	GET(encoder_information);
	GET(encoder_settings);
	GET(hallsensor_information);
	GET(hallsensor_settings);
	GET(gear_information);
	GET(gear_settings);
	GET(accessories_settings);
}

bool StageSettings::FromClassToDevice(void)
{
	SET(stage_name);
	SET(stage_information);
	SET(stage_settings);
	SET(motor_information);
	SET(motor_settings);
	SET(encoder_information);
	SET(encoder_settings);
	SET(hallsensor_information);
	SET(hallsensor_settings);
	SET(gear_information);
	SET(gear_settings);
	SET(accessories_settings);

	return true;
}

void StageSettings::FromSettingsToClass(QSettings *settings, QString *errors)
{
// -----------------------------------------
	// Stage name, information and settings [GNMF,GSTI,GSTS]
	settings->beginGroup("Stage");
	LoadHelperStr(settings, errors, "Positioner_name", stage_name.PositionerName);

	LoadHelperStr(settings, errors, "Manufacturer", stage_information.Manufacturer);
	LoadHelperStr(settings, errors, "Part_number", stage_information.PartNumber);

	LoadHelperFlt(settings, errors, "Lead_screw_pitch", &stage_settings.LeadScrewPitch);
	LoadHelperStr(settings, errors, "Units", stage_settings.Units);
	LoadHelperFlt(settings, errors, "Max_speed", &stage_settings.MaxSpeed);
	LoadHelperFlt(settings, errors, "Travel_range", &stage_settings.TravelRange);
	LoadHelperFlt(settings, errors, "Supply_voltage_min", &stage_settings.SupplyVoltageMin);
	LoadHelperFlt(settings, errors, "Supply_voltage_max", &stage_settings.SupplyVoltageMax);
	LoadHelperFlt(settings, errors, "Max_current_consumption", &stage_settings.MaxCurrentConsumption);
	LoadHelperFlt(settings, errors, "Horizontal_load_capacity", &stage_settings.HorizontalLoadCapacity);
	LoadHelperFlt(settings, errors, "Vertical_load_capacity", &stage_settings.VerticalLoadCapacity);
	settings->endGroup();
// -----------------------------------------
	// Stage unified motor information and settings [GMTI,GMTS]
	settings->beginGroup("StageMotor");
	LoadHelperStr(settings, errors, "Manufacturer", motor_information.Manufacturer);
	LoadHelperStr(settings, errors, "Part_number", motor_information.PartNumber);

	LoadHelperList(settings, errors, "Type", &motor_settings.MotorType, motorTypeList);
	LoadHelperUInt(settings, errors, "Poles", &motor_settings.Poles);
	LoadHelperUInt(settings, errors, "Phases", &motor_settings.Phases);
	LoadHelperFlt(settings, errors, "Nominal_voltage", &motor_settings.NominalVoltage);
	LoadHelperFlt(settings, errors, "Nominal_current", &motor_settings.NominalCurrent);
	LoadHelperFlt(settings, errors, "Nominal_speed", &motor_settings.NominalSpeed);
	LoadHelperFlt(settings, errors, "Nominal_torque", &motor_settings.NominalTorque);
	LoadHelperFlt(settings, errors, "Nominal_power", &motor_settings.NominalPower);
	LoadHelperFlt(settings, errors, "Winding_resistance", &motor_settings.WindingResistance);
	LoadHelperFlt(settings, errors, "Winding_inductance", &motor_settings.WindingInductance);
	LoadHelperFlt(settings, errors, "Rotor_inertia", &motor_settings.RotorInertia);
	LoadHelperFlt(settings, errors, "Stall_torque", &motor_settings.StallTorque);
	LoadHelperFlt(settings, errors, "Detent_torque", &motor_settings.DetentTorque);
	LoadHelperFlt(settings, errors, "Torque_constant", &motor_settings.TorqueConstant);
	LoadHelperFlt(settings, errors, "Speed_constant", &motor_settings.SpeedConstant);
	LoadHelperFlt(settings, errors, "Speed_torque_gradient", &motor_settings.SpeedTorqueGradient);
	LoadHelperFlt(settings, errors, "Mechanical_time_constant", &motor_settings.MechanicalTimeConstant);
	LoadHelperFlt(settings, errors, "Max_speed", &motor_settings.MaxSpeed);
	LoadHelperFlt(settings, errors, "Max_current", &motor_settings.MaxCurrent);
	LoadHelperFlt(settings, errors, "Max_current_time", &motor_settings.MaxCurrentTime);
	LoadHelperFlt(settings, errors, "No_load_current", &motor_settings.NoLoadCurrent);
	LoadHelperFlt(settings, errors, "No_load_speed", &motor_settings.NoLoadSpeed);
	settings->endGroup();
// -----------------------------------------
	// Stage encoder information and settings [GENI,GENS]
	settings->beginGroup("StageEncoder");
	LoadHelperStr(settings, errors, "Manufacturer", encoder_information.Manufacturer);
	LoadHelperStr(settings, errors, "Part_number", encoder_information.PartNumber);

	LoadHelperFlt(settings, errors, "Max_operating_frequency", &encoder_settings.MaxOperatingFrequency);
	LoadHelperFlt(settings, errors, "Max_current_consumption", &encoder_settings.MaxCurrentConsumption);
	LoadHelperFlt(settings, errors, "Supply_voltage_min", &encoder_settings.SupplyVoltageMin);
	LoadHelperFlt(settings, errors, "Supply_voltage_max", &encoder_settings.SupplyVoltageMax);
	LoadHelperUInt(settings, errors, "Pulses_per_revolution", &encoder_settings.PPR);
	LoadHelperFlag(settings, errors, "Differential_output", &encoder_settings.EncoderSettings, ENCSET_DIFFERENTIAL_OUTPUT);
	LoadHelperFlag(settings, errors, "Pushpull_output", &encoder_settings.EncoderSettings, ENCSET_PUSHPULL_OUTPUT);
	LoadHelperFlag(settings, errors, "Index_channel_present", &encoder_settings.EncoderSettings, ENCSET_INDEXCHANNEL_PRESENT);
	LoadHelperFlag(settings, errors, "Revolution_sensor_present", &encoder_settings.EncoderSettings, ENCSET_REVOLUTIONSENSOR_PRESENT);
	LoadHelperFlag(settings, errors, "Revolution_sensor_active_high", &encoder_settings.EncoderSettings, ENCSET_REVOLUTIONSENSOR_ACTIVE_HIGH);
	settings->endGroup();
// -----------------------------------------
	// Stage hall sensor information and settings [GHSI,GHSS]
	settings->beginGroup("StageHallsensor");
	LoadHelperStr(settings, errors, "Manufacturer", hallsensor_information.Manufacturer);
	LoadHelperStr(settings, errors, "Part_number", hallsensor_information.PartNumber);

	LoadHelperFlt(settings, errors, "Max_operating_frequency", &hallsensor_settings.MaxOperatingFrequency);
	LoadHelperFlt(settings, errors, "Max_current_consumption", &hallsensor_settings.MaxCurrentConsumption);
	LoadHelperFlt(settings, errors, "Supply_voltage_min", &hallsensor_settings.SupplyVoltageMin);
	LoadHelperFlt(settings, errors, "Supply_voltage_max", &hallsensor_settings.SupplyVoltageMax);
	LoadHelperUInt(settings, errors, "Pulses_per_revolution", &hallsensor_settings.PPR);
	settings->endGroup();
// -----------------------------------------
	// Stage gear information and settings [GGRI,GGRS]
	settings->beginGroup("StageGear");
	LoadHelperStr(settings, errors, "Manufacturer", gear_information.Manufacturer);
	LoadHelperStr(settings, errors, "Part_number", gear_information.PartNumber);

	LoadHelperFlt(settings, errors, "Reduction_in", &gear_settings.ReductionIn);
	LoadHelperFlt(settings, errors, "Reduction_out", &gear_settings.ReductionOut);
	LoadHelperFlt(settings, errors, "Rated_input_speed", &gear_settings.RatedInputSpeed);
	LoadHelperFlt(settings, errors, "Rated_input_torque", &gear_settings.RatedInputTorque);
	LoadHelperFlt(settings, errors, "Max_output_backlash", &gear_settings.MaxOutputBacklash);
	LoadHelperFlt(settings, errors, "Input_inertia", &gear_settings.InputInertia);
	LoadHelperFlt(settings, errors, "Efficiency", &gear_settings.Efficiency);
	settings->endGroup();
// -----------------------------------------
	// Stage accessories settings [GACC]
	settings->beginGroup("StageAccessories");
	LoadHelperStr(settings, errors, "Magnetic_brake_info", accessories_settings.MagneticBrakeInfo);
	LoadHelperFlt(settings, errors, "MB_rated_voltage", &accessories_settings.MBRatedVoltage);
	LoadHelperFlt(settings, errors, "MB_rated_current", &accessories_settings.MBRatedCurrent);
	LoadHelperFlt(settings, errors, "MB_torque", &accessories_settings.MBTorque);
	LoadHelperFlag(settings, errors, "MB_available", &accessories_settings.MBSettings, MB_AVAILABLE);
	LoadHelperFlag(settings, errors, "MB_powered_hold", &accessories_settings.MBSettings, MB_POWERED_HOLD);

	LoadHelperStr(settings, errors, "Temperature_sensor_info", accessories_settings.TemperatureSensorInfo);
	LoadHelperFlt(settings, errors, "TS_min", &accessories_settings.TSMin);
	LoadHelperFlt(settings, errors, "TS_max", &accessories_settings.TSMax);
	LoadHelperFlt(settings, errors, "TS_grad", &accessories_settings.TSGrad);
	LoadHelperList(settings, errors, "TS_type", &accessories_settings.TSSettings, tsTypeList);
	LoadHelperFlag(settings, errors, "TS_available", &accessories_settings.TSSettings, TS_AVAILABLE);
	LoadHelperFlag(settings, errors, "LS_on_sw1_available", &accessories_settings.LimitSwitchesSettings, LS_ON_SW1_AVAILABLE);
	LoadHelperFlag(settings, errors, "LS_on_sw2_available", &accessories_settings.LimitSwitchesSettings, LS_ON_SW2_AVAILABLE);
	LoadHelperFlag(settings, errors, "LS_sw1_active_low", &accessories_settings.LimitSwitchesSettings, LS_SW1_ACTIVE_LOW);
	LoadHelperFlag(settings, errors, "LS_sw2_active_low", &accessories_settings.LimitSwitchesSettings, LS_SW2_ACTIVE_LOW);
	LoadHelperFlag(settings, errors, "LS_shorted", &accessories_settings.LimitSwitchesSettings, LS_SHORTED);
	settings->endGroup();
}

bool StageSettings::FromClassToSettings(QSettings *settings)
{
	// Stage name, information and settings [GNMF,GSTI,GSTS]
	settings->beginGroup("Stage");
	settings->setValue("Positioner_name", stage_name.PositionerName);

	settings->setValue("Manufacturer", stage_information.Manufacturer);
	settings->setValue("Part_number", stage_information.PartNumber);

	settings->setValue("Lead_screw_pitch", toStr(stage_settings.LeadScrewPitch));
	settings->setValue("Units", stage_settings.Units);
	settings->setValue("Max_speed", toStr(stage_settings.MaxSpeed));
	settings->setValue("Travel_range", toStr(stage_settings.TravelRange));
	settings->setValue("Supply_voltage_min", toStr(stage_settings.SupplyVoltageMin));
	settings->setValue("Supply_voltage_max", toStr(stage_settings.SupplyVoltageMax));
	settings->setValue("Max_current_consumption", toStr(stage_settings.MaxCurrentConsumption));
	settings->setValue("Horizontal_load_capacity", toStr(stage_settings.HorizontalLoadCapacity));
	settings->setValue("Vertical_load_capacity", toStr(stage_settings.VerticalLoadCapacity));
	settings->endGroup();
// -----------------------------------------
	// Stage universal motor information and settings [GMTI,GMTS]
	settings->beginGroup("StageMotor");
	settings->setValue("Manufacturer", motor_information.Manufacturer);
	settings->setValue("Part_number", motor_information.PartNumber);

	SAVE_KEY(motorTypeList, motor_settings.MotorType, INT_MAX, "Type");
	settings->setValue("Poles", toStr(motor_settings.Poles));
	settings->setValue("Phases", toStr(motor_settings.Phases));
	settings->setValue("Nominal_voltage", toStr(motor_settings.NominalVoltage));
	settings->setValue("Nominal_current", toStr(motor_settings.NominalCurrent));
	settings->setValue("Nominal_speed", toStr(motor_settings.NominalSpeed));
	settings->setValue("Nominal_torque", toStr(motor_settings.NominalTorque));
	settings->setValue("Nominal_power", toStr(motor_settings.NominalPower));
	settings->setValue("Winding_resistance", toStr(motor_settings.WindingResistance));
	settings->setValue("Winding_inductance", toStr(motor_settings.WindingInductance));
	settings->setValue("Rotor_inertia", toStr(motor_settings.RotorInertia));
	settings->setValue("Stall_torque", toStr(motor_settings.StallTorque));
	settings->setValue("Detent_torque", toStr(motor_settings.DetentTorque));
	settings->setValue("Torque_constant", toStr(motor_settings.TorqueConstant));
	settings->setValue("Speed_constant", toStr(motor_settings.SpeedConstant));
	settings->setValue("Speed_torque_gradient", toStr(motor_settings.SpeedTorqueGradient));
	settings->setValue("Mechanical_time_constant", toStr(motor_settings.MechanicalTimeConstant));
	settings->setValue("Max_speed", toStr(motor_settings.MaxSpeed));
	settings->setValue("Max_current", toStr(motor_settings.MaxCurrent));
	settings->setValue("Max_current_time", toStr(motor_settings.MaxCurrentTime));
	settings->setValue("No_load_current", toStr(motor_settings.NoLoadCurrent));
	settings->setValue("No_load_speed", toStr(motor_settings.NoLoadSpeed));
	settings->endGroup();
// -----------------------------------------
	// Stage encoder information and settings [GENI,GENS]
	settings->beginGroup("StageEncoder");
	settings->setValue("Manufacturer", encoder_information.Manufacturer);
	settings->setValue("Part_number", encoder_information.PartNumber);

	settings->setValue("Max_operating_frequency", toStr(encoder_settings.MaxOperatingFrequency));
	settings->setValue("Max_current_consumption", toStr(encoder_settings.MaxCurrentConsumption));
	settings->setValue("Supply_voltage_min", toStr(encoder_settings.SupplyVoltageMin));
	settings->setValue("Supply_voltage_max", toStr(encoder_settings.SupplyVoltageMax));
	settings->setValue("Pulses_per_revolution", encoder_settings.PPR);
	settings->setValue("Differential_output", (encoder_settings.EncoderSettings & ENCSET_DIFFERENTIAL_OUTPUT) != 0);
	settings->setValue("Pushpull_output", (encoder_settings.EncoderSettings & ENCSET_PUSHPULL_OUTPUT) != 0);
	settings->setValue("Index_channel_present", (encoder_settings.EncoderSettings & ENCSET_INDEXCHANNEL_PRESENT) != 0);
	settings->setValue("Revolution_sensor_present", (encoder_settings.EncoderSettings & ENCSET_REVOLUTIONSENSOR_PRESENT) != 0);
	settings->setValue("Revolution_sensor_active_high", (encoder_settings.EncoderSettings & ENCSET_REVOLUTIONSENSOR_ACTIVE_HIGH) != 0);

	settings->endGroup();
// -----------------------------------------
	// Stage hallsensor information and settings [GHSI,GHSS]
	settings->beginGroup("StageHallsensor");
	settings->setValue("Manufacturer", hallsensor_information.Manufacturer);
	settings->setValue("Part_number", hallsensor_information.PartNumber);

	settings->setValue("Max_operating_frequency", toStr(hallsensor_settings.MaxOperatingFrequency));
	settings->setValue("Max_current_consumption", toStr(hallsensor_settings.MaxCurrentConsumption));
	settings->setValue("Supply_voltage_min", toStr(hallsensor_settings.SupplyVoltageMin));
	settings->setValue("Supply_voltage_max", toStr(hallsensor_settings.SupplyVoltageMax));
	settings->setValue("Pulses_per_revolution", hallsensor_settings.PPR);
	settings->endGroup();
// -----------------------------------------
	// Stage gear information and settings [GGRI,GGRS]
	settings->beginGroup("StageGear");
	settings->setValue("Manufacturer", gear_information.Manufacturer);
	settings->setValue("Part_number", gear_information.PartNumber);

	settings->setValue("Reduction_in", toStr(gear_settings.ReductionIn));
	settings->setValue("Reduction_out", toStr(gear_settings.ReductionOut));
	settings->setValue("Rated_input_speed", toStr(gear_settings.RatedInputSpeed));
	settings->setValue("Rated_input_torque", toStr(gear_settings.RatedInputTorque));
	settings->setValue("Max_output_backlash", toStr(gear_settings.MaxOutputBacklash));
	settings->setValue("Input_inertia", toStr(gear_settings.InputInertia));
	settings->setValue("Efficiency", toStr(gear_settings.Efficiency));
	settings->endGroup();
// -----------------------------------------
	// Stage accessories settings [GACC]
	settings->beginGroup("StageAccessories");
	settings->setValue("Magnetic_brake_info", accessories_settings.MagneticBrakeInfo);
	settings->setValue("MB_rated_voltage", toStr(accessories_settings.MBRatedVoltage));
	settings->setValue("MB_rated_current", toStr(accessories_settings.MBRatedCurrent));
	settings->setValue("MB_torque", toStr(accessories_settings.MBTorque));
	settings->setValue("MB_available", (accessories_settings.MBSettings & MB_AVAILABLE) != 0);
	settings->setValue("MB_powered_hold", (accessories_settings.MBSettings & MB_POWERED_HOLD) != 0);

	settings->setValue("Temperature_sensor_info", accessories_settings.TemperatureSensorInfo);
	settings->setValue("TS_min", toStr(accessories_settings.TSMin));
	settings->setValue("TS_max", toStr(accessories_settings.TSMax));
	settings->setValue("TS_grad", toStr(accessories_settings.TSGrad));
	SAVE_KEY(tsTypeList, accessories_settings.TSSettings, TS_TYPE_BITS, "TS_type");
	settings->setValue("TS_available", (accessories_settings.TSSettings & TS_AVAILABLE) != 0);
	settings->setValue("LS_on_sw1_available", (accessories_settings.LimitSwitchesSettings & LS_ON_SW1_AVAILABLE) != 0);
	settings->setValue("LS_on_sw2_available", (accessories_settings.LimitSwitchesSettings & LS_ON_SW2_AVAILABLE) != 0);
	settings->setValue("LS_sw1_active_low", (accessories_settings.LimitSwitchesSettings & LS_SW1_ACTIVE_LOW) != 0);
	settings->setValue("LS_sw2_active_low", (accessories_settings.LimitSwitchesSettings & LS_SW2_ACTIVE_LOW) != 0);
	settings->setValue("LS_shorted", (accessories_settings.LimitSwitchesSettings & LS_SHORTED) != 0);
	settings->endGroup();

	return true;
}