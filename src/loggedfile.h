#include <messagelog.h>
#include <logsettings.h>
#include <QFile>

class LoggedFile : public QFile
{
public:
    LoggedFile(QString filename);
    ~LoggedFile();

	bool open(QIODevice::OpenMode flags);
	void setLog(MessageLog *log);
	void setLogSettings(LogSettings *ls);

private:
	static MessageLog* m_log;
	static LogSettings* m_ls;
};