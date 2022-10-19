#if defined(WIN32) || defined(WIN64)
#include <windows.h>
void msleep(int msec) { Sleep( msec ); }
#else
#include <stdint.h>
#include <unistd.h>
void msleep(int msec) { usleep( msec*1000 ); }
#endif

#include <QApplication>
#include <QString>
#include <functions.h>
#include <cmath>
#include <stdlib.h>
#include <QChar>
#include <QStandardItemModel>

QString knf = " key not found\n";
QString kup = " key unknown parameter\n";

#define TOSTR {\
/*	if(n == 0) return "0";*/\
	QString s;\
	s.setNum(n);\
	int diff = size - s.length();\
	if(diff<0) return s;\
	QString zero = "";\
	for(int i=0; i<diff; i++) zero += " ";\
	return zero + s;\
}

TimeInterval::TimeInterval()
{
	setCurrent();
}
void TimeInterval::setCurrent()
{
	time = QDateTime::currentDateTimeUtc();
}
int TimeInterval::secsElapsed()
{
	return time.secsTo(QDateTime::currentDateTimeUtc());
}
int TimeInterval::msecsElapsed()
{
	return time.msecsTo(QDateTime::currentDateTimeUtc());
}

QString toStr(signed int n, bool show_zero, int size) { TOSTR Q_UNUSED(show_zero) }
QString toStr(unsigned int n, bool show_zero, int size) { TOSTR Q_UNUSED(show_zero) }
QString toStr(signed long long n, bool show_zero, int size) { TOSTR Q_UNUSED(show_zero) }
QString toStr(unsigned long long n, bool show_zero, int size) { TOSTR Q_UNUSED(show_zero) }
QString toStr(double n, bool show_zero)
{
	if((n == 0.0) && show_zero) return "0.0";

	QString s;
	s.setNum(n);
	if(((n - floor(n)) == 0) && show_zero) s+=".0";
	return s;
}

#ifdef _MSC_VER
#pragma warning ( push )
#pragma warning ( disable : 4146 )
#endif
void setUnsetBit(bool condition, unsigned int* toset, unsigned int flag)
{
	Q_ASSERT( flag == (flag & -flag) ); // bombs if our "flag" has more than 1 bit set
	if (condition)
		*toset |= flag;
	else
		*toset &= ~flag;
}

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
#endif
void Base16to256(QString text, std::vector<uint8_t>* key) // assumes vector size is the length of data we read from the string
{
	for (int i=0; i<(int)key->size(); i++) {
		key->at(i) = (text.mid(2*i,1).toInt(NULL,16) << 4) + (text.mid(2*i+1,1).toInt(NULL,16));
	}
}

void Base256to16(std::vector<uint8_t> key, QString* text) // assumes we want to write entire vector into the string
{
	text->clear();
	for (int i=0; i<(int)key.size(); i++) {
		uint8_t byte = key.at(i);
		text->replace(2*i, 1, QString::number(byte >> 4, 16).toUpper());
		text->replace(2*i+1, 1, QString::number(byte % 16, 16).toUpper());
	}
}
#ifdef _MSC_VER
#pragma warning ( pop )
#endif

void LoadHelperSInt(QSettings *settings, QString *errors, const char *s, int *toset)
{
	if (settings->contains(s))
		*toset = settings->value(s).toInt();
	else
		errors->append(s + knf);
}

void LoadHelperUInt(QSettings *settings, QString *errors, const char *s, unsigned int *toset)
{
	if (settings->contains(s))
		*toset = settings->value(s).toUInt();
	else
		errors->append(s + knf);
}

/*
  If key "s" is true, then sets the bit "flag" in "toset" variable,
  if key "s" is false, then clears the bit "flag" in "toset" variable.
  If key "s" doesn't exist, then appends error message to the QString pointed to by "errors" variable.
  You might want to select a setting group with settings->beginGroup first.
*/
#ifdef _MSC_VER
#pragma warning ( push )
#pragma warning ( disable : 4146 )
#endif
void LoadHelperFlag(QSettings *settings, QString *errors, const char *s, unsigned int *toset, unsigned int flag)
{
	Q_ASSERT( flag == (flag & -flag) ); // bombs if our "flag" has more than 1 bit set
	if (settings->contains(s)) {
		if (settings->value(s).toBool())
			*toset |= flag;
		else
			*toset &= ~flag;
	} else
		errors->append(s + knf);
}

