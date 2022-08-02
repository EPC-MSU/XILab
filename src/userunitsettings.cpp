#include <userunitsettings.h>
#include <ximc.h>
#include <float.h>
#include <functions.h>

UserUnitSettings::UserUnitSettings(bool _enable, double _stepMult, double _unitMult, unsigned int _precision, QString _unitName, QString _correctionTable)
{
	enable = _enable;
	stepMult = _stepMult;
	unitMult = _unitMult;
	precision = _precision;
	unitName = _unitName;
	correctionTable = _correctionTable;
}

UserUnitSettings::UserUnitSettings()
{
	enable = false;
	stepMult = 1;
	unitMult = 1;
	precision = 3;
	unitName = QString("mm");
	correctionTable = QString("");
}

UserUnitSettings::~UserUnitSettings()
{
}

QString UserUnitSettings::getOffSuffix(UnitType unit_type, int fbtype)
{
	QString s;
	if (isSpeedUnitRotation(fbtype)) {
		switch (unit_type) {
		case TYPE_COORD: s = "counts"; break;
		case TYPE_SPEED: s = "rpm"; break;
		case TYPE_ACCEL: s = "rpm/s"; break;
		case TYPE_UNKNOWN:
		default: s = "unknown dc unit";
		}
	}
	else
	{
		switch (unit_type) {
			case TYPE_COORD: s = "steps"; break;
			case TYPE_SPEED: s = "steps/s"; break;
			case TYPE_ACCEL: s = "steps/s²"; break;
			case TYPE_UNKNOWN:
			default: s = "unknown stepper unit";
		}
	}

	return " " + s;
}

QString UserUnitSettings::getOnSuffix(UnitType unit_type, int fbtype)
{
	Q_UNUSED(fbtype)
	QString s;

	switch (unit_type) {
		case TYPE_COORD: s = unitName; break;
		case TYPE_SPEED: s = unitName + "/s"; break;
		case TYPE_ACCEL: s = unitName + "/s²"; break;
		case TYPE_UNKNOWN:
		default: s = "unknown user unit";
	}

	return " " + s;
}

QString UserUnitSettings::getSuffix(UnitType unit_type, int fbtype)
{
	if (enable)
		return getOnSuffix(unit_type, fbtype);
	else
		return getOffSuffix(unit_type, fbtype);
}

double UserUnitSettings::getUnitPerStep()
{
	double a;
	if (stepMult == 0) {
		a = FLT_MAX;
	} else if (unitMult == 0) {
		a = FLT_MIN;
	} else {
		a = unitMult / stepMult;
	}
	return a;
}