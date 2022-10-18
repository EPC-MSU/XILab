static void fromScriptValue(const QScriptValue &obj, calibration_t &s) {
	s.A = obj.property("A").toNumber();
	s.MicrostepMode = obj.property("MicrostepMode").toUInt16();
}

static void fromScriptValue(const QScriptValue &obj, feedback_settings_t &s) {
	s.IPS = obj.property("IPS").toUInt32();
	s.FeedbackType = obj.property("FeedbackType").toUInt32();
	s.FeedbackFlags = obj.property("FeedbackFlags").toUInt32();
	s.CountsPerTurn = obj.property("CountsPerTurn").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, home_settings_t &s) {
	s.FastHome = obj.property("FastHome").toUInt32();
	s.uFastHome = obj.property("uFastHome").toUInt32();
	s.SlowHome = obj.property("SlowHome").toUInt32();
	s.uSlowHome = obj.property("uSlowHome").toUInt32();
	s.HomeDelta = obj.property("HomeDelta").toInt32();
	s.uHomeDelta = obj.property("uHomeDelta").toInt32();
	s.HomeFlags = obj.property("HomeFlags").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, move_settings_t &s) {
	s.Speed = obj.property("Speed").toUInt32();
	s.uSpeed = obj.property("uSpeed").toUInt32();
	s.Accel = obj.property("Accel").toUInt32();
	s.Decel = obj.property("Decel").toUInt32();
	s.AntiplaySpeed = obj.property("AntiplaySpeed").toUInt32();
	s.uAntiplaySpeed = obj.property("uAntiplaySpeed").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, engine_settings_t &s) {
	s.NomVoltage = obj.property("NomVoltage").toUInt32();
	s.NomCurrent = obj.property("NomCurrent").toUInt32();
	s.NomSpeed = obj.property("NomSpeed").toUInt32();
	s.uNomSpeed = obj.property("uNomSpeed").toUInt32();
	s.EngineFlags = obj.property("EngineFlags").toUInt32();
	s.Antiplay = obj.property("Antiplay").toInt32();
	s.MicrostepMode = obj.property("MicrostepMode").toUInt32();
	s.StepsPerRev = obj.property("StepsPerRev").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, entype_settings_t &s) {
	s.EngineType = obj.property("EngineType").toUInt32();
	s.DriverType = obj.property("DriverType").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, power_settings_t &s) {
	s.HoldCurrent = obj.property("HoldCurrent").toUInt32();
	s.CurrReductDelay = obj.property("CurrReductDelay").toUInt32();
	s.PowerOffDelay = obj.property("PowerOffDelay").toUInt32();
	s.CurrentSetTime = obj.property("CurrentSetTime").toUInt32();
	s.PowerFlags = obj.property("PowerFlags").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, secure_settings_t &s) {
	s.LowUpwrOff = obj.property("LowUpwrOff").toUInt32();
	s.CriticalIpwr = obj.property("CriticalIpwr").toUInt32();
	s.CriticalUpwr = obj.property("CriticalUpwr").toUInt32();
	s.CriticalT = obj.property("CriticalT").toUInt32();
	s.CriticalIusb = obj.property("CriticalIusb").toUInt32();
	s.CriticalUusb = obj.property("CriticalUusb").toUInt32();
	s.MinimumUusb = obj.property("MinimumUusb").toUInt32();
	s.Flags = obj.property("Flags").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, edges_settings_t &s) {
	s.BorderFlags = obj.property("BorderFlags").toUInt32();
	s.EnderFlags = obj.property("EnderFlags").toUInt32();
	s.LeftBorder = obj.property("LeftBorder").toInt32();
	s.uLeftBorder = obj.property("uLeftBorder").toInt32();
	s.RightBorder = obj.property("RightBorder").toInt32();
	s.uRightBorder = obj.property("uRightBorder").toInt32();
}

static void fromScriptValue(const QScriptValue &obj, pid_settings_t &s) {
	s.KpU = obj.property("KpU").toUInt32();
	s.KiU = obj.property("KiU").toUInt32();
	s.KdU = obj.property("KdU").toUInt32();
	s.Kpf = obj.property("Kpf").toNumber();
	s.Kif = obj.property("Kif").toNumber();
	s.Kdf = obj.property("Kdf").toNumber();
}

