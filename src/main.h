#ifndef MAIN_H
#define MAIN_H

#include <deviceinterface.h>
#include <messagelog.h>

struct state_vars_t{
	status_t state;
	status_calb_t status_calb;
	chart_data_t chart_data;
	analog_data_t analog_data;

	calibration_t calb_data;
	bool connect;
	int device_mode;
	int last_mode;
};

QString compileDate();

#define XILAB_VERSION "2.0.13"

#endif //MAIN_H
