#include <graphtimeoffset.h>
#include <updatethread.h>


TimeOffset::TimeOffset(){
	offset = NULL;
	ticks = NULL;
	dis_ticks = NULL;
	num_ticks = 0;
}

TimeOffset::TimeOffset(TOffset *_offset){
	offset = _offset;
}


TimeOffset::~TimeOffset(){
	Clear();
}

void TimeOffset::addRealTick(double calcValue, double realValue)
{
	TTicksVal *curr = ticks;
	
	//исключаем повторное вхождение
	while(curr != NULL) {
		if(curr->calc_value == calcValue) return;
		curr = curr->next;
	}

	num_ticks++;

	curr = ticks;
	//мотаем до конца
	while((curr != NULL) && (curr->next != NULL))
		curr = curr->next;

	//создаем новый элемент
	TTicksVal *temp = new TTicksVal;
	temp->next = NULL;
	temp->calc_value = calcValue;
	temp->real_value = realValue;
	if(ticks == NULL) ticks = temp;
	else curr->next = temp;
}

void TimeOffset::addOffset(double t1, double t2){
	TOffset *curr = offset;

	//исключаем повторное вхождение
	while(curr != NULL) {
		//if(curr->timestamp == (t1 + UPDATE_INTERVAL)) return;
		if(curr->timestamp == (t1)) return;
		curr = curr->next;
	}

	curr = offset;
	//мотаем до конца
	while((curr != NULL) && (curr->next != NULL))
		curr = curr->next;

	//создаем новый элемент
	TOffset *temp = new TOffset;
	temp->next = NULL;
	temp->timestamp = t1;// + UPDATE_INTERVAL;
	temp->offset = t2 - t1;
	if(offset == NULL) offset = temp;
	else curr->next = temp;
}

void TimeOffset::addDisableTicks(double v)
{
	TDisTicks *curr = dis_ticks;

	//исключаем повторное вхождение
	while(curr != NULL) {
		if(curr->value == v) return;
		curr = curr->next;
	}

	curr = dis_ticks;
	//мотаем до конца
	while((curr != NULL) && (curr->next != NULL))
		curr = curr->next;

	//создаем новый элемент
	TDisTicks *temp = new TDisTicks;
	temp->next = NULL;
	temp->value = v;
	if(dis_ticks == NULL) dis_ticks = temp;
	else curr->next = temp;
}

double TimeOffset::CalcValue(double v) const
{
	//проверяем сначала в этом списке
	TTicksVal *temp = ticks;
	while(temp != NULL){
		if (v == temp->calc_value) return temp->calc_value;
		temp = temp->next;
	}

	TOffset *curr = offset;
	double v_temp = v;
	while(curr != NULL){
		//для сохраненного графика в области < 0. Его смещение не должно смещать текущий график
		if((curr->timestamp < 0) && (v < 0)){
			if(curr->timestamp <= v_temp)
				v+=curr->offset;
		}
		//для текущего графика
		else if((v >= 0) && (curr->timestamp >= 0)){
			if(curr->timestamp <= v_temp)
				v+=curr->offset;
		}
		curr = curr->next;
	}

	return v;
}


bool TimeOffset::testTick(double v)
{
	TDisTicks *curr = dis_ticks;
	while(curr != NULL){
		if(curr->value == v) return false;
		curr = curr->next;
	}

	return true;
}

void TimeOffset::moveData(double time_offset)
{
	//смещаем TOffset
	TOffset *temp = offset;
	while(temp != NULL) {
		temp->timestamp += time_offset;
		temp = temp->next;
	}

	//смещаем TTicksVal
	TTicksVal *temp2 = ticks;
	while(temp2 != NULL) {
		temp2->calc_value += time_offset;
		temp2->real_value += time_offset;
		temp2 = temp2->next;
	}

	//смещаем TDisTicks
	TDisTicks *temp3 = dis_ticks;
	while(temp3 != NULL) {
		temp3->value += time_offset;
		temp3 = temp3->next;
	}

}

void TimeOffset::Clear(){

	//удаляем offset 
	TOffset *curr;
	while(offset != NULL){
		curr = offset;
		offset = offset->next;
		delete curr;
	}
	offset = NULL;

	//удаляем ticks
	TTicksVal *temp;
	while(ticks != NULL){
		temp = ticks;
		ticks = ticks->next;
		delete temp;
	}
	ticks = NULL;

	//удаляем dis_ticks
	TDisTicks *temp2;
	while(dis_ticks != NULL){
		temp2 = dis_ticks;
		dis_ticks = dis_ticks->next;
		delete temp2;
	}
	dis_ticks = NULL;

	num_ticks = 0;
}