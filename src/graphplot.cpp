#include <QPainter>
#include <graphplot.h>
#include <graphcommonvars.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_text_engine.h>
#include <qwt_plot_canvas.h>
#include <graphscaleengine.h>

GraphPlot::GraphPlot(TimeOffset *_timeoffset, ChartLog *log, int data_type)
{
	timeoffset = _timeoffset;
	picker = NULL;
	curve = NULL;
	this->log = log;
	this->data_type = data_type;

	blockChangeSignal = false;

	setCanvasBackground(Qt::white);
	setStyleSheet("QwtPlot { padding: 3px }");
	plotLayout()->setCanvasMargin(-1);
	((QwtPlotCanvas*)canvas())->setLineWidth(0);
	setAutoReplot(false);

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	setMinimumHeight(50);
	setSizeHint(QSize(200, 200));

	//отступ слева для шкалы yLeft
	axisScaleDraw(QwtPlot::yLeft)->setMinimumExtent(LEFT_EXTENT);

	//yRight axis to make free space at right;
	enableAxis(QwtPlot::yRight);
	setAxisScaleDraw(QwtPlot::yRight, new voidScaleDraw);
	setAxisMaxMajor(QwtPlot::yRight, 0);
	setAxisMaxMinor(QwtPlot::yRight, 0);
	setAxisScale(QwtPlot::yRight, 0, 1);
	QFont font;
	font.setLetterSpacing(QFont::AbsoluteSpacing, 0);
	font.setWeight(1);
	setAxisFont(QwtPlot::yRight, font);
	axisScaleDraw(QwtPlot::yRight)->setMinimumExtent(3);

	setAxisScaleDraw(QwtPlot::xTop, new TimeScaleDraw(timeoffset));
	setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw(timeoffset));
	setAxisScaleEngine(QwtPlot::xTop, new myScaleEngine(this, timeoffset));
	setAxisScaleEngine(QwtPlot::xBottom, new myScaleEngine(this, timeoffset));

	//курсор :)
	setCursor(Qt::ArrowCursor);
	canvas()->setCursor(Qt::ArrowCursor);

	//yLeft
	divisor = 1;
	axisWidget(QwtPlot::yLeft)->setScaleDraw(new DivScaleDraw(1));

	//горизонтальная ось
	setAxisMaxMajor(QwtPlot::xBottom, 4);
	setAxisMaxMinor(QwtPlot::xBottom, 8);
	setAxisMaxMajor(QwtPlot::xTop, 4);
	setAxisMaxMinor(QwtPlot::xTop, 8);
	//вертикальная ось
	setAxisMaxMinor(QwtPlot::yLeft, 8);
	setAxisMaxMajor(QwtPlot::yLeft, 4);

	QwtPlotGrid *grid = new QwtPlotGrid();
	QPen gridPen(QColor(200,200,200));
	grid->setPen(gridPen);
	grid->setMinPen(QPen(QColor(230,230,230)));
	grid->setMajPen(QPen(QColor(200,200,200)));
	grid->enableXMin(true);
	grid->enableYMin(true);
	grid->attach(this);
}

GraphPlot::~GraphPlot()
{
	if(picker!=NULL) delete picker;
}

//обновление title при изменении множителя
void GraphPlot::updateTitle(double new_divisor)
{
	QString powstr;
	if(new_divisor == 10)
		powstr = "";
	else
		powstr.setNum(log10(new_divisor));

	QwtText axisTitle;
	QFont font;
	font.setPointSize(10);
	font.setBold(true);
	axisTitle.setFont(font);
	axisTitle.setTextEngine(QwtText::TeXText, new QwtRichTextEngine());

	if(new_divisor == 1)
		axisTitle.setText(title + ",<br>" + suffix);
	else
		axisTitle.setText(title + ",<br>" + "x10<sup>" + powstr + "</sup>&nbsp;" + suffix);

	setAxisTitle(QwtPlot::yLeft, axisTitle);
}

void GraphPlot::Init(GraphPlot *_plots[], GraphCurveSettings *stgs, QString _suffix, QString _title)
{
	suffix = _suffix;
	title = _title;

	curve = new QwtPlotCurve(title);
	curve->setPen(stgs->pen);
	curve->setRenderHint(QwtPlotItem::RenderAntialiased, stgs->antialiasing);
	curve->attach(this);
	setAxisScale(QwtPlot::yLeft, stgs->minScale, stgs->maxScale);
	if(stgs->autoscale)
		setAxisAutoScale(QwtPlot::yLeft);	

	setDivisor(1);

	for(int i=0; i<PLOTSNUM; i++) plots[i] = _plots[i];
	picker = new myPicker(this, plots, timeoffset, suffix, log, data_type);
}

void GraphPlot::ReInit(GraphCurveSettings *stgs)
{
	curve->setPen(stgs->pen);
	curve->setRenderHint(QwtPlotItem::RenderAntialiased, stgs->antialiasing);
	if(!stgs->autoscale)
		setAxisScale(QwtPlot::yLeft, stgs->minScale, stgs->maxScale);
	else
		setAxisAutoScale(QwtPlot::yLeft);

	picker->InitMarker();
}

void GraphPlot::setCaptions(QString _suffix, QString _title)
{
	suffix = _suffix;
	title = _title;
	if(curve!=NULL)
		curve->setTitle(title);
	if(picker!=NULL)
		picker->setSuffix(suffix);

	 axisWidget(QwtPlot::yLeft)->setTitle(title + ",\n" + suffix);
}

myPicker* GraphPlot::getPicker() {return picker; }
QwtPlotCurve* GraphPlot::getCurve() {return curve; }
TimeOffset* GraphPlot::getTimeOffset() {return timeoffset; }
void GraphPlot::setSuffix(QString _suff) {suffix = _suff; }

void GraphPlot::setSizeHint(QSize newSizeHint){
	mySizeHint = newSizeHint;
}

QSize GraphPlot::sizeHint() const
{
	return mySizeHint;
}

void GraphPlot::setDivisor(double new_divisor){
	divisor = new_divisor;
	static_cast<DivScaleDraw *>(axisScaleDraw(QwtPlot::yLeft))->setDivisor(new_divisor);
	axisScaleDraw(QwtPlot::yLeft)->setScaleDiv(axisScaleDraw(QwtPlot::yLeft)->scaleDiv());
	updateTitle(new_divisor);
}

int GraphPlot::getDivisor() { 
	return static_cast<DivScaleDraw *>(axisScaleDraw(QwtPlot::yLeft))->getDivisor(); 
}
