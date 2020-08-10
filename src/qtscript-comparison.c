bool operator==(const libximc::feedback_settings_t& l, const libximc::feedback_settings_t& r)
{
	bool eq = true;
	eq &= (l.IPS == r.IPS);
	eq &= (l.FeedbackType == r.FeedbackType);
	eq &= (l.FeedbackFlags == r.FeedbackFlags);
	eq &= (l.CountsPerTurn == r.CountsPerTurn);
	return eq;
}

bool operator==(const libximc::home_settings_t& l, const libximc::home_settings_t& r)
{
	bool eq = true;
	eq &= (l.FastHome == r.FastHome);
	eq &= (l.uFastHome == r.uFastHome);
	eq &= (l.SlowHome == r.SlowHome);
	eq &= (l.uSlowHome == r.uSlowHome);
	eq &= (l.HomeDelta == r.HomeDelta);
	eq &= (l.uHomeDelta == r.uHomeDelta);
	eq &= (l.HomeFlags == r.HomeFlags);
	return eq;
}

bool operator==(const libximc::move_settings_t& l, const libximc::move_settings_t& r)
{
	bool eq = true;
	eq &= (l.Speed == r.Speed);
	eq &= (l.uSpeed == r.uSpeed);
	eq &= (l.Accel == r.Accel);
	eq &= (l.Decel == r.Decel);
	eq &= (l.AntiplaySpeed == r.AntiplaySpeed);
	eq &= (l.uAntiplaySpeed == r.uAntiplaySpeed);
	return eq;
}

bool operator==(const libximc::engine_settings_t& l, const libximc::engine_settings_t& r)
{
	bool eq = true;
	eq &= (l.NomVoltage == r.NomVoltage);
	eq &= (l.NomCurrent == r.NomCurrent);
	eq &= (l.NomSpeed == r.NomSpeed);
	eq &= (l.uNomSpeed == r.uNomSpeed);
	eq &= (l.EngineFlags == r.EngineFlags);
	eq &= (l.Antiplay == r.Antiplay);
	eq &= (l.MicrostepMode == r.MicrostepMode);
	eq &= (l.StepsPerRev == r.StepsPerRev);
	return eq;
}

bool operator==(const libximc::entype_settings_t& l, const libximc::entype_settings_t& r)
{
	bool eq = true;
	eq &= (l.EngineType == r.EngineType);
	eq &= (l.DriverType == r.DriverType);
	return eq;
}

bool operator==(const libximc::power_settings_t& l, const libximc::power_settings_t& r)
{
	bool eq = true;
	eq &= (l.HoldCurrent == r.HoldCurrent);
	eq &= (l.CurrReductDelay == r.CurrReductDelay);
	eq &= (l.PowerOffDelay == r.PowerOffDelay);
	eq &= (l.CurrentSetTime == r.CurrentSetTime);
	eq &= (l.PowerFlags == r.PowerFlags);
	return eq;
}

bool operator==(const libximc::secure_settings_t& l, const libximc::secure_settings_t& r)
{
	bool eq = true;
	eq &= (l.LowUpwrOff == r.LowUpwrOff);
	eq &= (l.CriticalIpwr == r.CriticalIpwr);
	eq &= (l.CriticalUpwr == r.CriticalUpwr);
	eq &= (l.CriticalT == r.CriticalT);
	eq &= (l.CriticalIusb == r.CriticalIusb);
	eq &= (l.CriticalUusb == r.CriticalUusb);
	eq &= (l.MinimumUusb == r.MinimumUusb);
	eq &= (l.Flags == r.Flags);
	return eq;
}

bool operator==(const libximc::edges_settings_t& l, const libximc::edges_settings_t& r)
{
	bool eq = true;
	eq &= (l.BorderFlags == r.BorderFlags);
	eq &= (l.EnderFlags == r.EnderFlags);
	eq &= (l.LeftBorder == r.LeftBorder);
	eq &= (l.uLeftBorder == r.uLeftBorder);
	eq &= (l.RightBorder == r.RightBorder);
	eq &= (l.uRightBorder == r.uRightBorder);
	return eq;
}

