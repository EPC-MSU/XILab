#include <QPen>
#include <QPointer>
#include <qwt_text_engine.h>
#include <graphpicker.h>
#include <functions.h>
#include <time.h>

#include <graphplot.h>

myPicker::myPicker(GraphPlot *_parent, GraphPlot *_plots[], TimeOffset *_timeoffset, QString _suff, ChartLog *log, int data_type): QwtPlotPicker(_parent->canvas())
{
	for(int i=0; i<PLOTSNUM; i++) plots[i] = _plots[i];
	parent = _parent;
	marker = new QwtPlotMarker();
	InitMarker();
	marker->attach(parent);
	marker->setVisible(false);
	setTrackerMode(QwtPicker::AlwaysOn);
	timeoffset = _timeoffset;
	suffix = _suff;
	this->log = log;
	this->data_type = data_type;
}

void myPicker::setSuffix(QString suffix)
{
	this->suffix = suffix;
}


QwtText myPicker::trackerText (const QwtDoublePoint &pos) const
{
	if(parent->getCurve()->dataSize() == 0){
		hideMarker();
                return QwtText("", QwtText::AutoText);
	}

	QwtText ret;

	//заливка бэкграунда мягким цветом графика
	QColor color(parent->getCurve()->pen().color());
	color.setAlpha(64);
	QBrush brush;
	brush.setStyle(Qt::SolidPattern);
	brush.setColor(color);
	QPen pen(parent->getCurve()->pen());
	pen.setBrush(brush);
	ret.setBackgroundBrush(brush);
	ret.setBackgroundPen(pen);
	ret.setPaintAttribute(QwtText::PaintBackground, true);
	ret.setTextEngine(QwtText::TeXText, new QwtRichTextEngine());

	//рассчитываем y
	unsigned int i=1;
	bool found = false;
	while(i<parent->getCurve()->dataSize()){ 
		if((parent->getCurve()->sample(i-1).x() >= ceil(pos.x())-1) && (parent->getCurve()->sample(i).x() <= ceil(pos.x())-1)) {
			found = true;
			break;
		}
		i++; 
	}

	//здесь мы имеем i - позицию сэмпла, над которой находится указатель мыши
	//чтобы отобразить "time, step ustep/factor" надо получить данные логгера и смещения полей
	TList *curr = log->list;
	if(found){
		double timeValue = parent->getCurve()->sample(i).x();
		found = false;
		while(curr != NULL && !found){
			if(fabs(timeValue - curr->msec) < 0.001){
				found = true;
				break;
			}
			curr = curr->next;
		}
	}


	//рассчитываем x
	QString time;
	if(found)
		time = toStr(static_cast<int>(timeoffset->CalcValue(parent->getCurve()->sample(i).x())/1000)) + " s " + toStr((int)(timeoffset->CalcValue(parent->getCurve()->sample(i).x()) - static_cast<int>(timeoffset->CalcValue(parent->getCurve()->sample(i).x())/1000)*1000), true, 3) + " ms";
	else 
		time = "";

	if(!found && (pos.x() < 0)) return QwtText("", QwtText::AutoText);

	//необычный случай, но надо его обработать
	if(!found) 
		ret.setText(" " + time + "; ??? ");
	else{
		marker->setXValue(parent->getCurve()->sample(i).x());
		marker->setYValue(parent->getCurve()->sample(i).y());
		marker->setVisible(true);
		double data = 0.0, udata;
		if(data_type >= 0 && curr->step_factor > 1){	//в полношаговом режиме фактор не отображается
			if(data_type == POSNUM){
				data = curr->position;
			}
			else if(data_type == SPEEDNUM){
				data = curr->speed;
			}
			udata = 0;
			ret.setText(" " + time + "; " + QString("%1 %2/%3 ").arg(data).arg(udata).arg(curr->step_factor) + suffix + " ");
		}
		else
			ret.setText(" " + time + "; " + toStr(parent->getCurve()->sample(i).y()) + " " + suffix + " ");
	}

	//qDebug()<<"picker time = "<<clock()-t1<<" ms";

	return ret;
}

void myPicker::widgetMouseMoveEvent(QMouseEvent *e)
{
	QwtPicker::widgetMouseMoveEvent(e);
	
	//свой график перерисовываем, а остальные - только если у них включен маркер
	for(int i=0; i<PLOTSNUM; i++) {
		if(plots[i] == parent) plots[i]->replot();
		else if(plots[i]->getPicker()->getMarker()->isVisible()){
			plots[i]->getPicker()->hideMarker();
			plots[i]->replot();
		}
	}
}


void myPicker::InitMarker()
{
	QPen pen(parent->getCurve()->pen());
	pen.setWidth(1);
	QBrush brush(parent->getCurve()->brush());
	brush.setStyle(Qt::SolidPattern);
	brush.setColor(pen.color());
	int a;
	if(parent->getCurve()->pen().widthF() < 10)
		a = floor(2.5*parent->getCurve()->pen().widthF());
	else
		a = floor(1.5*parent->getCurve()->pen().widthF());
	if(a<5) a = 5;
	marker->setSymbol(new QwtSymbol(QwtSymbol::Ellipse, brush, pen, QSize(a,a)));
	marker->setRenderHint(QwtPlotItem::RenderAntialiased, true);
}

QwtPlotMarker* myPicker::getMarker()
{
	return marker;
}

void myPicker::hideMarker() const
{
	marker->setVisible(false);
}

