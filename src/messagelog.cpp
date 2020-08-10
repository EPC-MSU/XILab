#include <messagelog.h>
#include <QHeaderView>

#include <functions.h>
#include <QFileDialog>
#include <QDateTime>
#include <QScrollBar>

MessageLog::MessageLog()
{
	sn = 0;
	autofile = NULL;
	path = QDir(getDefaultPath());
	old_path = QDir("");

	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(5000);

	QDateTime now = QDateTime::currentDateTime();
	old_date = now.date();

	ReOpenLogFile(path, QDateTime::currentDateTime());
}

MessageLog::~MessageLog(void)
{
	update();
}

void MessageLog::setSerial(unsigned int _sn)
{
	this->sn = _sn;
	serial = toStr(sn);
}

void MessageLog::update()
{
	if (autofile != NULL) {
		autofile->flush();
	}
}

QString MessageLog::formatFilename(QDir basepath, QDateTime datetime)
{
	QString serial_str;
	serial = toStr(sn);
	if (sn == 0)
		serial_str = QString("");
	else
		serial_str = QString("_SN" + serial);
	return basepath.absolutePath() + "/xilab_log_" + datetime.toString("yyyy.MM.dd") + serial_str + ".csv";
}

bool MessageLog::pop(LogItem* item)
{
	QMutexLocker locker(&mutex);
	if (!loglist.isEmpty()) {
		LogItem first = loglist.takeFirst();
		item->datetime = first.datetime;
		item->message = first.message;
		item->source = first.source;
		item->loglevel = first.loglevel;
		return true;
	} else
		return false;
}

void MessageLog::InsertLine(QDateTime datetime, QString message, QString source, int loglevel, LogSettings *ls)
{
	QMutexLocker locker(&mutex);
	QString date = datetime.toString("yyyy.MM.dd");
	QString time = datetime.toString("hh:mm:ss.zzz");
	QString str = QString("\"" + date + "\","
						+ "\"" + time + "\","
						+ "\"" + source + "\","
						+ "\"" + message + "\"\r\n");
	LogItem a;
	a.datetime = datetime;
	a.message = message;
	a.source = source;
	a.loglevel = loglevel;
	loglist.append(a);

	buffer << str;
	if (ls != NULL) {
		// Save to file
		if (ls->autosave_enabled) {
			path = ls->autosave_path;
			if ((datetime.date() != old_date) | (path != old_path && path.exists())) {
				if (ReOpenLogFile(path, datetime)) {
					old_path = path;
					old_date = datetime.date();
				}
			}
			if (autofile != NULL) {
				autofile->write( buffer.join("").toUtf8().data() );
				buffer.clear();
			}
		}
	} else {
		if (autofile != NULL) { // (ls == NULL and autofile != NULL) means we're exiting with autosave on or starting
			autofile->write( buffer.join("").toUtf8().data() );
			buffer.clear();
		}
	}
}

void MessageLog::Clear()
{
	QMutexLocker locker(&mutex);
	loglist.clear();
}

bool MessageLog::ReOpenLogFile(QDir path, QDateTime datetime)
{
	CloseLogFile();
	autofile = new QFile(formatFilename(path, datetime));

	if (autofile->open(QIODevice::Append)) {
		return true;
	} else {
		delete autofile;
		autofile = NULL;
		return false;
	}
	return true;
}


void MessageLog::CloseLogFile()
{
	if (autofile != NULL) {
		autofile->flush();
		autofile->close();
		delete autofile;
		autofile = NULL;
	}
}