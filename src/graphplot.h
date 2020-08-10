#ifndef GRAPHPLOT_H
#define GRAPHPLOT_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

#include <chartlog.h>
#include <graphtimeoffset.h>
#include <graphsettings.h>
#include <graphpicker.h>
#include <graphcommonvars.h>
#include <graphscaledraw.h>

class GraphPlot;
class DivScaleDraw;

class GraphPlot : public QwtPlot
{
public:
	GraphPlot(TimeOffset *_timeoffset, ChartLog *log, int data_type);
	~GraphPlot();

	void Init(GraphPlot *_plots[], GraphCurveSettings *stgs, QString _suffix, QString _title);
	void ReInit(GraphCurveSettings *stgs);
	void setCaptions(QString _suffix, QString _title);
	
	void setDivisor(double new_divisor);
	int getDivisor();
	void updateTitle(double new_divisor);

	void setSizeHint(QSize newSizeHint);
	QSize sizeHint() const;


	myPicker* getPicker();
	QwtPlotCurve* getCurve();
	TimeOffset* getTimeOffset();

	void setSuffix(QString _suff);
private:
	GraphPlot *plots[PLOTSNUM];
	QwtPlotCurve *curve;
	myPicker *picker;
	TimeOffset *timeoffset;
	QString suffix;
	QString title;
	QSize mySizeHint;
	bool blockChangeSignal;
	int divisor;
	ChartLog *log;
	int data_type;
};

#endif // GRAPHPLOT_H
