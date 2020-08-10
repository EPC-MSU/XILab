#ifndef GRAPHPLOTMULTI_H
#define GRAPHPLOTMULTI_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

#include <chartlog.h>
#include <graphtimeoffset.h>
#include <graphsettings.h>
#include <graphcommonvars.h>
#include <graphscaledraw.h>
#include <QStaticText>

class GraphPlotMulti;
class DivScaleDraw;


class GraphPlotMulti : public QwtPlot
{
public:
	GraphPlotMulti(TimeOffset *_timeoffset, ChartLog *log, int data_type);
	~GraphPlotMulti();

	void Init(GraphCurveSettings *stgs, QString _suffix, QString _title);
	void ReInit(GraphCurveSettings *stgs);
	void setCaptions(QString _suffix, QString _title);
	
	void updateTitle(int new_divisor);

	void setSizeHint(QSize newSizeHint);
	QSize sizeHint() const;

	void setCookedData(const double * x, const double * y, int size, int nth_curve);

	QwtPlotCurve* getCurve(int n);
	TimeOffset* getTimeOffset();

	void setSuffix(QString _suff);
private:
	void drawCanvas(QPainter*);
	QList<QStaticText> txt;
	QwtPlotCurve *curve[DIGNUM];
	TimeOffset *timeoffset;
	QString suffix;
	QString title;
	QSize mySizeHint;
	bool blockChangeSignal;
	int divisor;
	ChartLog *log;
	int data_type;
};

#endif // GRAPHPLOTMULTI_H
