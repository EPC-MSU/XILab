#ifndef MESSAGELOG_H
#define MESSAGELOG_H

#include <deviceinterface.h>
#include <QDateTime>
#include <QObject>
#include <QTableWidget>
#include <logsettings.h>
#include <QFile>
#include <QTimer>

#define SOURCE_XILAB	"XiLab"
#define SOURCE_SCRIPT	"Script"
#define SOURCE_LIBRARY	"libximc"

typedef struct LogItem{
	QDateTime datetime;
	QString message;
	QString source;
	int loglevel;
} LogItem;

class MessageLog: public QObject
{
	Q_OBJECT
public:
	MessageLog(void);
	~MessageLog(void);

	void Clear();
	void CloseLogFile();
	void setSerial(unsigned int);

	int size;
	bool pop(LogItem*);

private:
	int cnt;
	unsigned int sn;
	QDateTime *basetime;
	QString serial;
	QDir path, old_path;
	QList<LogItem> loglist;
	QString default_save_path;
	QTimer *timer;
	QTime *updateTimer;
	QFile *autofile;
	QDate old_date;
	QStringList buffer;
	QMutex mutex;

	QString formatFilename(QDir, QDateTime);
	bool ReOpenLogFile(QDir, QDateTime);

private slots:
	void update();

public slots:
	void InsertLine(QDateTime datetime, QString message, QString source, int loglevel, LogSettings *ls);

};

#endif // MESSAGELOG_H
