#ifndef SCRIPTAGENT_H
#define SCRIPTAGENT_H

#include <QtCore/QObject>
#include <QtScript>
#include <QScriptEngineAgent>
#include <QTimer>

class ScriptAgent : public QObject, public QScriptEngineAgent
{
	Q_OBJECT

public:
	ScriptAgent(QScriptEngine *engine);
	~ScriptAgent();

private:
	QTimer *timer;
	int line, prev_line;

private slots:
	void update();

signals:
	void positionChanged(int lineNum);
	void exceptionThrown(QScriptValue exception);

protected:
	void positionChange(qint64 scriptId, int lineNumber, int columnNumber);
	void exceptionThrow(qint64 scriptId, const QScriptValue & exception, bool hasHandler);
};

#endif // SCRIPTAGENT_H
