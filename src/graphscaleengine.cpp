#include <graphscaleengine.h>
#include <updatethread.h>
#include <qwt_scale_map.h>


myScaleEngine::myScaleEngine(QwtPlot* _plot, TimeOffset *_timeoffset)
{
	plot = _plot;
	timeoffset = _timeoffset;
	user_ticks.clear();
}

void myScaleEngine::add_ticks(double x)
{
	user_ticks.append(x);
}

void myScaleEngine::move_ticks(double v)
{
	for(int i=0; i<user_ticks.count(); i++)
		user_ticks[i] = user_ticks[i] + v;
}

void myScaleEngine::delete_ticks()
{
	user_ticks.clear();
}

/*!
  Return a transformation, for linear scales
*/

QwtScaleTransformation *myScaleEngine::transformation() const
{
    return new QwtScaleTransformation(QwtScaleTransformation::Linear);
}


/*!
    Align and divide an interval 

   \param maxNumSteps Max. number of steps
   \param x1 First limit of the interval (In/Out)
   \param x2 Second limit of the interval (In/Out)
   \param stepSize Step size (Out)

   \sa setAttribute()
*/
void myScaleEngine::autoScale(int maxNumSteps, 
    double &x1, double &x2, double &stepSize) const
{
    QwtDoubleInterval interval(x1, x2);
    interval = interval.normalized();

    interval.setMinValue(interval.minValue() - lowerMargin());
    interval.setMaxValue(interval.maxValue() + upperMargin());

    if (testAttribute(QwtScaleEngine::Symmetric))
        interval = interval.symmetrize(reference());
 
    if (testAttribute(QwtScaleEngine::IncludeReference))
        interval = interval.extend(reference());

    if (interval.width() == 0.0)
        interval = buildInterval(interval.minValue());

    stepSize = divideInterval(interval.width(), qwtMax(maxNumSteps, 1));

    if ( !testAttribute(QwtScaleEngine::Floating) )
        interval = align(interval, stepSize);

    x1 = interval.minValue();
    x2 = interval.maxValue();

    if (testAttribute(QwtScaleEngine::Inverted))
    {
        qSwap(x1, x2);
        stepSize = -stepSize;
    }
}

/*!
   \brief Calculate a scale division

   \param x1 First interval limit 
   \param x2 Second interval limit 
   \param maxMajSteps Maximum for the number of major steps
   \param maxMinSteps Maximum number of minor steps
   \param stepSize Step size. If stepSize == 0, the scaleEngine
                   calculates one.

   \sa QwtScaleEngine::stepSize(), QwtScaleEngine::subDivide()
*/
QwtScaleDiv myScaleEngine::divideScale(double x1, double x2,
    int maxMajSteps, int maxMinSteps, double stepSize) const
{
    QwtDoubleInterval interval = QwtDoubleInterval(x1, x2).normalized();
    if (interval.width() <= 0 )
        return QwtScaleDiv();

    stepSize = qwtAbs(stepSize);
    if ( stepSize == 0.0 )
    {
        if ( maxMajSteps < 1 )
            maxMajSteps = 1;

        stepSize = divideInterval(interval.width(), maxMajSteps);
    }

    QwtScaleDiv scaleDiv;

    if ( stepSize != 0.0 )
    {
        QwtValueList ticks[QwtScaleDiv::NTickTypes];
        buildTicks(interval, stepSize, maxMinSteps, ticks);

        scaleDiv = QwtScaleDiv(interval, ticks);
    }

    if ( x1 > x2 )
        scaleDiv.invert();

    return scaleDiv;
}

