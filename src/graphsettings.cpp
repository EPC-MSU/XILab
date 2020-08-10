#include <graphsettings.h>

#define INIT(STGS) { \
	STGS = new GraphCurveSettings(); \
	STGS->pen.setCapStyle(Qt::RoundCap); \
	STGS->pen.setJoinStyle(Qt::RoundJoin); \
	}

GraphSettings::GraphSettings()
{
	commonStgs = new GraphCommonSettings();
	INIT(positionStgs);
	INIT(speedStgs);
	INIT(encoderStgs);
	INIT(envoltageStgs);
	INIT(pwrvoltageStgs);
	INIT(usbvoltageStgs);
	INIT(voltAStgs);
	INIT(voltBStgs);
	INIT(voltCStgs);
	INIT(currAStgs);
	INIT(currBStgs);
	INIT(currCStgs);
	INIT(encurrentStgs);
	INIT(pwrcurrentStgs);
	INIT(usbcurrentStgs);
	INIT(pwmStgs);
	INIT(temperatureStgs);
	INIT(joyStgs);
	INIT(potStgs);
	INIT(flagsStgs);
}

GraphSettings::~GraphSettings()
{
}


//CommonSettings
GraphCommonSettings::GraphCommonSettings()
{
}

GraphCommonSettings::~GraphCommonSettings()
{
}

//CurveSettings
GraphCurveSettings::GraphCurveSettings()
{
}

GraphCurveSettings::~GraphCurveSettings()
{
}