static void fromScriptValue(const QScriptValue &obj, sync_in_settings_t &s) {
	s.SyncInFlags = obj.property("SyncInFlags").toUInt32();
	s.ClutterTime = obj.property("ClutterTime").toUInt32();
	s.Position = obj.property("Position").toInt32();
	s.uPosition = obj.property("uPosition").toInt32();
	s.Speed = obj.property("Speed").toUInt32();
	s.uSpeed = obj.property("uSpeed").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, sync_out_settings_t &s) {
	s.SyncOutFlags = obj.property("SyncOutFlags").toUInt32();
	s.SyncOutPulseSteps = obj.property("SyncOutPulseSteps").toUInt32();
	s.SyncOutPeriod = obj.property("SyncOutPeriod").toUInt32();
	s.Accuracy = obj.property("Accuracy").toUInt32();
	s.uAccuracy = obj.property("uAccuracy").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, extio_settings_t &s) {
	s.EXTIOSetupFlags = obj.property("EXTIOSetupFlags").toUInt32();
	s.EXTIOModeFlags = obj.property("EXTIOModeFlags").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, brake_settings_t &s) {
	s.t1 = obj.property("t1").toUInt32();
	s.t2 = obj.property("t2").toUInt32();
	s.t3 = obj.property("t3").toUInt32();
	s.t4 = obj.property("t4").toUInt32();
	s.BrakeFlags = obj.property("BrakeFlags").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, control_settings_t &s) {
	for (int i=0; i<10; i++) { s.MaxSpeed[i] = obj.property("MaxSpeed").property(i).toUInt32(); }
	for (int i=0; i<10; i++) { s.uMaxSpeed[i] = obj.property("uMaxSpeed").property(i).toUInt32(); }
	for (int i=0; i<9; i++) { s.Timeout[i] = obj.property("Timeout").property(i).toUInt32(); }
	s.MaxClickTime = obj.property("MaxClickTime").toUInt32();
	s.Flags = obj.property("Flags").toUInt32();
	s.DeltaPosition = obj.property("DeltaPosition").toInt32();
	s.uDeltaPosition = obj.property("uDeltaPosition").toInt32();
}

static void fromScriptValue(const QScriptValue &obj, joystick_settings_t &s) {
	s.JoyLowEnd = obj.property("JoyLowEnd").toUInt32();
	s.JoyCenter = obj.property("JoyCenter").toUInt32();
	s.JoyHighEnd = obj.property("JoyHighEnd").toUInt32();
	s.ExpFactor = obj.property("ExpFactor").toUInt32();
	s.DeadZone = obj.property("DeadZone").toUInt32();
	s.JoyFlags = obj.property("JoyFlags").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, ctp_settings_t &s) {
	s.CTPMinError = obj.property("CTPMinError").toUInt32();
	s.CTPFlags = obj.property("CTPFlags").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, uart_settings_t &s) {
	s.Speed = obj.property("Speed").toUInt32();
	s.UARTSetupFlags = obj.property("UARTSetupFlags").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, controller_name_t &s) {
	strncpy(s.ControllerName, obj.property("ControllerName").toString().toLocal8Bit().data(), 16);
	s.CtrlFlags = obj.property("CtrlFlags").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, get_position_t &s) {
	s.Position = obj.property("Position").toInt32();
	s.uPosition = obj.property("uPosition").toInt32();
	s.EncPosition = obj.property("EncPosition").toInteger();
}

static void fromScriptValue(const QScriptValue &obj, set_position_t &s) {
	s.Position = obj.property("Position").toInt32();
	s.uPosition = obj.property("uPosition").toInt32();
	s.EncPosition = obj.property("EncPosition").toInteger();
	s.PosFlags = obj.property("PosFlags").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, chart_data_t &s) {
	s.WindingVoltageA = obj.property("WindingVoltageA").toInt32();
	s.WindingVoltageB = obj.property("WindingVoltageB").toInt32();
	s.WindingVoltageC = obj.property("WindingVoltageC").toInt32();
	s.WindingCurrentA = obj.property("WindingCurrentA").toInt32();
	s.WindingCurrentB = obj.property("WindingCurrentB").toInt32();
	s.WindingCurrentC = obj.property("WindingCurrentC").toInt32();
	s.Pot = obj.property("Pot").toUInt32();
	s.Joy = obj.property("Joy").toUInt32();
	s.DutyCycle = obj.property("DutyCycle").toInt32();
}

