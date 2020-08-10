#ifndef STAGESETTINGS_H
#define STAGESETTINGS_H

#include <deviceinterface.h>
#include <ximc.h>
#include <QSettings>

class StageSettings {
private:
	DeviceInterface *devinterface;

public:
	StageSettings(DeviceInterface *_devinterface);
    ~StageSettings();

    void FromDeviceToClass();
    bool FromClassToDevice();

	void FromSettingsToClass(QSettings *settings, QString *errors);
	bool FromClassToSettings(QSettings *settings);

	stage_name_t stage_name;
	stage_information_t stage_information;
	stage_settings_t stage_settings;
	motor_information_t motor_information;
	motor_settings_t motor_settings;
	encoder_information_t encoder_information;
	encoder_settings_t encoder_settings;
	hallsensor_information_t hallsensor_information;
	hallsensor_settings_t hallsensor_settings;
	gear_information_t gear_information;
	gear_settings_t gear_settings;
	accessories_settings_t accessories_settings;

	bool need_stage_name_save;
	bool need_stage_information_save;
	bool need_stage_settings_save;
	bool need_motor_information_save;
	bool need_motor_settings_save;
	bool need_encoder_information_save;
	bool need_encoder_settings_save;
	bool need_hallsensor_information_save;
	bool need_hallsensor_settings_save;
	bool need_gear_information_save;
	bool need_gear_settings_save;
	bool need_accessories_settings_save;

	bool need_load;

	QList<QPair<const char*, unsigned int> > motorTypeList, tsTypeList;
};

#endif // STAGESETTINGS_H
