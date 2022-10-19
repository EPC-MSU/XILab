Q_DECLARE_METATYPE(calibration_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const calibration_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("A", s.A);
	obj.setProperty("MicrostepMode", s.MicrostepMode);
	return obj;
}
Q_DECLARE_METATYPE(feedback_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const feedback_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("IPS", s.IPS);
	obj.setProperty("FeedbackType", s.FeedbackType);
	obj.setProperty("FeedbackFlags", s.FeedbackFlags);
	obj.setProperty("CountsPerTurn", s.CountsPerTurn);
	return obj;
}
Q_DECLARE_METATYPE(home_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const home_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("FastHome", s.FastHome);
	obj.setProperty("uFastHome", s.uFastHome);
	obj.setProperty("SlowHome", s.SlowHome);
	obj.setProperty("uSlowHome", s.uSlowHome);
	obj.setProperty("HomeDelta", s.HomeDelta);
	obj.setProperty("uHomeDelta", s.uHomeDelta);
	obj.setProperty("HomeFlags", s.HomeFlags);
	return obj;
}
Q_DECLARE_METATYPE(move_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const move_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("Speed", s.Speed);
	obj.setProperty("uSpeed", s.uSpeed);
	obj.setProperty("Accel", s.Accel);
	obj.setProperty("Decel", s.Decel);
	obj.setProperty("AntiplaySpeed", s.AntiplaySpeed);
	obj.setProperty("uAntiplaySpeed", s.uAntiplaySpeed);
	return obj;
}
Q_DECLARE_METATYPE(engine_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const engine_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("NomVoltage", s.NomVoltage);
	obj.setProperty("NomCurrent", s.NomCurrent);
	obj.setProperty("NomSpeed", s.NomSpeed);
	obj.setProperty("uNomSpeed", s.uNomSpeed);
	obj.setProperty("EngineFlags", s.EngineFlags);
	obj.setProperty("Antiplay", s.Antiplay);
	obj.setProperty("MicrostepMode", s.MicrostepMode);
	obj.setProperty("StepsPerRev", s.StepsPerRev);
	return obj;
}
Q_DECLARE_METATYPE(entype_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const entype_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("EngineType", s.EngineType);
	obj.setProperty("DriverType", s.DriverType);
	return obj;
}
Q_DECLARE_METATYPE(power_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const power_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("HoldCurrent", s.HoldCurrent);
	obj.setProperty("CurrReductDelay", s.CurrReductDelay);
	obj.setProperty("PowerOffDelay", s.PowerOffDelay);
	obj.setProperty("CurrentSetTime", s.CurrentSetTime);
	obj.setProperty("PowerFlags", s.PowerFlags);
	return obj;
}
Q_DECLARE_METATYPE(secure_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const secure_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("LowUpwrOff", s.LowUpwrOff);
	obj.setProperty("CriticalIpwr", s.CriticalIpwr);
	obj.setProperty("CriticalUpwr", s.CriticalUpwr);
	obj.setProperty("CriticalT", s.CriticalT);
	obj.setProperty("CriticalIusb", s.CriticalIusb);
	obj.setProperty("CriticalUusb", s.CriticalUusb);
	obj.setProperty("MinimumUusb", s.MinimumUusb);
	obj.setProperty("Flags", s.Flags);
	return obj;
}
Q_DECLARE_METATYPE(edges_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const edges_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("BorderFlags", s.BorderFlags);
	obj.setProperty("EnderFlags", s.EnderFlags);
	obj.setProperty("LeftBorder", s.LeftBorder);
	obj.setProperty("uLeftBorder", s.uLeftBorder);
	obj.setProperty("RightBorder", s.RightBorder);
	obj.setProperty("uRightBorder", s.uRightBorder);
	return obj;
}
Q_DECLARE_METATYPE(pid_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const pid_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("KpU", s.KpU);
	obj.setProperty("KiU", s.KiU);
	obj.setProperty("KdU", s.KdU);
	obj.setProperty("Kpf", s.Kpf);
	obj.setProperty("Kif", s.Kif);
	obj.setProperty("Kdf", s.Kdf);
	return obj;
}
Q_DECLARE_METATYPE(sync_in_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const sync_in_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("SyncInFlags", s.SyncInFlags);
	obj.setProperty("ClutterTime", s.ClutterTime);
	obj.setProperty("Position", s.Position);
	obj.setProperty("uPosition", s.uPosition);
	obj.setProperty("Speed", s.Speed);
	obj.setProperty("uSpeed", s.uSpeed);
	return obj;
}
Q_DECLARE_METATYPE(sync_out_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const sync_out_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("SyncOutFlags", s.SyncOutFlags);
	obj.setProperty("SyncOutPulseSteps", s.SyncOutPulseSteps);
	obj.setProperty("SyncOutPeriod", s.SyncOutPeriod);
	obj.setProperty("Accuracy", s.Accuracy);
	obj.setProperty("uAccuracy", s.uAccuracy);
	return obj;
}
Q_DECLARE_METATYPE(extio_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const extio_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("EXTIOSetupFlags", s.EXTIOSetupFlags);
	obj.setProperty("EXTIOModeFlags", s.EXTIOModeFlags);
	return obj;
}
Q_DECLARE_METATYPE(brake_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const brake_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("t1", s.t1);
	obj.setProperty("t2", s.t2);
	obj.setProperty("t3", s.t3);
	obj.setProperty("t4", s.t4);
	obj.setProperty("BrakeFlags", s.BrakeFlags);
	return obj;
}
Q_DECLARE_METATYPE(control_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const control_settings_t &s) {
	QScriptValue obj = engine->newObject();
	QScriptValue MaxSpeed = engine->newObject();
	for (int i=0; i<10; i++) { MaxSpeed.setProperty(i, s.MaxSpeed[i]); }
	obj.setProperty("MaxSpeed", MaxSpeed);
	QScriptValue uMaxSpeed = engine->newObject();
	for (int i=0; i<10; i++) { uMaxSpeed.setProperty(i, s.uMaxSpeed[i]); }
	obj.setProperty("uMaxSpeed", uMaxSpeed);
	QScriptValue Timeout = engine->newObject();
	for (int i=0; i<9; i++) { Timeout.setProperty(i, s.Timeout[i]); }
	obj.setProperty("Timeout", Timeout);
	obj.setProperty("MaxClickTime", s.MaxClickTime);
	obj.setProperty("Flags", s.Flags);
	obj.setProperty("DeltaPosition", s.DeltaPosition);
	obj.setProperty("uDeltaPosition", s.uDeltaPosition);
	return obj;
}
Q_DECLARE_METATYPE(joystick_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const joystick_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("JoyLowEnd", s.JoyLowEnd);
	obj.setProperty("JoyCenter", s.JoyCenter);
	obj.setProperty("JoyHighEnd", s.JoyHighEnd);
	obj.setProperty("ExpFactor", s.ExpFactor);
	obj.setProperty("DeadZone", s.DeadZone);
	obj.setProperty("JoyFlags", s.JoyFlags);
	return obj;
}
Q_DECLARE_METATYPE(ctp_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const ctp_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("CTPMinError", s.CTPMinError);
	obj.setProperty("CTPFlags", s.CTPFlags);
	return obj;
}
Q_DECLARE_METATYPE(uart_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const uart_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("Speed", s.Speed);
	obj.setProperty("UARTSetupFlags", s.UARTSetupFlags);
	return obj;
}
Q_DECLARE_METATYPE(controller_name_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const controller_name_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("ControllerName", s.ControllerName);
	obj.setProperty("CtrlFlags", s.CtrlFlags);
	return obj;
}
Q_DECLARE_METATYPE(get_position_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const get_position_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("Position", s.Position);
	obj.setProperty("uPosition", s.uPosition);
	obj.setProperty("EncPosition", (double)s.EncPosition);
	return obj;
}
Q_DECLARE_METATYPE(set_position_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const set_position_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("Position", s.Position);
	obj.setProperty("uPosition", s.uPosition);
	obj.setProperty("EncPosition", (double)s.EncPosition);
	obj.setProperty("PosFlags", s.PosFlags);
	return obj;
}
Q_DECLARE_METATYPE(chart_data_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const chart_data_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("WindingVoltageA", s.WindingVoltageA);
	obj.setProperty("WindingVoltageB", s.WindingVoltageB);
	obj.setProperty("WindingVoltageC", s.WindingVoltageC);
	obj.setProperty("WindingCurrentA", s.WindingCurrentA);
	obj.setProperty("WindingCurrentB", s.WindingCurrentB);
	obj.setProperty("WindingCurrentC", s.WindingCurrentC);
	obj.setProperty("Pot", s.Pot);
	obj.setProperty("Joy", s.Joy);
	obj.setProperty("DutyCycle", s.DutyCycle);
	return obj;
}

