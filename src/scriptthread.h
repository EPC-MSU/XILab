#ifndef SCRIPTTHREAD_H
#define SCRIPTTHREAD_H

#include <QThread>
#include <QPair>
#include <QTextEdit>
#include <QtScript>
#include <QScriptValue>
#include <scriptagent.h>
#include <deviceinterface.h>
#include <axisobject.h>
#include <fileobject.h>
#include <basethread.h>

extern QObject* p_scriptThread;

class ScriptThread: public BaseThread
{
	Q_OBJECT
public:
	ScriptThread(QObject *parent, DeviceInterface *_devinterface);
	~ScriptThread();
	
	bool do_eval;
	QScriptEngine *qengine;
	ScriptAgent *sAgent;

private:
	DeviceInterface *devinterface;
	QObject *_parent;
	int reconnect_time;
	QList <QPair<QObject*, QString> > objs;
	QString scriptcode;
	QTextEdit *codeEdit;

private slots:
	void user_update();
	void log_slot(QString);

};


#endif