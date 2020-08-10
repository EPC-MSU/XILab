#include <loggedfile.h>

LoggedFile::LoggedFile(QString filename) : QFile(filename) {}
LoggedFile::~LoggedFile() {}
bool LoggedFile::open(QIODevice::OpenMode flags)
{
	bool open_ok = QFile::open(flags);
	if (!open_ok)
		m_log->InsertLine(QDateTime::currentDateTime(), "Failed opening file " + fileName(), SOURCE_XILAB, LOGLEVEL_ERROR, m_ls);
	return open_ok;
}

void LoggedFile::setLog(MessageLog *log)
{
	m_log = log;
}

void LoggedFile::setLogSettings(LogSettings *ls)
{
	m_ls = ls;
}

MessageLog* LoggedFile::m_log = NULL;
LogSettings* LoggedFile::m_ls = NULL;
