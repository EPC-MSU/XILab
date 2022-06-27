#ifndef USERUNITSETTINGS_H
#define USERUNITSETTINGS_H

#include <QString>

class UserUnitSettings {
public:
	enum UnitType{TYPE_COORD, TYPE_SPEED, TYPE_ACCEL, TYPE_UNKNOWN};
	UserUnitSettings(bool _enable, double _stepMult, double _unitMult, unsigned int _precision, QString _unitName, QString _correctionTable);
	UserUnitSettings();
    ~UserUnitSettings();
	QString getOffSuffix(UnitType unit_type, int fbtype);
	QString getOnSuffix(UnitType unit_type, int fbtype);
	QString getSuffix(UnitType unit_type, int fbtype);
	double getUnitPerStep();

public:
	bool enable;
	unsigned int precision;
	QString unitName;
	QString correctionTable;
	
	unsigned int messageType;
	QString messageText;
	unsigned int messageActiom;

private:
	double stepMult;
	double unitMult;

friend class PageUserUnitsWgt;
};

#endif // USERUNITSETTINGS_H