static void fromScriptValue(const QScriptValue &obj, analog_data_t &s) {
	s.A1Voltage_ADC = obj.property("A1Voltage_ADC").toUInt32();
	s.A2Voltage_ADC = obj.property("A2Voltage_ADC").toUInt32();
	s.B1Voltage_ADC = obj.property("B1Voltage_ADC").toUInt32();
	s.B2Voltage_ADC = obj.property("B2Voltage_ADC").toUInt32();
	s.SupVoltage_ADC = obj.property("SupVoltage_ADC").toUInt32();
	s.ACurrent_ADC = obj.property("ACurrent_ADC").toUInt32();
	s.BCurrent_ADC = obj.property("BCurrent_ADC").toUInt32();
	s.FullCurrent_ADC = obj.property("FullCurrent_ADC").toUInt32();
	s.Temp_ADC = obj.property("Temp_ADC").toUInt32();
	s.Joy_ADC = obj.property("Joy_ADC").toUInt32();
	s.Pot_ADC = obj.property("Pot_ADC").toUInt32();
	s.L5_ADC = obj.property("L5_ADC").toUInt32();
	s.H5_ADC = obj.property("H5_ADC").toUInt32();
	s.A1Voltage = obj.property("A1Voltage").toInt32();
	s.A2Voltage = obj.property("A2Voltage").toInt32();
	s.B1Voltage = obj.property("B1Voltage").toInt32();
	s.B2Voltage = obj.property("B2Voltage").toInt32();
	s.SupVoltage = obj.property("SupVoltage").toInt32();
	s.ACurrent = obj.property("ACurrent").toInt32();
	s.BCurrent = obj.property("BCurrent").toInt32();
	s.FullCurrent = obj.property("FullCurrent").toInt32();
	s.Temp = obj.property("Temp").toInt32();
	s.Joy = obj.property("Joy").toInt32();
	s.Pot = obj.property("Pot").toInt32();
	s.L5 = obj.property("L5").toInt32();
	s.H5 = obj.property("H5").toInt32();
	s.deprecated = obj.property("deprecated").toUInt32();
	s.R = obj.property("R").toInt32();
	s.L = obj.property("L").toInt32();
}

static void fromScriptValue(const QScriptValue &obj, debug_read_t &s) {
	for (int i=0; i<128; i++) { s.DebugData[i] = obj.property("DebugData").property(i).toUInt32(); }
}


static void fromScriptValue(const QScriptValue &obj, stage_name_t &s) {
	strncpy(s.PositionerName, obj.property("PositionerName").toString().toLocal8Bit().data(), 16);
}

static void fromScriptValue(const QScriptValue &obj, stage_information_t &s) {
	strncpy(s.Manufacturer, obj.property("Manufacturer").toString().toLocal8Bit().data(), 16);
	strncpy(s.PartNumber, obj.property("PartNumber").toString().toLocal8Bit().data(), 24);
}

static void fromScriptValue(const QScriptValue &obj, stage_settings_t &s) {
	s.LeadScrewPitch = obj.property("LeadScrewPitch").toNumber();
	strncpy(s.Units, obj.property("Units").toString().toLocal8Bit().data(), 8);
	s.MaxSpeed = obj.property("MaxSpeed").toNumber();
	s.TravelRange = obj.property("TravelRange").toNumber();
	s.SupplyVoltageMin = obj.property("SupplyVoltageMin").toNumber();
	s.SupplyVoltageMax = obj.property("SupplyVoltageMax").toNumber();
	s.MaxCurrentConsumption = obj.property("MaxCurrentConsumption").toNumber();
	s.HorizontalLoadCapacity = obj.property("HorizontalLoadCapacity").toNumber();
	s.VerticalLoadCapacity = obj.property("VerticalLoadCapacity").toNumber();
}

static void fromScriptValue(const QScriptValue &obj, motor_information_t &s) {
	strncpy(s.Manufacturer, obj.property("Manufacturer").toString().toLocal8Bit().data(), 16);
	strncpy(s.PartNumber, obj.property("PartNumber").toString().toLocal8Bit().data(), 24);
}