bool operator==(const libximc::pid_settings_t& l, const libximc::pid_settings_t& r)
{
	bool eq = true;
	eq &= (l.KpU == r.KpU);
	eq &= (l.KiU == r.KiU);
	eq &= (l.KdU == r.KdU);
	eq &= (l.Kpf == r.Kpf);
	eq &= (l.Kif == r.Kif);
	eq &= (l.Kdf == r.Kdf);
	return eq;
}

bool operator==(const libximc::emf_settings_t& l, const libximc::emf_settings_t& r)
{
	bool eq = true;
	eq &= (l.L == r.L);
	eq &= (l.R == r.R);
	eq &= (l.Km == r.Km);
	eq &= (l.BackEMFFlags == r.BackEMFFlags);
	return eq;
}

bool operator==(const libximc::sync_in_settings_t& l, const libximc::sync_in_settings_t& r)
{
	bool eq = true;
	eq &= (l.SyncInFlags == r.SyncInFlags);
	eq &= (l.ClutterTime == r.ClutterTime);
	eq &= (l.Position == r.Position);
	eq &= (l.uPosition == r.uPosition);
	eq &= (l.Speed == r.Speed);
	eq &= (l.uSpeed == r.uSpeed);
	return eq;
}

bool operator==(const libximc::sync_out_settings_t& l, const libximc::sync_out_settings_t& r)
{
	bool eq = true;
	eq &= (l.SyncOutFlags == r.SyncOutFlags);
	eq &= (l.SyncOutPulseSteps == r.SyncOutPulseSteps);
	eq &= (l.SyncOutPeriod == r.SyncOutPeriod);
	eq &= (l.Accuracy == r.Accuracy);
	eq &= (l.uAccuracy == r.uAccuracy);
	return eq;
}

bool operator==(const libximc::extio_settings_t& l, const libximc::extio_settings_t& r)
{
	bool eq = true;
	eq &= (l.EXTIOSetupFlags == r.EXTIOSetupFlags);
	eq &= (l.EXTIOModeFlags == r.EXTIOModeFlags);
	return eq;
}

bool operator==(const libximc::brake_settings_t& l, const libximc::brake_settings_t& r)
{
	bool eq = true;
	eq &= (l.t1 == r.t1);
	eq &= (l.t2 == r.t2);
	eq &= (l.t3 == r.t3);
	eq &= (l.t4 == r.t4);
	eq &= (l.BrakeFlags == r.BrakeFlags);
	return eq;
}

bool operator==(const libximc::control_settings_t& l, const libximc::control_settings_t& r)
{
	bool eq = true;
	for (int i=0; i<10; i++) { eq &= (l.MaxSpeed[i] == r.MaxSpeed[i]); }
	for (int i=0; i<10; i++) { eq &= (l.uMaxSpeed[i] == r.uMaxSpeed[i]); }
	for (int i=0; i<9; i++) { eq &= (l.Timeout[i] == r.Timeout[i]); }
	eq &= (l.MaxClickTime == r.MaxClickTime);
	eq &= (l.Flags == r.Flags);
	eq &= (l.DeltaPosition == r.DeltaPosition);
	eq &= (l.uDeltaPosition == r.uDeltaPosition);
	return eq;
}

bool operator==(const libximc::joystick_settings_t& l, const libximc::joystick_settings_t& r)
{
	bool eq = true;
	eq &= (l.JoyLowEnd == r.JoyLowEnd);
	eq &= (l.JoyCenter == r.JoyCenter);
	eq &= (l.JoyHighEnd == r.JoyHighEnd);
	eq &= (l.ExpFactor == r.ExpFactor);
	eq &= (l.DeadZone == r.DeadZone);
	eq &= (l.JoyFlags == r.JoyFlags);
	return eq;
}

