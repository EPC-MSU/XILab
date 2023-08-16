#ifndef NETWORKSETTINGS_H
#define NETWORKSETTINGS_H

#include <deviceinterface.h>
#include <functions.h>
#include <ximc.h>
class NetworkSettings
{
public:
    NetworkSettings(DeviceInterface *_devinterface);
    ~NetworkSettings(void);

	void LoadNetworkSettings();
    void LoadPasswordSettings();
    void LoadRestNetSettings();

    network_settings_t net_set;
    QString passw;

private:
	DeviceInterface *devinterface;
	
};

#endif // NETWORKSETTINGS_H
