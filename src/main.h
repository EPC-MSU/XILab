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

<<<<<<< HEAD
#define XILAB_VERSION "3.0.1"
=======
#define XILAB_VERSION "1.20.2"
>>>>>>> dev-1.20

#endif //MAIN_H