bool operator==(const libximc::ctp_settings_t& l, const libximc::ctp_settings_t& r)
{
	bool eq = true;
	eq &= (l.CTPMinError == r.CTPMinError);
	eq &= (l.CTPFlags == r.CTPFlags);
	return eq;
}

bool operator==(const libximc::uart_settings_t& l, const libximc::uart_settings_t& r)
{
	bool eq = true;
	eq &= (l.Speed == r.Speed);
	eq &= (l.UARTSetupFlags == r.UARTSetupFlags);
	return eq;
}

bool operator==(const libximc::controller_name_t& l, const libximc::controller_name_t& r)
{
	bool eq = true;
	eq &= (strcmp(l.ControllerName, r.ControllerName) == 0);
	eq &= (l.CtrlFlags == r.CtrlFlags);
	return eq;
}

bool operator==(const libximc::command_add_sync_in_action_t& l, const libximc::command_add_sync_in_action_t& r)
{
	bool eq = true;
	eq &= (l.Position == r.Position);
	eq &= (l.uPosition == r.uPosition);
	eq &= (l.Time == r.Time);
	return eq;
}

bool operator==(const libximc::get_position_t& l, const libximc::get_position_t& r)
{
	bool eq = true;
	eq &= (l.Position == r.Position);
	eq &= (l.uPosition == r.uPosition);
	eq &= (l.EncPosition == r.EncPosition);
	return eq;
}

bool operator==(const libximc::set_position_t& l, const libximc::set_position_t& r)
{
	bool eq = true;
	eq &= (l.Position == r.Position);
	eq &= (l.uPosition == r.uPosition);
	eq &= (l.EncPosition == r.EncPosition);
	eq &= (l.PosFlags == r.PosFlags);
	return eq;
}

bool operator==(const libximc::chart_data_t& l, const libximc::chart_data_t& r)
{
	bool eq = true;
	eq &= (l.WindingVoltageA == r.WindingVoltageA);
	eq &= (l.WindingVoltageB == r.WindingVoltageB);
	eq &= (l.WindingVoltageC == r.WindingVoltageC);
	eq &= (l.WindingCurrentA == r.WindingCurrentA);
	eq &= (l.WindingCurrentB == r.WindingCurrentB);
	eq &= (l.WindingCurrentC == r.WindingCurrentC);
	eq &= (l.Pot == r.Pot);
	eq &= (l.Joy == r.Joy);
	eq &= (l.DutyCycle == r.DutyCycle);
	return eq;
}

bool operator==(const libximc::serial_number_t& l, const libximc::serial_number_t& r)
{
	bool eq = true;
	eq &= (l.SN == r.SN);
	for (int i=0; i<32; i++) { eq &= (l.Key[i] == r.Key[i]); }
	return eq;
}

bool operator==(const libximc::analog_data_t& l, const libximc::analog_data_t& r)
{
	bool eq = true;
	eq &= (l.A1Voltage_ADC == r.A1Voltage_ADC);
	eq &= (l.A2Voltage_ADC == r.A2Voltage_ADC);
	eq &= (l.B1Voltage_ADC == r.B1Voltage_ADC);
	eq &= (l.B2Voltage_ADC == r.B2Voltage_ADC);
	eq &= (l.SupVoltage_ADC == r.SupVoltage_ADC);
	eq &= (l.ACurrent_ADC == r.ACurrent_ADC);
	eq &= (l.BCurrent_ADC == r.BCurrent_ADC);
	eq &= (l.FullCurrent_ADC == r.FullCurrent_ADC);
	eq &= (l.Temp_ADC == r.Temp_ADC);
	eq &= (l.Joy_ADC == r.Joy_ADC);
	eq &= (l.Pot_ADC == r.Pot_ADC);
	eq &= (l.L5_ADC == r.L5_ADC);
	eq &= (l.H5_ADC == r.H5_ADC);
	eq &= (l.A1Voltage == r.A1Voltage);
	eq &= (l.A2Voltage == r.A2Voltage);
	eq &= (l.B1Voltage == r.B1Voltage);
	eq &= (l.B2Voltage == r.B2Voltage);
	eq &= (l.SupVoltage == r.SupVoltage);
	eq &= (l.ACurrent == r.ACurrent);
	eq &= (l.BCurrent == r.BCurrent);
	eq &= (l.FullCurrent == r.FullCurrent);
	eq &= (l.Temp == r.Temp);
	eq &= (l.Joy == r.Joy);
	eq &= (l.Pot == r.Pot);
	eq &= (l.L5 == r.L5);
	eq &= (l.H5 == r.H5);
	eq &= (l.deprecated == r.deprecated);
	eq &= (l.R == r.R);
	eq &= (l.L == r.L);
	return eq;
}