void myScaleEngine::buildTicks(
    const QwtDoubleInterval& interval, double stepSize, int maxMinSteps,
    QwtValueList ticks[QwtScaleDiv::NTickTypes]) const
{
    const QwtDoubleInterval boundingInterval =
        align(interval, stepSize);
    
    ticks[QwtScaleDiv::MajorTick] = 
        buildMajorTicks(boundingInterval, stepSize);

	//сдвигаем тики, следующие после разрыва на целые позиции
	QList<int> remove_list;
	if(timeoffset->offset != NULL){
		for(int i=0; i<ticks[QwtScaleDiv::MajorTick].count(); i++){
			double curr_tick = ticks[QwtScaleDiv::MajorTick][i];
			if(user_ticks.indexOf(ticks[QwtScaleDiv::MajorTick][i]) >= 0) continue;		//не смещаем тики в месте разрыва
			double real_tick = timeoffset->CalcValue(ticks[QwtScaleDiv::MajorTick][i])/1000.0;
			if(i>0)
				if(timeoffset->CalcValue(ticks[QwtScaleDiv::MajorTick][i-1])/1000.0 == real_tick) remove_list.append(i);
			int int_tick = ceil(real_tick);
			if((real_tick != ceil(real_tick)) || !((int_tick%10 == 0) || (int_tick%10 == 5))){
				int k=0; int int_tick2 = int_tick;
				while(k<10){
					k++;
					int_tick2++;
					if((abs(int_tick2%10) == 5) || (abs(int_tick2%10) == 0)) break;
				}
				double plus_offset = 0;
				//int mod10 = int_tick%10;
				//if (abs(int_tick)%10 < 5) plus_offset = 1000*((int_tick/10)*10 + 5 - real_tick);
				//else plus_offset = 1000*((int_tick/10)*10 + 10 - real_tick);

				//plus_offset = 1000*((int_tick2/10)*10 + abs(int_tick2%10) - real_tick);
				plus_offset = 1000*(int_tick2 - real_tick);
				double value_on_scale = ticks[QwtScaleDiv::MajorTick][i] + plus_offset;

				ticks[QwtScaleDiv::MajorTick][i] = value_on_scale;
				double _res_curr = ticks[QwtScaleDiv::MajorTick][i];
				double calc_scale = timeoffset->CalcValue(ticks[QwtScaleDiv::MajorTick][i])/1000.0;
				int int_calc_scale = ceil(calc_scale);
				if((calc_scale != int_calc_scale) || !((int_calc_scale%5 == 0) || (int_calc_scale%10 == 0))){
					timeoffset->addRealTick(curr_tick, int_tick2*1000.0);
				}
				_res_curr+=1;
			}
		}
	}

	//Не получилась затея, вообще при прокрутке некоторые значения вообще пропадают :(
	//удаляем повторяющиеся
	//for(int i=0; i<remove_list.count(); i++) ticks[QwtScaleDiv::MajorTick].removeAt(remove_list[i]);

	if ( maxMinSteps > 0 )
    {
        buildMinorTicks(ticks[QwtScaleDiv::MajorTick], maxMinSteps, stepSize,
            ticks[QwtScaleDiv::MinorTick], ticks[QwtScaleDiv::MediumTick]);
    }
    

	for ( int i = 0; i < QwtScaleDiv::NTickTypes; i++ )
    {
        ticks[i] = strip(ticks[i], interval);

        // ticks very close to 0.0 are 
        // explicitely set to 0.0

        for ( int j = 0; j < (int)ticks[i].count(); j++ )
        {
            if ( abs(ticks[i][j]) < 0.000001*stepSize ) // compareEps is gone in Qwt 6; constant "10^-6" comes from there
                ticks[i][j] = 0.0;
        }
    }


	//удаляем тики, близкие к user_ticks (если расстояние меньше stepSize)

	for(int k=0; k<user_ticks.count(); k++){
		double curr_tick = user_ticks[k];
		if(timeoffset->CalcValue(curr_tick) == 8.5){
			curr_tick -=1;
		}
		for ( int i = 0; i < QwtScaleDiv::NTickTypes; i++ )
			for ( int j = 0; j < (int)ticks[i].count(); j++ )
			{
			
				/*
				QFont font = plot->axisFont(QwtPlot::xBottom);
				QRect rect1 = plot->axisScaleDraw(QwtPlot::xBottom)->labelRect(font, ticks[i][j]);
				QRect rect2 = plot->axisScaleDraw(QwtPlot::xBottom)->labelRect(font, user_ticks[k]);
				if (rect1.isValid() && rect2.isValid() && (rect2.intersects(rect1) || rect1.intersects(rect2))) {
					ticks[i].removeAt(j);
				}
				*/
				

				if(abs(ticks[i][j]-user_ticks[k]) < 2*floor(qwtAbs(stepSize / maxMinSteps)))
					ticks[i].removeAt(j);
			}
	}

	//не рисуем метки на близко расположенных user_ticks
	if(user_ticks.count() >= 2){
		for (int i=1; i<user_ticks.count(); i++){
			if (abs(user_ticks[i] - user_ticks[i-1]) < 2*floor(qwtAbs(stepSize / maxMinSteps)))
				timeoffset->addDisableTicks(user_ticks[i-1]);
		}
	}

	//добавляем тики разрыва на шкалу
	ticks[QwtScaleDiv::MajorTick].append(user_ticks);

}