Q_DECLARE_METATYPE(analog_data_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const analog_data_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("A1Voltage_ADC", s.A1Voltage_ADC);
	obj.setProperty("A2Voltage_ADC", s.A2Voltage_ADC);
	obj.setProperty("B1Voltage_ADC", s.B1Voltage_ADC);
	obj.setProperty("B2Voltage_ADC", s.B2Voltage_ADC);
	obj.setProperty("SupVoltage_ADC", s.SupVoltage_ADC);
	obj.setProperty("ACurrent_ADC", s.ACurrent_ADC);
	obj.setProperty("BCurrent_ADC", s.BCurrent_ADC);
	obj.setProperty("FullCurrent_ADC", s.FullCurrent_ADC);
	obj.setProperty("Temp_ADC", s.Temp_ADC);
	obj.setProperty("Joy_ADC", s.Joy_ADC);
	obj.setProperty("Pot_ADC", s.Pot_ADC);
	obj.setProperty("L5_ADC", s.L5_ADC);
	obj.setProperty("H5_ADC", s.H5_ADC);
	obj.setProperty("A1Voltage", s.A1Voltage);
	obj.setProperty("A2Voltage", s.A2Voltage);
	obj.setProperty("B1Voltage", s.B1Voltage);
	obj.setProperty("B2Voltage", s.B2Voltage);
	obj.setProperty("SupVoltage", s.SupVoltage);
	obj.setProperty("ACurrent", s.ACurrent);
	obj.setProperty("BCurrent", s.BCurrent);
	obj.setProperty("FullCurrent", s.FullCurrent);
	obj.setProperty("Temp", s.Temp);
	obj.setProperty("Joy", s.Joy);
	obj.setProperty("Pot", s.Pot);
	obj.setProperty("L5", s.L5);
	obj.setProperty("H5", s.H5);
	obj.setProperty("deprecated", s.deprecated);
	obj.setProperty("R", s.R);
	obj.setProperty("L", s.L);
	return obj;
}
Q_DECLARE_METATYPE(debug_read_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const debug_read_t &s) {
	QScriptValue obj = engine->newObject();
	QScriptValue DebugData = engine->newObject();
	for (int i=0; i<128; i++) { DebugData.setProperty(i, s.DebugData[i]); }
	obj.setProperty("DebugData", DebugData);
	return obj;
}

