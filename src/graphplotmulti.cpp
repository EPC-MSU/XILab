#include <QPainter>
#include <graphplot.h>
#include <graphplotmulti.h>
#include <graphcommonvars.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_text_engine.h>
#include <qwt_plot_canvas.h>
#include <graphscaleengine.h>

GraphPlotMulti::GraphPlotMulti(TimeOffset *_timeoffset, ChartLog *log, int data_type)
{
	timeoffset = _timeoffset;
	for (int i=0; i<DIGNUM; i++)
		curve[i] = NULL;
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
	//setAxisScaleDraw(QwtPlot::yLeft, static_cast<QwtScaleDraw *>(new DivScaleDraw(1)));
	axisWidget(QwtPlot::yLeft)->setScaleDraw(new voidScaleDraw());
	//axisScaleEngine(QwtPlot::yLeft)->setMargins(1,1);

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

	txt	<< QStaticText("STATE_RIGHT_EDGE")
		<< QStaticText("STATE_LEFT_EDGE")
		<< QStaticText("STATE_BUTTON_RIGHT")
		<< QStaticText("STATE_BUTTON_LEFT")
		<< QStaticText("STATE_GPIO_LEVEL")
		<< QStaticText("STATE_BRAKE")
		<< QStaticText("STATE_REV_SENSOR")
		<< QStaticText("STATE_SYNC_INPUT")
		<< QStaticText("STATE_SYNC_OUTPUT")
		;
}

GraphPlotMulti::~GraphPlotMulti()
{
}

void GraphPlotMulti::drawCanvas(QPainter* p)
{
	QwtPlot::drawCanvas(p);

	QFont font;
	font.setPointSize((int)max(p->window().height()*0.2/(DIGNUM+1),8.0));
	p->setFont(font);
	for (int i=0; i<DIGNUM; i++) {
		p->drawStaticText(0, p->window().height()*(i+0.5)/(DIGNUM+1), txt.at(DIGNUM-1-i));
	}
}

//обновление title при изменении множителя
void GraphPlotMulti::updateTitle(int new_divisor)
{
	QString powstr;
	if(new_divisor == 10)
		powstr = "";
	else
		powstr.setNum(log10((double)new_divisor));

	QwtText axisTitle;
	QFont font;
	font.setPointSize(10);
	font.setBold(true);
	axisTitle.setFont(font);
	axisTitle.setTextEngine(QwtText::TeXText, new QwtRichTextEngine());
	
	axisTitle.setText(title + "<br>");
	setAxisTitle(QwtPlot::yLeft, axisTitle);
}

void GraphPlotMulti::Init(GraphCurveSettings *stgs, QString _suffix, QString _title)
{
	suffix = _suffix;
	title = _title;
	updateTitle(1);
	
	for (int i=0; i<DIGNUM; i++) {
		curve[i] = new QwtPlotCurve(title);
		curve[i]->attach(this);
	}
	ReInit(stgs);
}

void GraphPlotMulti::ReInit(GraphCurveSettings *stgs)
{
	for (int i=0;i<DIGNUM;i++){
		stgs->pen.setColor(QColor().fromHsv((i*360/DIGNUM + 315)%360, 255, 255)); // hue is offset so that the rainbow goes from red to violet
		curve[i]->setPen(stgs->pen);
		curve[i]->setRenderHint(QwtPlotItem::RenderAntialiased, stgs->antialiasing);
	}
	setAxisScale(QwtPlot::yLeft, 0, DIGNUM+1);
}

void GraphPlotMulti::setCaptions(QString _suffix, QString _title)
{
	suffix = _suffix;
	title = _title;
	if(curve[0]!=NULL)
		curve[0]->setTitle(title);
}

QwtPlotCurve* GraphPlotMulti::getCurve(int n)
{
	return curve[n];
}

void GraphPlotMulti::setCookedData(const double * x, const double * y, int size, int nth_curve)
{
	curve[nth_curve]->setSamples(x, y, size);
}

TimeOffset* GraphPlotMulti::getTimeOffset() {return timeoffset; }
void GraphPlotMulti::setSuffix(QString _suff) {suffix = _suff; }

void GraphPlotMulti::setSizeHint(QSize newSizeHint){
	mySizeHint = newSizeHint;
}

QSize GraphPlotMulti::sizeHint() const
{
	return mySizeHint;
}
