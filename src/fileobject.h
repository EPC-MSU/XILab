#ifndef FILEOBJECT_H
#define FILEOBJECT_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QScriptValue>
#include <QScriptEngine>
#include <QVariant>

class FileObject : public QFile
{
    Q_OBJECT

public:
    FileObject(QScriptEngine* _engine, QString filename);

private:
	QFile file;
	QScriptEngine *engine;

public slots:
	bool open();
	void close();
	qint64 size();
	bool seek(qint64 pos);
	bool resize(qint64 size);
	bool remove();
//	qint64 read(QScriptValue v, qint64 maxsize);
//	qint64 write(QScriptValue v, qint64 maxsize);
	QScriptValue read(qint64 maxsize);
	qint64 write(QScriptValue v, qint64 maxsize);
	qint64 write(QScriptValue v);
};

#endif
