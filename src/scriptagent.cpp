#include <scriptagent.h>
const int LINE_REFRESH_MS = 20;

ScriptAgent::ScriptAgent (QScriptEngine *engine) : QScriptEngineAgent(engine)
{
	prev_line = -1;
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(LINE_REFRESH_MS);
}

ScriptAgent::~ScriptAgent()
{
	disconnect(this, SLOT(update()));
}

void ScriptAgent::positionChange (qint64 scriptId, int lineNumber, int columnNumber) {
	elimination_warnings = scriptId && columnNumber; //Для устранения предупреждений
	line = lineNumber;
}

void ScriptAgent::exceptionThrow (qint64 scriptId, const QScriptValue & exception, bool hasHandler) { 
	elimination_warnings = scriptId && hasHandler; //Для устранения предупреждений
	emit exceptionThrown(exception);
}

void ScriptAgent::update() {
	if (prev_line == line)
		return;

	emit positionChanged(line);
	prev_line = line;
}