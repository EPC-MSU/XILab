Q_DECLARE_METATYPE(home_settings_calb_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const home_settings_calb_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("FastHome", s.FastHome);
	obj.setProperty("SlowHome", s.SlowHome);
	obj.setProperty("HomeDelta", s.HomeDelta);
	obj.setProperty("HomeFlags", s.HomeFlags);
	return obj;
}
Q_DECLARE_METATYPE(move_settings_calb_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const move_settings_calb_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("Speed", s.Speed);
	obj.setProperty("Accel", s.Accel);
	obj.setProperty("Decel", s.Decel);
	obj.setProperty("AntiplaySpeed", s.AntiplaySpeed);
	return obj;
}
Q_DECLARE_METATYPE(engine_settings_calb_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const engine_settings_calb_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("NomVoltage", s.NomVoltage);
	obj.setProperty("NomCurrent", s.NomCurrent);
	obj.setProperty("NomSpeed", s.NomSpeed);
	obj.setProperty("EngineFlags", s.EngineFlags);
	obj.setProperty("Antiplay", s.Antiplay);
	obj.setProperty("MicrostepMode", s.MicrostepMode);
	obj.setProperty("StepsPerRev", s.StepsPerRev);
	return obj;
}
Q_DECLARE_METATYPE(edges_settings_calb_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const edges_settings_calb_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("BorderFlags", s.BorderFlags);
	obj.setProperty("EnderFlags", s.EnderFlags);
	obj.setProperty("LeftBorder", s.LeftBorder);
	obj.setProperty("RightBorder", s.RightBorder);
	return obj;
}
Q_DECLARE_METATYPE(sync_in_settings_calb_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const sync_in_settings_calb_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("SyncInFlags", s.SyncInFlags);
	obj.setProperty("ClutterTime", s.ClutterTime);
	obj.setProperty("Position", s.Position);
	obj.setProperty("Speed", s.Speed);
	return obj;
}
Q_DECLARE_METATYPE(sync_out_settings_calb_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const sync_out_settings_calb_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("SyncOutFlags", s.SyncOutFlags);
	obj.setProperty("SyncOutPulseSteps", s.SyncOutPulseSteps);
	obj.setProperty("SyncOutPeriod", s.SyncOutPeriod);
	obj.setProperty("Accuracy", s.Accuracy);
	return obj;
}
Q_DECLARE_METATYPE(control_settings_calb_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const control_settings_calb_t &s) {
	QScriptValue obj = engine->newObject();
	QScriptValue MaxSpeed = engine->newObject();
	for (int i=0; i<10; i++) { MaxSpeed.setProperty(i, s.MaxSpeed[i]); }
	obj.setProperty("MaxSpeed", MaxSpeed);
	QScriptValue Timeout = engine->newObject();
	for (int i=0; i<9; i++) { Timeout.setProperty(i, s.Timeout[i]); }
	obj.setProperty("Timeout", Timeout);
	obj.setProperty("MaxClickTime", s.MaxClickTime);
	obj.setProperty("Flags", s.Flags);
	obj.setProperty("DeltaPosition", s.DeltaPosition);
	return obj;
}
Q_DECLARE_METATYPE(get_position_calb_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const get_position_calb_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("Position", s.Position);
	obj.setProperty("EncPosition", (double)s.EncPosition);
	return obj;
}
Q_DECLARE_METATYPE(set_position_calb_t);
static QScriptValue toScriptValue(QScriptEngine *engine, const set_position_calb_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("Position", s.Position);
	obj.setProperty("EncPosition", (double)s.EncPosition);
	obj.setProperty("PosFlags", s.PosFlags);
	return obj;
}
