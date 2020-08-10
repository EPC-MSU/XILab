#include <intsettings.h>


IntSettings::IntSettings(){
    skinType = GeneralSkin;
	twoWheels=true;
	selected_wavelength_value=1055;
    selected_wavelength = 1;
    for (int i = 0;i != FILTER_COUNT;i++) wheel1Tran[i] = 0.1*i;
    for (int i = 0;i != FILTER_COUNT;i++) wheel2Tran[i] = 0.01*i;
}

IntSettings::~IntSettings(){}

