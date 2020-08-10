#include <QPainter>
#include <QPalette>
#include <qwt_scale_draw.h>
#include <qwt_scale_map.h>
#include <qwt_global.h>
#include <qwt_scale_div.h>
#include <qwt_text.h>
#include <qpen.h>
#include <qwt_math.h>
#include <qwt_painter.h>

#include <graphscaledraw.h>
#include <updatethread.h>

#include <qmatrix.h>
#define QwtMatrix QMatrix

class TimeScaleDraw::PrivateData
{
public:
    PrivateData():
        len(0),
        alignment(QwtScaleDraw::BottomScale),
        labelAlignment(0),
        labelRotation(0.0)
    {
	}

    QPoint pos;
    int len;
    Alignment alignment;
    QwtScaleDiv scldiv;
    Qt::Alignment labelAlignment;
    double labelRotation;
};

TimeScaleDraw::TimeScaleDraw(TimeOffset *_timeoffset)
{
	d_data = new TimeScaleDraw::PrivateData();
	timeoffset = _timeoffset;
	min_time = 0;
    setLength(100);
}

QwtText TimeScaleDraw::label(double v) const
{
	QwtScaleDraw();

	QString ret;

        if(!timeoffset->testTick(v)) return QwtText("", QwtText::AutoText);

	v = timeoffset->CalcValue(v);

	//если v<min_time, в этой точке нет кривой, метку не рисуем
        if(v<min_time) return QwtText("", QwtText::AutoText);
	ret.setNum(((int)v/100)/10.0);
	return ret;
}

void TimeScaleDraw::setMinTime(double time)
{
	min_time = time;
}

/*-------------------------------------------------------------*/

DivScaleDraw::DivScaleDraw(int _divisor)
{
	divisor = _divisor;
}

void DivScaleDraw::setDivisor(int new_divisor)
{
	divisor = new_divisor;
}

int DivScaleDraw::getDivisor()
{
	return divisor;
}
