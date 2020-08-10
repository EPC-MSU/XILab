
static void fromScriptValue(const QScriptValue &obj, home_settings_calb_t &s) {
	s.FastHome = obj.property("FastHome").toNumber();
	s.SlowHome = obj.property("SlowHome").toNumber();
	s.HomeDelta = obj.property("HomeDelta").toNumber();
	s.HomeFlags = obj.property("HomeFlags").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, move_settings_calb_t &s) {
	s.Speed = obj.property("Speed").toNumber();
	s.Accel = obj.property("Accel").toNumber();
	s.Decel = obj.property("Decel").toNumber();
	s.AntiplaySpeed = obj.property("AntiplaySpeed").toNumber();
}

static void fromScriptValue(const QScriptValue &obj, engine_settings_calb_t &s) {
	s.NomVoltage = obj.property("NomVoltage").toUInt32();
	s.NomCurrent = obj.property("NomCurrent").toUInt32();
	s.NomSpeed = obj.property("NomSpeed").toNumber();
	s.EngineFlags = obj.property("EngineFlags").toUInt32();
	s.Antiplay = obj.property("Antiplay").toNumber();
	s.MicrostepMode = obj.property("MicrostepMode").toUInt32();
	s.StepsPerRev = obj.property("StepsPerRev").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, edges_settings_calb_t &s) {
	s.BorderFlags = obj.property("BorderFlags").toUInt32();
	s.EnderFlags = obj.property("EnderFlags").toUInt32();
	s.LeftBorder = obj.property("LeftBorder").toNumber();
	s.RightBorder = obj.property("RightBorder").toNumber();
}

static void fromScriptValue(const QScriptValue &obj, sync_in_settings_calb_t &s) {
	s.SyncInFlags = obj.property("SyncInFlags").toUInt32();
	s.ClutterTime = obj.property("ClutterTime").toUInt32();
	s.Position = obj.property("Position").toNumber();
	s.Speed = obj.property("Speed").toNumber();
}

static void fromScriptValue(const QScriptValue &obj, sync_out_settings_calb_t &s) {
	s.SyncOutFlags = obj.property("SyncOutFlags").toUInt32();
	s.SyncOutPulseSteps = obj.property("SyncOutPulseSteps").toUInt32();
	s.SyncOutPeriod = obj.property("SyncOutPeriod").toUInt32();
	s.Accuracy = obj.property("Accuracy").toNumber();
}

static void fromScriptValue(const QScriptValue &obj, control_settings_calb_t &s) {
	for (int i=0; i<10; i++) { s.MaxSpeed[i] = obj.property("MaxSpeed").property(i).toNumber(); }
	for (int i=0; i<9; i++) { s.Timeout[i] = obj.property("Timeout").property(i).toUInt32(); }
	s.MaxClickTime = obj.property("MaxClickTime").toUInt32();
	s.Flags = obj.property("Flags").toUInt32();
	s.DeltaPosition = obj.property("DeltaPosition").toNumber();
}

static void fromScriptValue(const QScriptValue &obj, command_add_sync_in_action_calb_t &s) {
	s.Position = obj.property("Position").toNumber();
	s.Time = obj.property("Time").toUInt32();
}

static void fromScriptValue(const QScriptValue &obj, get_position_calb_t &s) {
	s.Position = obj.property("Position").toNumber();
	s.EncPosition = obj.property("EncPosition").toInteger();
}

static void fromScriptValue(const QScriptValue &obj, set_position_calb_t &s) {
	s.Position = obj.property("Position").toNumber();
	s.EncPosition = obj.property("EncPosition").toInteger();
	s.PosFlags = obj.property("PosFlags").toUInt32();
}
