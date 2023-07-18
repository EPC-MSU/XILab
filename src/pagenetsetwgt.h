#ifndef PAGENETSETWGT_H
#define PAGENETSETWGT_H

#include <QtGui/QWidget>
#include <deviceinterface.h>
#include <updatethread.h>
#include <controllersettings.h>

namespace Ui {
    class PageNetSetClass;
}

class PageNetSetWgt : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PageNetSetWgt)
public:
    explicit PageNetSetWgt(QWidget *parent, UpdateThread *_updateThread, network_settings_t *_net_sets, DeviceInterface *_devinterface);
    virtual ~PageNetSetWgt();


    void FromUiToClass() {};

protected:
    virtual void changeEvent(QEvent *e);

private:
	DeviceInterface *devinterface;
    Ui::PageNetSetClass *m_ui;
    UpdateThread *updateThread;
	network_settings_t *network_sets;
	uint8_t ipv4[4];
	uint8_t subnet[4];
	uint8_t gateway[4];
	bool is_dhcp;

public slots:
    
	void OnWriteBtnPressed();
	

};

#endif // PAGENETSETWGT_H