Q_DECLARE_METATYPE(stage_name_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const stage_name_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("PositionerName", s.PositionerName);
	return obj;
}
Q_DECLARE_METATYPE(stage_information_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const stage_information_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("Manufacturer", s.Manufacturer);
	obj.setProperty("PartNumber", s.PartNumber);
	return obj;
}
Q_DECLARE_METATYPE(stage_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const stage_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("LeadScrewPitch", s.LeadScrewPitch);
	obj.setProperty("Units", s.Units);
	obj.setProperty("MaxSpeed", s.MaxSpeed);
	obj.setProperty("TravelRange", s.TravelRange);
	obj.setProperty("SupplyVoltageMin", s.SupplyVoltageMin);
	obj.setProperty("SupplyVoltageMax", s.SupplyVoltageMax);
	obj.setProperty("MaxCurrentConsumption", s.MaxCurrentConsumption);
	obj.setProperty("HorizontalLoadCapacity", s.HorizontalLoadCapacity);
	obj.setProperty("VerticalLoadCapacity", s.VerticalLoadCapacity);
	return obj;
}
Q_DECLARE_METATYPE(motor_information_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const motor_information_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("Manufacturer", s.Manufacturer);
	obj.setProperty("PartNumber", s.PartNumber);
	return obj;
}
Q_DECLARE_METATYPE(motor_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const motor_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("MotorType", s.MotorType);
	obj.setProperty("ReservedField", s.ReservedField);
	obj.setProperty("Poles", s.Poles);
	obj.setProperty("Phases", s.Phases);
	obj.setProperty("NominalVoltage", s.NominalVoltage);
	obj.setProperty("NominalCurrent", s.NominalCurrent);
	obj.setProperty("NominalSpeed", s.NominalSpeed);
	obj.setProperty("NominalTorque", s.NominalTorque);
	obj.setProperty("NominalPower", s.NominalPower);
	obj.setProperty("WindingResistance", s.WindingResistance);
	obj.setProperty("WindingInductance", s.WindingInductance);
	obj.setProperty("RotorInertia", s.RotorInertia);
	obj.setProperty("StallTorque", s.StallTorque);
	obj.setProperty("DetentTorque", s.DetentTorque);
	obj.setProperty("TorqueConstant", s.TorqueConstant);
	obj.setProperty("SpeedConstant", s.SpeedConstant);
	obj.setProperty("SpeedTorqueGradient", s.SpeedTorqueGradient);
	obj.setProperty("MechanicalTimeConstant", s.MechanicalTimeConstant);
	obj.setProperty("MaxSpeed", s.MaxSpeed);
	obj.setProperty("MaxCurrent", s.MaxCurrent);
	obj.setProperty("MaxCurrentTime", s.MaxCurrentTime);
	obj.setProperty("NoLoadCurrent", s.NoLoadCurrent);
	obj.setProperty("NoLoadSpeed", s.NoLoadSpeed);
	return obj;
}
Q_DECLARE_METATYPE(encoder_information_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const encoder_information_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("Manufacturer", s.Manufacturer);
	obj.setProperty("PartNumber", s.PartNumber);
	return obj;
}
Q_DECLARE_METATYPE(encoder_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const encoder_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("MaxOperatingFrequency", s.MaxOperatingFrequency);
	obj.setProperty("SupplyVoltageMin", s.SupplyVoltageMin);
	obj.setProperty("SupplyVoltageMax", s.SupplyVoltageMax);
	obj.setProperty("MaxCurrentConsumption", s.MaxCurrentConsumption);
	obj.setProperty("PPR", s.PPR);
	obj.setProperty("EncoderSettings", s.EncoderSettings);
	return obj;
}
Q_DECLARE_METATYPE(hallsensor_information_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const hallsensor_information_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("Manufacturer", s.Manufacturer);
	obj.setProperty("PartNumber", s.PartNumber);
	return obj;
}
Q_DECLARE_METATYPE(hallsensor_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const hallsensor_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("MaxOperatingFrequency", s.MaxOperatingFrequency);
	obj.setProperty("SupplyVoltageMin", s.SupplyVoltageMin);
	obj.setProperty("SupplyVoltageMax", s.SupplyVoltageMax);
	obj.setProperty("MaxCurrentConsumption", s.MaxCurrentConsumption);
	obj.setProperty("PPR", s.PPR);
	return obj;
}
Q_DECLARE_METATYPE(gear_information_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const gear_information_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("Manufacturer", s.Manufacturer);
	obj.setProperty("PartNumber", s.PartNumber);
	return obj;
}
Q_DECLARE_METATYPE(gear_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const gear_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("ReductionIn", s.ReductionIn);
	obj.setProperty("ReductionOut", s.ReductionOut);
	obj.setProperty("RatedInputTorque", s.RatedInputTorque);
	obj.setProperty("RatedInputSpeed", s.RatedInputSpeed);
	obj.setProperty("MaxOutputBacklash", s.MaxOutputBacklash);
	obj.setProperty("InputInertia", s.InputInertia);
	obj.setProperty("Efficiency", s.Efficiency);
	return obj;
}
Q_DECLARE_METATYPE(accessories_settings_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const accessories_settings_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("MagneticBrakeInfo", s.MagneticBrakeInfo);
	obj.setProperty("MBRatedVoltage", s.MBRatedVoltage);
	obj.setProperty("MBRatedCurrent", s.MBRatedCurrent);
	obj.setProperty("MBTorque", s.MBTorque);
	obj.setProperty("MBSettings", s.MBSettings);
	obj.setProperty("TemperatureSensorInfo", s.TemperatureSensorInfo);
	obj.setProperty("TSMin", s.TSMin);
	obj.setProperty("TSMax", s.TSMax);
	obj.setProperty("TSGrad", s.TSGrad);
	obj.setProperty("TSSettings", s.TSSettings);
	obj.setProperty("LimitSwitchesSettings", s.LimitSwitchesSettings);
	return obj;
}

Q_DECLARE_METATYPE(device_information_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const device_information_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("Major", s.Major);
	obj.setProperty("Minor", s.Minor);
	obj.setProperty("Release", s.Release);
	obj.setProperty("Manufacturer", s.Manufacturer);
	obj.setProperty("ManufacturerId", s.ManufacturerId);
	obj.setProperty("ProductDescription", s.ProductDescription);
	return obj;
	
}