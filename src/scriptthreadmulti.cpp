#include <scriptthreadmulti.h>
#include <multiaxis.h>
#include <axisobject.h>

QList<DeviceInterface*>* di;
ScriptThreadMulti* self;


Q_DECLARE_METATYPE(status_t);
Q_DECLARE_METATYPE(status_calb_t);
QScriptValue toScriptValueM(QScriptEngine *engine, const status_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("MoveSts", s.MoveSts);
	obj.setProperty("MvCmdSts", s.MvCmdSts);
	obj.setProperty("PWRSts", s.PWRSts);
	obj.setProperty("EncSts", s.EncSts);
	obj.setProperty("WindSts", s.WindSts);
	obj.setProperty("CurPosition", s.CurPosition);
	obj.setProperty("uCurPosition", s.uCurPosition);
	obj.setProperty("EncPosition", (double)s.EncPosition);
	obj.setProperty("CurSpeed", s.CurSpeed);
	obj.setProperty("uCurSpeed", s.uCurSpeed);
	obj.setProperty("Ipwr", s.Ipwr);
	obj.setProperty("Upwr", s.Upwr/100.0);
	obj.setProperty("Iusb", s.Iusb);
	obj.setProperty("Uusb", s.Uusb/100.0);
	obj.setProperty("curT", s.CurT/10.0);
	obj.setProperty("Flags", s.Flags);
	obj.setProperty("GPIOFlags", s.GPIOFlags);
	obj.setProperty("CmdBufFreeSpace", s.CmdBufFreeSpace);
	return obj;
}

QScriptValue toScriptValueM(QScriptEngine *engine, const status_calb_t &s) {
	QScriptValue obj = engine->newObject();
	obj.setProperty("MoveSts", s.MoveSts);
	obj.setProperty("MvCmdSts", s.MvCmdSts);
	obj.setProperty("PWRSts", s.PWRSts);
	obj.setProperty("EncSts", s.EncSts);
	obj.setProperty("WindSts", s.WindSts);
	obj.setProperty("CurPosition", s.CurPosition);
	obj.setProperty("EncPosition", (double)s.EncPosition);
	obj.setProperty("CurSpeed", s.CurSpeed);
	obj.setProperty("Ipwr", s.Ipwr);
	obj.setProperty("Upwr", s.Upwr/100.0);
	obj.setProperty("Iusb", s.Iusb);
	obj.setProperty("Uusb", s.Uusb/100.0);
	obj.setProperty("curT", s.CurT/10.0);
	obj.setProperty("Flags", s.Flags);
	obj.setProperty("GPIOFlags", s.GPIOFlags);
	obj.setProperty("CmdBufFreeSpace", s.CmdBufFreeSpace);
	return obj;
}

void fromScriptValueM(const QScriptValue &obj, status_t &s) {
	Q_UNUSED(obj)
	Q_UNUSED(s)
}

void fromScriptValueM(const QScriptValue &obj, status_calb_t &s) {
	Q_UNUSED(obj)
	Q_UNUSED(s)
}

#include <qtscript-fromscript.c>
#include <qtscript-toscript.c>
#include <qtscript-fromscript_calb.c>
#include <qtscript-toscript_calb.c>

extern QObject* p_mainWnd;

static QScriptValue mwait(QScriptContext *context, QScriptEngine *engine)
{
	Q_UNUSED(engine)
	int ms = context->argument(0).toNumber();
	((ScriptThreadMulti*)p_scriptThread)->snore(ms);
	return QScriptValue::NullValue;
}

static QScriptValue new_axis(QScriptContext *context, QScriptEngine *engine)
{
	unsigned int sn = context->argument(0).toNumber();
	for (int i=0; i< di->size(); i++) {
		uint32_t serial;
		di->at(i)->get_serial_number(&serial);
		if (serial == sn) {
			AxisObject *o = new AxisObject(di->at(i));
			QObject::connect(o, SIGNAL(log_signal(QString)), self, SLOT(log_slot(QString)));
			return engine->newQObject(o, QScriptEngine::ScriptOwnership); // let the engine manage the new object's lifetime.
		}
	}
	return QScriptValue::NullValue;
}

static QScriptValue newaxis(QScriptContext *context, QScriptEngine *engine)
{
	QMetaObject::invokeMethod((Multiaxis*)p_mainWnd, "Log",
							Qt::QueuedConnection, Q_ARG(QString, QString("\"newaxis\" script function is deprecated, please use \"new_axis\" instead")), Q_ARG(QString, SOURCE_SCRIPT), Q_ARG(int, LOGLEVEL_WARNING));
	return new_axis(context, engine);
}