static void fromScriptValue(const QScriptValue &obj, motor_settings_t &s) {
	s.MotorType = obj.property("MotorType").toUInt32();
	s.ReservedField = obj.property("ReservedField").toUInt32();
	s.Poles = obj.property("Poles").toUInt32();
	s.Phases = obj.property("Phases").toUInt32();
	s.NominalVoltage = obj.property("NominalVoltage").toNumber();
	s.NominalCurrent = obj.property("NominalCurrent").toNumber();
	s.NominalSpeed = obj.property("NominalSpeed").toNumber();
	s.NominalTorque = obj.property("NominalTorque").toNumber();
	s.NominalPower = obj.property("NominalPower").toNumber();
	s.WindingResistance = obj.property("WindingResistance").toNumber();
	s.WindingInductance = obj.property("WindingInductance").toNumber();
	s.RotorInertia = obj.property("RotorInertia").toNumber();
	s.StallTorque = obj.property("StallTorque").toNumber();
	s.DetentTorque = obj.property("DetentTorque").toNumber();
	s.TorqueConstant = obj.property("TorqueConstant").toNumber();
	s.SpeedConstant = obj.property("SpeedConstant").toNumber();
	s.SpeedTorqueGradient = obj.property("SpeedTorqueGradient").toNumber();
	s.MechanicalTimeConstant = obj.property("MechanicalTimeConstant").toNumber();
	s.MaxSpeed = obj.property("MaxSpeed").toNumber();
	s.MaxCurrent = obj.property("MaxCurrent").toNumber();
	s.MaxCurrentTime = obj.property("MaxCurrentTime").toNumber();
	s.NoLoadCurrent = obj.property("NoLoadCurrent").toNumber();
	s.NoLoadSpeed = obj.property("NoLoadSpeed").toNumber();
}

static void fromScriptValue(const QScriptValue &obj, encoder_information_t &s) {
	strncpy(s.Manufacturer, obj.property("Manufacturer").toString().toLocal8Bit().data(), 16);
	strncpy(s.PartNumber, obj.property("PartNumber").toString().toLocal8Bit().data(), 24);
}

static void fromScriptValue(const QScriptValue &obj, encoder_settings_t &s) {
	s.MaxOperatingFrequency = obj.property("MaxOperatingFrequency").toNumber();
	s.SupplyVoltageMin = obj.property("SupplyVoltageMin").toNumber();
	s.SupplyVoltageMax = obj.property("SupplyVoltageMax").toNumber();
	s.MaxCurrentConsumption = obj.property("MaxCurrentConsumption").toNumber();
	s.PPR = obj.property("PPR").toUInt32();
	s.EncoderSettings = obj.property("EncoderSettings").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, hallsensor_information_t &s) {
	strncpy(s.Manufacturer, obj.property("Manufacturer").toString().toLocal8Bit().data(), 16);
	strncpy(s.PartNumber, obj.property("PartNumber").toString().toLocal8Bit().data(), 24);
}

static void fromScriptValue(const QScriptValue &obj, hallsensor_settings_t &s) {
	s.MaxOperatingFrequency = obj.property("MaxOperatingFrequency").toNumber();
	s.SupplyVoltageMin = obj.property("SupplyVoltageMin").toNumber();
	s.SupplyVoltageMax = obj.property("SupplyVoltageMax").toNumber();
	s.MaxCurrentConsumption = obj.property("MaxCurrentConsumption").toNumber();
	s.PPR = obj.property("PPR").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, gear_information_t &s) {
	strncpy(s.Manufacturer, obj.property("Manufacturer").toString().toLocal8Bit().data(), 16);
	strncpy(s.PartNumber, obj.property("PartNumber").toString().toLocal8Bit().data(), 24);
}

static void fromScriptValue(const QScriptValue &obj, gear_settings_t &s) {
	s.ReductionIn = obj.property("ReductionIn").toNumber();
	s.ReductionOut = obj.property("ReductionOut").toNumber();
	s.RatedInputTorque = obj.property("RatedInputTorque").toNumber();
	s.RatedInputSpeed = obj.property("RatedInputSpeed").toNumber();
	s.MaxOutputBacklash = obj.property("MaxOutputBacklash").toNumber();
	s.InputInertia = obj.property("InputInertia").toNumber();
	s.Efficiency = obj.property("Efficiency").toNumber();
}