bool operator==(const libximc::debug_read_t& l, const libximc::debug_read_t& r)
{
	bool eq = true;
	for (int i=0; i<128; i++) { eq &= (l.DebugData[i] == r.DebugData[i]); }
	return eq;
}

bool operator==(const libximc::stage_name_t& l, const libximc::stage_name_t& r)
{
	bool eq = true;
	eq &= (strcmp(l.PositionerName, r.PositionerName) == 0);
	return eq;
}

bool operator==(const libximc::stage_information_t& l, const libximc::stage_information_t& r)
{
	bool eq = true;
	eq &= (strcmp(l.Manufacturer, r.Manufacturer) == 0);
	eq &= (strcmp(l.PartNumber, r.PartNumber) == 0);
	return eq;
}

bool operator==(const libximc::stage_settings_t& l, const libximc::stage_settings_t& r)
{
	bool eq = true;
	eq &= (l.LeadScrewPitch == r.LeadScrewPitch);
	eq &= (strcmp(l.Units, r.Units) == 0);
	eq &= (l.MaxSpeed == r.MaxSpeed);
	eq &= (l.TravelRange == r.TravelRange);
	eq &= (l.SupplyVoltageMin == r.SupplyVoltageMin);
	eq &= (l.SupplyVoltageMax == r.SupplyVoltageMax);
	eq &= (l.MaxCurrentConsumption == r.MaxCurrentConsumption);
	eq &= (l.HorizontalLoadCapacity == r.HorizontalLoadCapacity);
	eq &= (l.VerticalLoadCapacity == r.VerticalLoadCapacity);
	return eq;
}

bool operator==(const libximc::motor_information_t& l, const libximc::motor_information_t& r)
{
	bool eq = true;
	eq &= (strcmp(l.Manufacturer, r.Manufacturer) == 0);
	eq &= (strcmp(l.PartNumber, r.PartNumber) == 0);
	return eq;
}

bool operator==(const libximc::motor_settings_t& l, const libximc::motor_settings_t& r)
{
	bool eq = true;
	eq &= (l.MotorType == r.MotorType);
	eq &= (l.ReservedField == r.ReservedField);
	eq &= (l.Poles == r.Poles);
	eq &= (l.Phases == r.Phases);
	eq &= (l.NominalVoltage == r.NominalVoltage);
	eq &= (l.NominalCurrent == r.NominalCurrent);
	eq &= (l.NominalSpeed == r.NominalSpeed);
	eq &= (l.NominalTorque == r.NominalTorque);
	eq &= (l.NominalPower == r.NominalPower);
	eq &= (l.WindingResistance == r.WindingResistance);
	eq &= (l.WindingInductance == r.WindingInductance);
	eq &= (l.RotorInertia == r.RotorInertia);
	eq &= (l.StallTorque == r.StallTorque);
	eq &= (l.DetentTorque == r.DetentTorque);
	eq &= (l.TorqueConstant == r.TorqueConstant);
	eq &= (l.SpeedConstant == r.SpeedConstant);
	eq &= (l.SpeedTorqueGradient == r.SpeedTorqueGradient);
	eq &= (l.MechanicalTimeConstant == r.MechanicalTimeConstant);
	eq &= (l.MaxSpeed == r.MaxSpeed);
	eq &= (l.MaxCurrent == r.MaxCurrent);
	eq &= (l.MaxCurrentTime == r.MaxCurrentTime);
	eq &= (l.NoLoadCurrent == r.NoLoadCurrent);
	eq &= (l.NoLoadSpeed == r.NoLoadSpeed);
	return eq;
}

