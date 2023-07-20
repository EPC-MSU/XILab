#include <QString>
#include <functions.h>
#include "mainwindow.h"
#include <netsettings.h>

static QString from_uint20_toqstr(const unsigned int inta[20])
{
    QString ret;
    int i = 0;
    while (i != 20 && inta[i] != 0)
    {
        ret += QChar(inta[i++]);
    }
    return ret;
}

NetworkSettings::NetworkSettings(DeviceInterface *_devinterface)
{
	devinterface = _devinterface;

    memset(&net_set, 0, sizeof(network_settings_t));
}

NetworkSettings::~NetworkSettings(void)
{
}

void NetworkSettings::LoadNetworkSettings()
{
    LoadPasswordSettings();
    LoadRestNetSettings();
}

void NetworkSettings::LoadPasswordSettings()
{
	bool temp_state = devinterface->cs->updatesEnabled();
	devinterface->cs->setUpdatesEnabled(false);

    password_settings_t _pst;
    
    try {
        result_t result = devinterface->get_password_settings(&_pst);
        //uint32_t serialnumber;
        //result_t result = devinterface->get_serial_number(&serialnumber);
        if (result != result_ok) {
            throw my_exception("Error calling get_password_settings");
        }
        passw = from_uint20_toqstr(_pst.UserPassword);
    }
    catch (my_exception &e) {
        e.text();
       // devinterface->close_device(); // Will trigger NoDevice according to requirements in bug #8220
    }

   	devinterface->cs->setUpdatesEnabled(temp_state);
}

void NetworkSettings::LoadRestNetSettings()
{
    bool temp_state = devinterface->cs->updatesEnabled();
    devinterface->cs->setUpdatesEnabled(false);
       
    try {
        result_t  result = devinterface->get_network_settings(&net_set);
        if (result != result_ok) {
            throw my_exception("Error calling get_network_settings");
        }
    }
    catch (my_exception &e) {
        e.text();
       // devinterface->close_device(); // Will trigger NoDevice according to requirements in bug #8220
    }

    devinterface->cs->setUpdatesEnabled(temp_state);
}

