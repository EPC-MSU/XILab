#ifndef SLIDERSETTINGS_H
#define SLIDERSETTINGS_H

class SliderSettings {
public:
	SliderSettings(double _left, double _right, bool _loadfromdevice, bool _watchoverlimits, bool _showsecondary);
	SliderSettings();
    ~SliderSettings();

	double left;
	double right;
	bool loadfromdevice;
	bool watchoverlimits;
	bool showsecondary;
};

#endif // SLIDERSETTINGS_H
