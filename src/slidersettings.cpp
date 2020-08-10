#include <slidersettings.h>

SliderSettings::SliderSettings(double _left, double _right, bool _loadfromdevice, bool _watchoverlimits, bool _showsecondary)
{
	left = _left;
	right = _right;
	loadfromdevice = _loadfromdevice;
	watchoverlimits = _watchoverlimits;
	showsecondary = _showsecondary;
}

SliderSettings::SliderSettings()
{
}

SliderSettings::~SliderSettings()
{
}