/*
  Takes key "s" as unsigned int, applies a bitmask "mask" to it and sets/clears mask bits in "toset" variable.
  If key "s" doesn't exist, then appends error message to the QString pointed to by "errors" variable.
  You might want to select a setting group with settings->beginGroup first.
*/
void LoadHelperMask(QSettings *settings, QString *errors, const char *s, unsigned int *toset, unsigned int mask)
{
	Q_ASSERT( mask != (mask & -mask) ); // mask must have more than 1 bit set
	if(settings->contains(s)) {
		*toset &= ~mask;
		*toset |= (mask & settings->value(s).toUInt());
	}
	else
		errors->append(s+knf);
}
#ifdef _MSC_VER
#pragma warning ( pop )
#endif
/*
  Takes key "s" as a string, and searches for matching value in the first part of the pairs in the list "list".
  If a match is found, then assignes value of the second part of the found pair to the "toset" variable.
  If key "s" doesn't exist, then appends error message to the QString pointed to by "errors" variable.
  You might want to select a setting group with settings->beginGroup first.
*/
void LoadHelperList(QSettings *settings, QString *errors, const char *s, unsigned int *toset, QList<QPair<const char *, unsigned int> > list)
{
	QByteArray qb;
	const char* val;
	bool found = false;
	if (settings->contains(s)) {
		qb = settings->value(s).toString().toLocal8Bit();
		val = qb.data();
		for (int i = 0; i < list.size(); ++i) {
			if (strcmp(list.at(i).first, val) == 0) {
				*toset = list.at(i).second;
				found = true;
				break;
			}
		}
		if (!found)
			errors->append(s+kup);
	} else 
		errors->append(s+knf);
}

/*
  Loads a string, identified by key "s", into the char array "toset".
  If key "s" doesn't exist, then appends error message to the QString pointed to by "errors" variable.
  As it is with previous helper functions, select a setting group with settings->beginGroup first.
*/
void LoadHelperStr(QSettings *settings, QString *errors, const char *s, char *toset)
{
	if (settings->contains(s))
		strcpy(toset, settings->value(s).toByteArray().data());
	else
		errors->append(s + knf);
}

/*
  Loads a floating point number, identified by key "s", into the float "toset".
  Float is stored as a string.
  If key "s" doesn't exist, then appends error message to the QString pointed to by "errors" variable.
  As it is with previous helper functions, select a setting group with settings->beginGroup first.
*/
void LoadHelperFlt(QSettings *settings, QString *errors, const char *s, float *toset)
{
	if (settings->contains(s))
		*toset = settings->value(s).toFloat();
	else
		errors->append(s + knf);
}

/*
  Loops through the combobox items, setting background of "selected" item to color white and background of the rest of the items to color black.
*/
void ComboboxGrayout(QComboBox *box, int selected)
{
	for (int i=0; i < box->count(); i++)
		if (i == selected)
			box->setItemData( i, QColor( Qt::white ), Qt::BackgroundRole );
		else
			box->setItemData( i, QColor( Qt::lightGray ), Qt::BackgroundRole );
}

void sleep_act(int msec)
{
	if(msec == 0){
		QApplication::processEvents();
		return;
	}

	TimeInterval t1;
	while(t1.msecsElapsed() < msec){
		QApplication::processEvents();
	}
}

QString getFileName(QString filename)
{
	QString ret;
	int start = qMax(filename.lastIndexOf("/"), filename.lastIndexOf("\\"));
	if((start>-1) && (start <=filename.length()))
		return filename.right(filename.length() - start - 1);
	else
		return filename;
}

QString getDefaultPath()
{
#if defined(WIN32) || defined(WIN64)
	//output format: %appdata%/XILab
	return QString::fromLocal8Bit(getenv("APPDATA"))+ "\\XILab.conf";
#endif
#ifdef __LINUX__
	//output format: $HOME/.XILab/
	return QString::fromLocal8Bit(getenv("HOME")) + "/.config/XILab.conf";
#endif
#ifdef __APPLE__
	//output format: $HOME/Library/Application Support/XiLab (see #5749-7)
	return QString::fromLocal8Bit(getenv("HOME")) + "/Library/Application Support/XILab.conf";
#endif
}

QString getOldDefaultPath()
{
#if defined(WIN32) || defined(WIN64)
	//output format: %appdata%/XILab
	return QString::fromLocal8Bit(getenv("APPDATA")) + "\\XILab.conf";
#endif
#ifdef __LINUX__
	//output format: $HOME/.XILab/
	return QString::fromLocal8Bit(getenv("HOME")) + "/.config/XILab.conf";
#endif
#ifdef __APPLE__
	//output format: $HOME/XiLab
	return QString::fromLocal8Bit(getenv("HOME")) + "/XILab.conf";
#endif
}

QString MakeProgramConfigFilename()
{
#if defined(WIN32) || defined(WIN64)
	//output format: %appdata%\XILab.conf\settings.ini
        return getDefaultPath() + "\\settings.ini";
#endif
#ifdef __LINUX__
	//output format: $HOME/XILab.conf/settings.ini
        return getDefaultPath() + "/settings.ini";
#endif
#ifdef __APPLE__
	//output format: $HOME/XILab.conf/settings.ini
        return getDefaultPath() + "/settings.ini";
#endif
}

