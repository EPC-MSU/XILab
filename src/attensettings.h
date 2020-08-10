#ifndef ATTENSETTINGS_H
#define ATTENSETTINGS_H

#include <QLineEdit>

#define FILTER_COUNT 8//[1 .. 8] - range for first wheel ; [9 .. 16 ] - range for second wheel
#define WHEEL_COUNT 2

enum SkinType{
	GeneralSkin = 1,
	AttenuatorSkin = 2
};

namespace Calibration {
	typedef enum {
		NO_CALIBRATION = 1,
		IN_PROGRESS = 2,
		OK = 3
	} Status;
}

class AttenSettings {
public:
    AttenSettings();
    ~AttenSettings();
    SkinType skinType;
    int selected_wavelength;//three type of wavelength,default:1-1055 nm,2-532 nm,3-262 nm
	bool twoWheels;
	int selected_wavelength_value;
    double wheel1Tran[FILTER_COUNT];
    double wheel2Tran[FILTER_COUNT];
};

#endif // ATTENSETTINGS_H
