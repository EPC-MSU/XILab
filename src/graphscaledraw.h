#ifndef GRAPHSCALEDRAW_H
#define GRAPHSCALEDRAW_H

#include <chartdlg.h>
#include <qwt_scale_draw.h>


class TimeScaleDraw: public QwtScaleDraw
{
public:
    TimeScaleDraw(TimeOffset *_timeoffset);
    QwtText label(double v) const;
	void setMinTime(double time);
private:
	TimeOffset *timeoffset;
	double min_time;		//координата Х левой точки графика. Метки левее нее - не рисуются. По умолчанию ее значение 0.

    class PrivateData;
    PrivateData *d_data;
};


//класс ScaleDraw, который не рисует ничего :)
class voidScaleDraw : public QwtScaleDraw
{
public:
	voidScaleDraw(){
		setTickLength(QwtScaleDiv::MajorTick, 0);
		setTickLength(QwtScaleDiv::MediumTick, 0);
		setTickLength(QwtScaleDiv::MinorTick, 0);		
	}
	virtual QwtText label(double) const
	{
		return QwtText();
	}

};


// Установкой делителя divisor можно управлять цифрами на шкале слайдера
class DivScaleDraw: public QwtScaleDraw
{
public:
	DivScaleDraw(int _divisor);
        virtual QwtText label(double v) const
	{
		QString s;
		s.setNum(v/divisor);
		return s;
	}
	void setDivisor(int new_divisor);	
	int getDivisor();
private:
    int divisor;
};

#endif // GRAPHSCALEDRAW_H