QString DefaultScriptScratchName()
{
	return getDefaultPath() + "/scratch.txt"; // since both windows and unix-types will process this kind of slash
}

QString BindyKeyfileName()
{
	return getDefaultPath() + "/keyfile.sqlite";
}

QString VirtualControllerBlobFilename(uint32_t serial)
{
	return QString("%1/%2%3").arg(getDefaultPath()).arg("V_").arg(serial);
}

QString DefaultConfigFilename()
{
#if defined(WIN32) || defined(WIN64)
	return QApplication::applicationDirPath() + "/xilabdefault.cfg";
#endif
#ifdef __LINUX__
	return "/usr/share/xilab/xilabdefault.cfg";
#endif
#ifdef __APPLE__
	return QApplication::applicationDirPath() + "/xilabdefault.cfg";
#endif
}

QString DefaultBindyKeyfileName()
{
	const char* fn = "/default_keyfile.sqlite";
#if defined(WIN32) || defined(WIN64)
	return QApplication::applicationDirPath() + fn;
#endif
#ifdef __LINUX__
	return QString("/usr/share/xilab") + fn;
#endif
#ifdef __APPLE__
	return QApplication::applicationDirPath() + fn;
#endif
}

QString DefaultLocalConfigFilename()
{
	return QApplication::applicationDirPath() + "/xilabdefault.cfg";
}

QString getDefaultUserfilesInstallPath()
{
#if defined(WIN32) || defined(WIN64)
	return QCoreApplication::applicationDirPath();
#endif
#ifdef __LINUX__
	return "/usr/share/xilab/";
#endif
#ifdef __APPLE__
	return "/Library/XILab/";
#endif
}

QString getDefaultScriptsPath()
{
	return getDefaultUserfilesInstallPath() + "/scripts/";
}

QString getDefaultProfilesPath()
{
	return getDefaultUserfilesInstallPath() + "/profiles/";
}

QString TruncateDeviceName (const char* device_name)
{
	return QString(device_name).replace(QRegExp("xi-com:(\\\\\\\\.\\\\|)"), "");
}

/*
  Fixes initial grayout problem of all signal-slot linked widgets by toggling source widgets.
  Double toggle preserves their state (assuming no tri-state checkboxes).
*/
void fixGrayout (QAbstractButton* trigger)
{
	trigger->toggle();
	trigger->toggle();
}

void disableComboboxElement (QComboBox *combobox, int index, bool flag)
{
	const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(combobox->model());
	QStandardItem* item = model->item(index);

	item->setFlags(flag ? item->flags() & ~(Qt::ItemIsSelectable|Qt::ItemIsEnabled) : (Qt::ItemIsSelectable|Qt::ItemIsEnabled));
	
	// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
	// QVaraiant() clears item data in order to use default color
	item->setData(flag ? combobox->palette().color(QPalette::Disabled, QPalette::Text) : QVariant(), Qt::TextColorRole);
}

/*
  Performs comparison on three versions as if they were three-digit arbitrary-radix numbers.
  Returns true if middle one is less than or equals higher version and is greater than or equals lower version.
*/
bool is_three_asc_order(version_t lower, version_t test, version_t higher)
{
	if (is_asc_order(lower,test) && is_asc_order(test,higher))
		return true;
	else
		return false;
}

/*
  Performs comparison on two versions as if they were three-digit arbitrary-radix numbers.
  Returns true if lower version is less than or equals higher version.
*/
bool is_asc_order(version_t lower, version_t higher)
{
	if (lower.major > higher.major)
		return false;
	else if (lower.major == higher.major && lower.minor > higher.minor)
		return false;
	else if (lower.major == higher.major && lower.minor == higher.minor && lower.release > higher.release)
		return false;

	return true;
}

/*
  If "src" value is equal to or larger than "ceiling", then set "tgt" to zero, else copy "src" into the "tgt".
*/
void copyOrZero(unsigned int src, unsigned int* tgt, unsigned int ceiling)
{
	if (src >= ceiling) {
		*tgt = 0;
	} else {
		*tgt = src;
	}
}

void copyOrZero(unsigned int src, unsigned int* tgt)
{
	copyOrZero(src, tgt, (1 << 16));
}

bool isSpeedUnitRotation(int feedbacktype)
{
	switch (feedbacktype) {
	case FEEDBACK_ENCODER:
	case FEEDBACK_ENCODER_MEDIATED:
		return true;
	}
	return false;
}

#include "qtscript-comparison.c"