QwtValueList myScaleEngine::buildMajorTicks(
    const QwtDoubleInterval &interval, double stepSize) const
{
    int numTicks = qRound(interval.width() / stepSize) + 1;
    if ( numTicks > 10000 )
        numTicks = 10000;

    QwtValueList ticks;

	ticks += interval.minValue();
	for (int i = 1; i < numTicks - 1; i++){
		ticks += interval.minValue() + i * stepSize;

	}
    ticks += interval.maxValue();

    return ticks;
}

void myScaleEngine::buildMinorTicks(
    const QwtValueList& majorTicks,
    int maxMinSteps, double stepSize,
    QwtValueList &minorTicks, 
    QwtValueList &mediumTicks) const
{   
    double minStep = divideInterval(stepSize, maxMinSteps);
    if (minStep == 0.0)  
        return; 
        
    // # ticks per interval
    int numTicks = (int)::ceil(qwtAbs(stepSize / minStep)) - 1;
    
    // Do the minor steps fit into the interval?
    if ( abs((numTicks + 1)*qwtAbs(minStep) - qwtAbs(stepSize)) < 0.000001 * stepSize ) // same as line 184, compareEps is no more
    {   
        numTicks = 1;
        minStep = stepSize * 0.5;
    }

    int medIndex = -1;
    if ( numTicks % 2 )
        medIndex = numTicks / 2;

    // calculate minor ticks

    for (int i = 0; i < (int)majorTicks.count(); i++)
    {
        double val = majorTicks[i];
        for (int k = 0; k < numTicks; k++)
        {
            val += minStep;

            double alignedValue = val;
            if ( abs(val) < 0.000001*stepSize )
                alignedValue = 0.0;

            if ( k == medIndex )
                mediumTicks += alignedValue;
			else{
				//на участке с разрывом тики не ставим
				bool allow = true;
				if(i < ((int)majorTicks.count()-1)){
					for(int j=0; j < user_ticks.count(); j++){
						if((majorTicks[i] < user_ticks[j]) && (majorTicks[i+1] > user_ticks[j]))
							allow = false;
						if (!allow) break;
					}
				}

				if(allow) minorTicks += alignedValue;
			}
        }
    }
}

/*!
  \brief Align an interval to a step size

  The limits of an interval are aligned that both are integer
  multiples of the step size.

  \param interval Interval
  \param stepSize Step size

  \return Aligned interval
*/
QwtDoubleInterval myScaleEngine::align(
    const QwtDoubleInterval &interval, double stepSize) const
{
    const double x1 = 
        QwtScaleArithmetic::floorEps(interval.minValue(), stepSize);
    const double x2 = 
        QwtScaleArithmetic::ceilEps(interval.maxValue(), stepSize);

    return QwtDoubleInterval(x1, x2);
}
