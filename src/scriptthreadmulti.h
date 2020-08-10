#ifndef SCRIPTTHREADMULTI_H
#define SCRIPTTHREADMULTI_H

#include <QThread>
#include <QPair>
#include <QTextEdit>
#include <QtScript>
#include <QScriptValue>
#include <scriptagent.h>
#include <deviceinterface.h>
#include <fileobject.h>
#include <basethread.h>

class ScriptThreadMulti: public BaseThread
{
	Q_OBJECT
public:
	ScriptThreadMulti(QObject *parent, QList<DeviceInterface*> _devinterface);
	~ScriptThreadMulti();
	
	bool do_eval;

	QScriptEngine *qengine;
	ScriptAgent *sAgent;

private:
	QList<DeviceInterface*> devinterfaces;
	QObject *_parent;
	QTimer *timer;
	int reconnect_time;
	QList <QPair<QObject*, QString> > objs;
	QString scriptcode;
	QTextEdit *codeEdit;

private slots:
	void user_update();
	void log_slot(QString);

};


#endif