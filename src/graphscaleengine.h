#ifndef GRAPHSCALEENGINE_H
#define GRAPHSCALEENGINE_H

#include <graphtimeoffset.h>

#include <qwt_scale_engine.h>
#include <qwt_math.h>
#include <qwt_global.h>
#include <qwt_scale_div.h>
#include <qwt_interval.h>
#include <qwt_plot.h>
#include <qwt_point_3d.h>
#include <qwt_compat.h>


class myScaleEngine: public QwtScaleEngine
{
public:
	myScaleEngine(QwtPlot* _plot, TimeOffset *_timeoffset);

	void add_ticks(double x);
	void move_ticks(double v);
	void delete_ticks();

    virtual void autoScale(int maxSteps, 
        double &x1, double &x2, double &stepSize) const;

    virtual QwtScaleDiv divideScale(double x1, double x2,
        int numMajorSteps, int numMinorSteps,
        double stepSize = 0.0) const;

    virtual QwtScaleTransformation *transformation() const;

protected:
    QwtDoubleInterval align(const QwtDoubleInterval&,
        double stepSize) const;

private:
	QwtValueList user_ticks;
	TimeOffset *timeoffset;
	QwtPlot *plot;

    void buildTicks(
        const QwtDoubleInterval &, double stepSize, int maxMinSteps,
        QwtValueList ticks[QwtScaleDiv::NTickTypes]) const;

    void buildMinorTicks(
        const QwtValueList& majorTicks,
        int maxMinMark, double step,
        QwtValueList &, QwtValueList &) const;

    QwtValueList buildMajorTicks(
        const QwtDoubleInterval &interval, double stepSize) const;
};

#endif // GRAPHSCALEENGINE_H
