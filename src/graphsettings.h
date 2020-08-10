#ifndef GRAPHSETTINGS_H
#define GRAPHSETTINGS_H

#include <QColor>
#include <QPen>

class GraphCommonSettings {
public:
	GraphCommonSettings();
    ~GraphCommonSettings();

	unsigned int datapoints;
	unsigned int interval;
	bool dataUpdate;
	bool autostart;
private:
};

class GraphCurveSettings {
public:
	GraphCurveSettings();
    ~GraphCurveSettings();

	QPen pen;
	bool antialiasing;
	bool autoscale;
	int minScale;
	int maxScale;

private:
};



//GraphSettings
class GraphSettings {
public:
	GraphSettings();
    ~GraphSettings();

	GraphCommonSettings *commonStgs;
	GraphCurveSettings *positionStgs;
	GraphCurveSettings *speedStgs;
	GraphCurveSettings *encoderStgs;
	GraphCurveSettings *envoltageStgs;
	GraphCurveSettings *pwrvoltageStgs;
	GraphCurveSettings *usbvoltageStgs;
	GraphCurveSettings *voltAStgs;
	GraphCurveSettings *voltBStgs;
	GraphCurveSettings *voltCStgs;
	GraphCurveSettings *encurrentStgs;
	GraphCurveSettings *pwrcurrentStgs;
	GraphCurveSettings *usbcurrentStgs;
	GraphCurveSettings *currAStgs;
	GraphCurveSettings *currBStgs;
	GraphCurveSettings *currCStgs;
	GraphCurveSettings *pwmStgs;
	GraphCurveSettings *temperatureStgs;
	GraphCurveSettings *joyStgs;
	GraphCurveSettings *potStgs;
	GraphCurveSettings *flagsStgs;
private:
};

#endif // GRAPHSETTINGS_H