bool operator==(const libximc::encoder_information_t& l, const libximc::encoder_information_t& r)
{
	bool eq = true;
	eq &= (strcmp(l.Manufacturer, r.Manufacturer) == 0);
	eq &= (strcmp(l.PartNumber, r.PartNumber) == 0);
	return eq;
}

bool operator==(const libximc::encoder_settings_t& l, const libximc::encoder_settings_t& r)
{
	bool eq = true;
	eq &= (l.MaxOperatingFrequency == r.MaxOperatingFrequency);
	eq &= (l.SupplyVoltageMin == r.SupplyVoltageMin);
	eq &= (l.SupplyVoltageMax == r.SupplyVoltageMax);
	eq &= (l.MaxCurrentConsumption == r.MaxCurrentConsumption);
	eq &= (l.PPR == r.PPR);
	eq &= (l.EncoderSettings == r.EncoderSettings);
	return eq;
}

bool operator==(const libximc::hallsensor_information_t& l, const libximc::hallsensor_information_t& r)
{
	bool eq = true;
	eq &= (strcmp(l.Manufacturer, r.Manufacturer) == 0);
	eq &= (strcmp(l.PartNumber, r.PartNumber) == 0);
	return eq;
}

bool operator==(const libximc::hallsensor_settings_t& l, const libximc::hallsensor_settings_t& r)
{
	bool eq = true;
	eq &= (l.MaxOperatingFrequency == r.MaxOperatingFrequency);
	eq &= (l.SupplyVoltageMin == r.SupplyVoltageMin);
	eq &= (l.SupplyVoltageMax == r.SupplyVoltageMax);
	eq &= (l.MaxCurrentConsumption == r.MaxCurrentConsumption);
	eq &= (l.PPR == r.PPR);
	return eq;
}

bool operator==(const libximc::gear_information_t& l, const libximc::gear_information_t& r)
{
	bool eq = true;
	eq &= (strcmp(l.Manufacturer, r.Manufacturer) == 0);
	eq &= (strcmp(l.PartNumber, r.PartNumber) == 0);
	return eq;
}

bool operator==(const libximc::gear_settings_t& l, const libximc::gear_settings_t& r)
{
	bool eq = true;
	eq &= (l.ReductionIn == r.ReductionIn);
	eq &= (l.ReductionOut == r.ReductionOut);
	eq &= (l.RatedInputTorque == r.RatedInputTorque);
	eq &= (l.RatedInputSpeed == r.RatedInputSpeed);
	eq &= (l.MaxOutputBacklash == r.MaxOutputBacklash);
	eq &= (l.InputInertia == r.InputInertia);
	eq &= (l.Efficiency == r.Efficiency);
	return eq;
}

bool operator==(const libximc::accessories_settings_t& l, const libximc::accessories_settings_t& r)
{
	bool eq = true;
	eq &= (strcmp(l.MagneticBrakeInfo, r.MagneticBrakeInfo) == 0);
	eq &= (l.MBRatedVoltage == r.MBRatedVoltage);
	eq &= (l.MBRatedCurrent == r.MBRatedCurrent);
	eq &= (l.MBTorque == r.MBTorque);
	eq &= (l.MBSettings == r.MBSettings);
	eq &= (strcmp(l.TemperatureSensorInfo, r.TemperatureSensorInfo) == 0);
	eq &= (l.TSMin == r.TSMin);
	eq &= (l.TSMax == r.TSMax);
	eq &= (l.TSGrad == r.TSGrad);
	eq &= (l.TSSettings == r.TSSettings);
	eq &= (l.LimitSwitchesSettings == r.LimitSwitchesSettings);
	return eq;
}

