#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QString>
#include <QDate>
#include <QDateTime>
#include <deviceinterface.h>
#include <vector>
#include <QSettings>
#include <QComboBox>
#include <QAbstractButton>
#include "version_struct.h"


#if defined(WIN32) || defined(WIN64)
#undef min
#undef max
#endif

#define pair(A, B)	QPair<const char*, unsigned int>(A, B)

#define SAVE_KEY(LIST, FLAG, MASK, KEY)\
	for (int i=0; i<LIST.length(); i++)\
		if ((FLAG & MASK) == LIST.at(i).second)\
			settings->setValue(KEY, LIST.at(i).first);\

#include <algorithm>

void msleep(int msec);

class TimeInterval
{
public:
	TimeInterval();
	void setCurrent();
	int secsElapsed();
	int msecsElapsed();
private:
	QDate date;
	QDateTime time;
};

QString toStr(signed int n, bool show_zero=false, int size=0);
QString toStr(unsigned int n, bool show_zero=false, int size=0);
QString toStr(signed long long n, bool show_zero=false, int size=0);
QString toStr(unsigned long long n, bool show_zero=false, int size=0);

QString toStr(double n, bool show_zero=false);
void setUnsetBit(bool condition, unsigned int* toset, unsigned int flag);
void Base16to256(QString text, std::vector<uint8_t>* key);
void Base256to16(std::vector<uint8_t> key, QString* text);

void LoadHelperSInt(QSettings *settings, QString *errors, const char *s, int *toset);
void LoadHelperUInt(QSettings *settings, QString *errors, const char *s, unsigned int *toset);
void LoadHelperFlag(QSettings *settings, QString *errors, const char *s, unsigned int *toset, unsigned int flag);
void LoadHelperMask(QSettings *settings, QString *errors, const char *s, unsigned int *toset, unsigned int mask);
void LoadHelperList(QSettings *settings, QString *errors, const char *s, unsigned int *toset, QList<QPair<const char *, unsigned int> > list);
void LoadHelperStr(QSettings *settings, QString *errors, const char *s, char *toset);
void LoadHelperFlt(QSettings *settings, QString *errors, const char *s, float *toset);
void ComboboxGrayout(QComboBox *box, int selected);

void sleep_act(int msec);

QString getFileName(QString filename);
QString getDefaultPath();
QString getOldDefaultPath();
QString getDefaultUserfilesInstallPath();
QString getDefaultScriptsPath();
QString getDefaultProfilesPath();
QString MakeProgramConfigFilename();
QString DefaultScriptScratchName();
QString DefaultConfigFilename();
QString DefaultLocalConfigFilename();
QString BindyKeyfileName();
QString DefaultBindyKeyfileName();
QString VirtualControllerBlobFilename(uint32_t serial);
QString TruncateDeviceName (const char* device_name);
void fixGrayout(QAbstractButton* btn);
void disableComboboxElement (QComboBox *combobox, int index, bool flag);
bool is_asc_order(version_t lower, version_t higher);
bool is_three_asc_order(version_t lower, version_t test, version_t higher);
void copyOrZero(unsigned int src, unsigned int* tgt, unsigned int ceiling);
void copyOrZero(unsigned int src, unsigned int* tgt);

bool operator==(const engine_settings_t& l, const engine_settings_t& r);
bool operator==(const entype_settings_t& l, const entype_settings_t& r);
bool operator==(const pid_settings_t& l, const pid_settings_t& r);
bool operator==(const emf_settings_t& l, const emf_settings_t& r);
bool operator==(const sync_in_settings_t& l, const sync_in_settings_t& r);
bool operator==(const sync_out_settings_t& l, const sync_out_settings_t& r);
bool operator==(const edges_settings_t& l, const edges_settings_t& r);
bool operator==(const secure_settings_t& l, const secure_settings_t& r);
bool operator==(const move_settings_t& l, const move_settings_t& r);
bool operator==(const feedback_settings_t& l, const feedback_settings_t& r);
bool operator==(const home_settings_t& l, const home_settings_t& r);
bool operator==(const power_settings_t& l, const power_settings_t& r);
bool operator==(const brake_settings_t& l, const brake_settings_t& r);
bool operator==(const control_settings_t& l, const control_settings_t& r);
bool operator==(const joystick_settings_t& l, const joystick_settings_t& r);
bool operator==(const ctp_settings_t& l, const ctp_settings_t& r);
bool operator==(const uart_settings_t& l, const uart_settings_t& r);
bool operator==(const extio_settings_t& l, const extio_settings_t& r);
bool operator==(const controller_name_t& l, const controller_name_t& r);

bool operator==(const stage_name_t& l, const stage_name_t& r);
bool operator==(const stage_information_t& l, const stage_information_t& r);
bool operator==(const stage_settings_t& l, const stage_settings_t& r);
bool operator==(const motor_information_t& l, const motor_information_t& r);
bool operator==(const motor_settings_t& l, const motor_settings_t& r);
bool operator==(const encoder_information_t& l, const encoder_information_t& r);
bool operator==(const encoder_settings_t& l, const encoder_settings_t& r);
bool operator==(const hallsensor_information_t& l, const hallsensor_information_t& r);
bool operator==(const hallsensor_settings_t& l, const hallsensor_settings_t& r);
bool operator==(const gear_information_t& l, const gear_information_t& r);
bool operator==(const gear_settings_t& l, const gear_settings_t& r);
bool operator==(const accessories_settings_t& l, const accessories_settings_t& r);

#define safe_copy(DST,SRC) {\
	strncpy(DST,SRC,sizeof DST - 1);\
	DST[sizeof DST - 1] = '\0';\
}

bool isSpeedUnitRotation(int feedbacktype);

#endif // FUNCTIONS_H
