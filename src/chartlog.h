#ifndef CHARTLOG_H
#define CHARTLOG_H

#include <deviceinterface.h>
#include <qwt_scale_draw.h>
#include <QDateTime>
#include <QObject>

typedef struct TList{
	TList *back;
	TList *next;
	double encoder_position;
	double position;
    double speed;
	double engineCurrent;
	double engineVoltage;
	double pwrCurrent;
	double pwrVoltage;
	double usbCurrent;
	double usbVoltage;
    double currentA;
	double currentB;
	double currentC;
    double voltageA;
	double voltageB;
	double voltageC;
	double pwm;
	double temp;
	double joy;
	double pot;
	double msec;
	unsigned int GENRflags;
	unsigned int GPIOflags;
	unsigned int step_factor;
} TList;


class ChartLog: public QObject
{
	Q_OBJECT
public:
    ChartLog(QDateTime *base);
    ~ChartLog(void);

    void InsertMeasure(int number, QDateTime msrTime, measurement_t meas);
	void delete_list();
	void cleanPositionSpeed();
	int getDataSize(){ return sizeof(TList)-2*sizeof(int*); }
	int getFillSize(){ return 256 - getDataSize(); }
	
	TList *list;
	int size;

private:
	QDateTime *basetime;
	int cnt;

signals:
	void SgnUpdateExtent();
};

#endif // CHARTLOG_H
