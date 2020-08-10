#include <chartlog.h>
#include <chartdlg.h>
#include <ctime>
#include <cmath>


ChartLog::ChartLog(QDateTime *base)
{
	list = NULL;

	basetime = base;
	cnt = 0;
	size = 0;
}

ChartLog::~ChartLog(void)
{
	delete_list();	
}

void ChartLog::InsertMeasure(int number, QDateTime msrTime, measurement_t meas)
{
	Q_ASSERT(number == 0);
	TList *temp;

	if(list == NULL){
		list = new TList;
		list->next = NULL;
		list->back = NULL;
		//вставка нового элемента в начало списка
		temp = list;
	}
	else{
		//вставка нового элемента в начало списка
		temp = new TList;
		temp->next = list;
		temp->back = NULL;
		list->back = temp;
		list = temp;
	}

	//записываем новые измерения
	temp->encoder_position = meas.status.EncPosition;
	if (meas.enabled_calb) {
		temp->position = meas.status_calb.CurPosition;
		temp->speed = meas.status_calb.CurSpeed;
		if (isSpeedUnitRotation(meas.fbs.FeedbackType)) {
			temp->speed *= meas.fbs.CountsPerTurn / 60;
		}
	} else {
		temp->position = meas.status.CurPosition + (double)meas.status.uCurPosition / meas.step_factor;
		temp->speed = meas.status.CurSpeed + (double)meas.status.uCurSpeed / meas.step_factor;
	}
	temp->engineCurrent = meas.status.Ipwr;
	temp->engineVoltage = meas.status.Upwr/100.0;
	temp->pwrCurrent = meas.status.Ipwr;		//ток силовой части равен току через мотор
	temp->pwrVoltage = meas.status.Upwr/100.0;
	temp->usbCurrent = meas.status.Iusb;
	temp->usbVoltage = meas.status.Uusb/100.0;
	temp->currentA = meas.chart_data.WindingCurrentA;
	temp->currentB = meas.chart_data.WindingCurrentB;
	temp->currentC = meas.chart_data.WindingCurrentC;
	temp->voltageA = meas.chart_data.WindingVoltageA/100.0;
	temp->voltageB = meas.chart_data.WindingVoltageB/100.0;
	temp->voltageC = meas.chart_data.WindingVoltageC/100.0;
    temp->temp = meas.status.CurT/10.0;
	temp->pwm = meas.chart_data.DutyCycle/100.0;
	temp->msec = basetime->msecsTo(msrTime);
	temp->joy = meas.chart_data.Joy;
	temp->pot = meas.chart_data.Pot;
	temp->GENRflags = meas.status.Flags;
	temp->GPIOflags = meas.status.GPIOFlags;
	temp->step_factor = meas.step_factor;

	size++;
}

void ChartLog::delete_list()
{
	TList *curr;
	while(list != NULL){
		curr = list;
		list = list->next;
		delete curr;
	}
	list = NULL;
	size = 0;
	cnt = 0;
}

void ChartLog::cleanPositionSpeed()
{
	TList *curr = list;
	while (curr != NULL) {
		curr->position = 0;
		curr->speed = 0;
		curr = curr->next;
	}
}