static QScriptValue get_next_serial(QScriptContext *context, QScriptEngine *engine)
{
	Q_UNUSED(engine)
	if (context->argumentCount() > 1)
		return QScriptValue::NullValue;
	unsigned int sn = context->argument(0).toNumber();
	QList<uint32_t> sorted_serials;
	for (int i=0; i< di->size(); i++) {
		uint32_t serial;
		di->at(i)->get_serial_number(&serial);
		sorted_serials.append(serial);
	}
	qSort(sorted_serials);
	for (int i=0; i < sorted_serials.length(); i++) {
		if (sorted_serials.at(i) > sn) {
			return sorted_serials.at(i);
		}
	}
	return 0;
}

static QScriptValue new_calibration(QScriptContext *context, QScriptEngine *engine)
{
	if (context->argumentCount() != 2)
		return QScriptValue::NullValue;
//	return toScriptValue(engine, calb); // replaced by synthetic function, because the proper include is in other place
	QScriptValue obj = engine->newObject();
	obj.setProperty("A", context->argument(0).toNumber());
	obj.setProperty("MicrostepMode", context->argument(1).toInteger());
	return obj;
}

static QScriptValue new_file(QScriptContext *context, QScriptEngine *engine)
{
	if (context->argumentCount() != 1)
		return QScriptValue::NullValue;
	return engine->newQObject(new FileObject(engine, context->argument(0).toString()), QScriptEngine::ScriptOwnership); // let the engine manage the new object's lifetime.
}

static QScriptValue script_log(QScriptContext *context, QScriptEngine *engine)
{
	Q_UNUSED(engine)
	int loglevel;
	if (context->argumentCount() > 1) {
		loglevel = context->argument(1).toInteger();
	} else {
		loglevel = LOGLEVEL_INFO;
	}
	QMetaObject::invokeMethod((Multiaxis*)p_mainWnd, "Log",
							Qt::QueuedConnection, Q_ARG(QString, context->argument(0).toString()), Q_ARG(QString, SOURCE_SCRIPT), Q_ARG(int, loglevel));
	return QScriptValue::NullValue;
}

ScriptThreadMulti::ScriptThreadMulti(QObject *parent, QList<DeviceInterface*> _devinterfaces)
{
	devinterfaces = _devinterfaces;
	di = &devinterfaces;
	self = this;
	_parent = parent;
	wait_for_exit = false;
	do_eval = false;

	qengine = new QScriptEngine(this);
	sAgent = new ScriptAgent( qengine );
	connect(sAgent, SIGNAL(positionChanged(int)),          ((Multiaxis*)_parent)->scriptDlg, SLOT(OnScriptExecPositionChanged(int)));
	connect(sAgent, SIGNAL(exceptionThrown(QScriptValue)), ((Multiaxis*)_parent)->scriptDlg, SLOT(OnExceptionThrown(QScriptValue)));
	qengine->setAgent( sAgent );

	qengine->globalObject().setProperty("get_next_serial", qengine->newFunction(get_next_serial));
	qengine->globalObject().setProperty("new_calibration", qengine->newFunction(new_calibration));
	qengine->globalObject().setProperty("new_axis", qengine->newFunction(new_axis));
	qengine->globalObject().setProperty("newaxis", qengine->newFunction(newaxis));
	qengine->globalObject().setProperty("msleep", qengine->newFunction(mwait));
	qengine->globalObject().setProperty("log",   qengine->newFunction(script_log));
	qengine->globalObject().setProperty("new_file", qengine->newFunction(new_file));
	codeEdit = ((Multiaxis*)_parent)->scriptDlg->findChild<QTextEdit*>("codeEdit");

	#include "qsdefine.c"
	qScriptRegisterMetaType<status_t>(qengine, toScriptValueM, fromScriptValueM);
	qScriptRegisterMetaType<status_calb_t>(qengine, toScriptValueM, fromScriptValueM);

	#include "qtscript-registermt.c"
}

ScriptThreadMulti::~ScriptThreadMulti()
{
	delete timer;
	delete sAgent;
	delete qengine;
}


void ScriptThreadMulti::user_update()
{
	if (do_eval && !qengine->isEvaluating()) {
		qengine->pushContext();
		qengine->evaluate(codeEdit->toPlainText());
		qengine->popContext();
		do_eval = false;
	}
}

void ScriptThreadMulti::log_slot(QString str)
{
	QMetaObject::invokeMethod((Multiaxis*)p_mainWnd, "Log",
							  Qt::QueuedConnection,
							  Q_ARG(QString, str),
							  Q_ARG(QString, SOURCE_SCRIPT),
							  Q_ARG(int, LOGLEVEL_WARNING));
}