static void fromScriptValue(const QScriptValue &obj, accessories_settings_t &s) {
	strncpy(s.MagneticBrakeInfo, obj.property("MagneticBrakeInfo").toString().toLocal8Bit().data(), 24);
	s.MBRatedVoltage = obj.property("MBRatedVoltage").toNumber();
	s.MBRatedCurrent = obj.property("MBRatedCurrent").toNumber();
	s.MBTorque = obj.property("MBTorque").toNumber();
	s.MBSettings = obj.property("MBSettings").toUInt32();
	strncpy(s.TemperatureSensorInfo, obj.property("TemperatureSensorInfo").toString().toLocal8Bit().data(), 24);
	s.TSMin = obj.property("TSMin").toNumber();
	s.TSMax = obj.property("TSMax").toNumber();
	s.TSGrad = obj.property("TSGrad").toNumber();
	s.TSSettings = obj.property("TSSettings").toUInt32();
	s.LimitSwitchesSettings = obj.property("LimitSwitchesSettings").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, status_t &s) {
	s.MoveSts   = obj.property("MoveSts").toUInt32();
	s.MvCmdSts  = obj.property("MvCmdSts").toUInt32();
	s.PWRSts    = obj.property("PWRSts").toUInt32();
	s.EncSts    = obj.property("EncSts").toUInt32();
	s.WindSts   = obj.property("WindSts").toUInt32();

	s.CurPosition = obj.property("CurPosition").toInt32();
	s.uCurPosition = obj.property("uCurPosition").toInt32();
	s.EncPosition = obj.property("EncPosition").toInt32(); // int64 must be here!
	
	s.CurSpeed	  = obj.property("CurSpeed").toInt32();
	s.uCurSpeed	  = obj.property("uCurSpeed").toInt32();
	s.Ipwr		  = obj.property("Ipwr").toInt32();
	s.Upwr		  = obj.property("Upwr").toInt32();
	s.Iusb		  = obj.property("Iusb").toInt32();
	s.Uusb		  = obj.property("Uusb").toInt32();
	s.CurT		  = obj.property("CurT").toInt32();

	s.Flags            = obj.property("Flags").toUInt32();
	s.GPIOFlags		   = obj.property("GPIOFlags").toUInt32();
	s.CmdBufFreeSpace  = obj.property("CmdBufFreeSpace").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, status_calb_t &s) {
	s.MoveSts = obj.property("MoveSts").toUInt32();
	s.MvCmdSts = obj.property("MvCmdSts").toUInt32();
	s.PWRSts = obj.property("PWRSts").toUInt32();
	s.EncSts = obj.property("EncSts").toUInt32();
	s.WindSts = obj.property("WindSts").toUInt32();

	s.CurPosition = obj.property("CurPosition").toInt32();
	s.EncPosition = obj.property("EncPosition").toInt32(); // int64 must be here!

	s.CurSpeed = obj.property("CurSpeed").toInt32();
	s.Ipwr = obj.property("Ipwr").toInt32();
	s.Upwr = obj.property("Upwr").toInt32();
	s.Iusb = obj.property("Iusb").toInt32();
	s.Uusb = obj.property("Uusb").toInt32();
	s.CurT = obj.property("CurT").toInt32();

	s.Flags = obj.property("Flags").toUInt32();
	s.GPIOFlags = obj.property("GPIOFlags").toUInt32();
	s.CmdBufFreeSpace = obj.property("CmdBufFreeSpace").toUInt32();
}


static void fromScriptValue(const QScriptValue &obj, device_information_t &s) {
	s.Major = obj.property("Major").toUInt32();
	s.Minor = obj.property("Minor").toUInt32();
	s.Release = obj.property("Release").toUInt32();
	strncpy(s.Manufacturer, obj.property("Manufacturer").toString().toLocal8Bit().data(), 4);
	strncpy(s.ManufacturerId, obj.property("ManufacturerId").toString().toLocal8Bit().data(), 2);
	strncpy(s.ProductDescription, obj.property("ProductDescription").toString().toLocal8Bit().data(), 8);
}