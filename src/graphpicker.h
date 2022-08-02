#ifndef GRAPHPICKER_H
#define GRAPHPICKER_H

#include <qwt_plot.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include <qwt_math.h>
#include <qwt_text.h>
//#include <graphplot.h>
#include <chartlog.h>
#include <graphtimeoffset.h>
#include <graphcommonvars.h>
#include <qwt_compat.h>

class GraphPlot;

class myPicker: public QwtPlotPicker
{
public:
	myPicker(GraphPlot *_parent, GraphPlot *_plots[], TimeOffset *_timeoffset, QString _suff, ChartLog *log, int data_type);

	QwtPlotMarker* getMarker();
	void InitMarker();
	void hideMarker() const;
	void setSuffix(QString suffix);

private:
	QwtPlotMarker *marker;
	GraphPlot *plots[PLOTSNUM];
	GraphPlot *parent;
	TimeOffset *timeoffset;
	QString suffix;
	ChartLog *log;
	int data_type;

protected:
	using QwtPlotPicker::trackerText;
	QwtText trackerText (const QwtDoublePoint &pos) const;
	virtual void widgetMouseMoveEvent(QMouseEvent *);
};

#endif // GRAPHPICKER_H
