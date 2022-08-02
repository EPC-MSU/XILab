#include <scriptthread.h>
#include <mainwindow.h>

QObject* p_scriptThread;
extern QObject* p_mainWnd;
AxisObject *oneaxis;

Q_DECLARE_METATYPE(status_t);
Q_DECLARE_METATYPE(status_calb_t);

QScriptValue toScriptValue(QScriptEngine *engine, const status_t &s) {
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

QScriptValue toScriptValue(QScriptEngine *engine, const status_calb_t &s) {
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

#include <qtscript-fromscript.c>
#include <qtscript-toscript.c>
#include <qtscript-fromscript_calb.c>
#include <qtscript-toscript_calb.c>

static QScriptValue mwait(QScriptContext *context, QScriptEngine *engine)
{
	Q_UNUSED(engine)
	int ms = context->argument(0).toNumber();
	((ScriptThread*)p_scriptThread)->snore(ms);
	return QScriptValue::NullValue;
}

static QScriptValue new_axis(QScriptContext *context, QScriptEngine *engine)
{
	unsigned int sn = context->argument(0).toNumber();
	uint32_t serial;
	devinterface->get_serial_number(&serial);
	if (serial == sn) {
		AxisObject *o = new AxisObject(devinterface);
		QObject::connect(o, SIGNAL(log_signal(QString)), p_scriptThread, SLOT(log_slot(QString)));
		return engine->newQObject(o, QScriptEngine::ScriptOwnership); // let the engine manage the new object's lifetime.
	}
	return QScriptValue::NullValue;
}

static QScriptValue newaxis(QScriptContext *context, QScriptEngine *engine)
{
	QMetaObject::invokeMethod((MainWindow*)p_mainWnd, "Log",
							Qt::QueuedConnection, Q_ARG(QString, QString("\"newaxis\" script function is deprecated, please use \"new_axis\" instead")), Q_ARG(QString, SOURCE_SCRIPT), Q_ARG(int, LOGLEVEL_WARNING));
	return new_axis(context, engine);
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
	QMetaObject::invokeMethod((MainWindow*)p_mainWnd, "Log",
							Qt::QueuedConnection, Q_ARG(QString, context->argument(0).toString()), Q_ARG(QString, SOURCE_SCRIPT), Q_ARG(int, loglevel));
	return QScriptValue::NullValue;
}

static QScriptValue get_next_serial(QScriptContext *context, QScriptEngine *engine)
{
	Q_UNUSED(engine)
	if (context->argumentCount() > 1)
		return QScriptValue::NullValue;
	unsigned int sn = context->argument(0).toNumber();
	uint32_t serial;
	devinterface->get_serial_number(&serial);
	if (serial > sn)
		return serial;
	else
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

ScriptThread::ScriptThread(QObject *_parent, DeviceInterface *_devinterface)
{
	devinterface = _devinterface;
	wait_for_exit = false;
	do_eval = false;
	p_scriptThread = this;

	qengine = new QScriptEngine(this);
	sAgent = new ScriptAgent( qengine );
	connect(sAgent, SIGNAL(positionChanged(int)),          ((MainWindow*)_parent)->scriptDlg, SLOT(OnScriptExecPositionChanged(int)));
	connect(sAgent, SIGNAL(exceptionThrown(QScriptValue)), ((MainWindow*)_parent)->scriptDlg, SLOT(OnExceptionThrown(QScriptValue)));
	qengine->setAgent( sAgent );

	oneaxis = new AxisObject(devinterface);
	connect(oneaxis, SIGNAL(log_signal(QString)), this, SLOT(log_slot(QString)));
	QScriptValue qaxis = qengine->newQObject(oneaxis);

	//Extraction option 1: from QObject
	/*
	int metc = oneaxis->metaObject()->methodCount();
	char str[256];
	for (int i=0; i < metc; i++) {
		sscanf(oneaxis->metaObject()->method(i).signature(), "%255[^(]", str);
		if (strcmp(str, "destroyed") != 0 && strcmp(str, "deleteLater") != 0 && strcmp(str, "_q_reregisterTimers") != 0)
			qengine->globalObject().setProperty(str, qaxis.property(str));
	}
	*/
	// Extraction option 2: from QScriptValue
	QScriptValueIterator it(qaxis);
	char str[256];
	while (it.hasNext()) {
		it.next();
		sscanf(it.name().toLocal8Bit().data(), "%255[^(]", str);
		qengine->globalObject().setProperty(str, qaxis.property(str));
	}

	qengine->globalObject().setProperty("get_next_serial", qengine->newFunction(get_next_serial));
	qengine->globalObject().setProperty("new_calibration", qengine->newFunction(new_calibration));
	qengine->globalObject().setProperty("new_axis", qengine->newFunction(new_axis));
	qengine->globalObject().setProperty("newaxis", qengine->newFunction(newaxis));
	qengine->globalObject().setProperty("msleep", qengine->newFunction(mwait));
	qengine->globalObject().setProperty("log",   qengine->newFunction(script_log));
	qengine->globalObject().setProperty("new_file", qengine->newFunction(new_file));

	#include "qsdefine.c"

	qScriptRegisterMetaType<status_t>(qengine, toScriptValue, fromScriptValue);
	qScriptRegisterMetaType<status_calb_t>(qengine, toScriptValue, fromScriptValue);
	#include "qtscript-registermt.c"

	codeEdit = ((MainWindow*)_parent)->scriptDlg->findChild<QTextEdit*>("codeEdit");
}

ScriptThread::~ScriptThread()
{
	delete timer;
	p_scriptThread = NULL;
	p_mainWnd = NULL;
	disconnect(oneaxis);
	delete oneaxis;
}

void ScriptThread::user_update()
{
	if (do_eval && !qengine->isEvaluating()) {
		qengine->pushContext();
		qengine->evaluate(codeEdit->toPlainText());
		qengine->popContext();
		do_eval = false;
	}
}

void ScriptThread::log_slot(QString str)
{
	QMetaObject::invokeMethod((MainWindow*)p_mainWnd, "Log",
							  Qt::QueuedConnection,
							  Q_ARG(QString, str),
							  Q_ARG(QString, SOURCE_SCRIPT),
							  Q_ARG(int, LOGLEVEL_WARNING));
}
