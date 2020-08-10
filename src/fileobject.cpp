#include "fileobject.h"
#include <QDebug>
#include <QScriptValueIterator>

FileObject::FileObject(QScriptEngine* _engine, QString filename)
{
	engine = _engine;
	file.setFileName(filename);
}

bool FileObject::open()
{
	bool ok = file.open(QIODevice::ReadWrite | QIODevice::Text);
	if (!ok)
		file.open(QIODevice::ReadOnly | QIODevice::Text);
	return ok;
}

void FileObject::close()
{
	file.close();
}

qint64 FileObject::size()
{
	return file.size();
}

bool FileObject::seek(qint64 pos)
{
	return file.seek(pos);
}

bool FileObject::resize(qint64 size)
{
	return file.resize(size);
}

bool FileObject::remove()
{
	return file.remove();
}
/*
qint64 FileObject::read(QScriptValue v, qint64 maxsize)
{
	if (!file.isOpen())
		return -1;

	char* ptr = (char*)malloc(maxsize);
	qint64 readsize = file.read(ptr, maxsize);

	QByteArray qba(ptr, readsize);
	for (qint64 i=0; i<readsize; i++) {
		v.setProperty(i, qba.at(i));
	}
	delete ptr;
	return readsize;
}

qint64 FileObject::write(QScriptValue v, qint64 maxsize)
{
	if (!file.isOpen())
		return -1;

	qint64 length = v.property("length").toInteger();
	QByteArray qba;
	for (qint64 i=0; i<maxsize; i++)
		qba.append( (char)v.property(i).toInteger() );
	return file.write(qba.data());
}
*/
QScriptValue FileObject::read(qint64 maxsize)
{
	if (!file.isOpen())
		return -1;

	char* ptr = new char[maxsize];
	qint64 readsize = file.readLine(ptr, maxsize);
	QByteArray qba(ptr, readsize);
	delete[] ptr;
	return QString::fromUtf8(qba.constData());
}

qint64 FileObject::write(QScriptValue v, qint64 maxsize)
{
	return file.write(v.toString().toUtf8().constData(), maxsize);
}

qint64 FileObject::write(QScriptValue v)
{
	return file.write(v.toString().toUtf8().constData());
}
