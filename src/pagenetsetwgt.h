#ifndef PAGENETSETWGT_H
#define PAGENETSETWGT_H

#include <QtGui/QWidget>
#include <deviceinterface.h>
#include <updatethread.h>
#include <netsettings.h>

namespace Ui {
    class PageNetSetClass;
}

class PageNetSetWgt : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PageNetSetWgt)
public:
    explicit PageNetSetWgt(QWidget *parent, UpdateThread *_updateThread, NetworkSettings *_net_sets, DeviceInterface *_devinterface);
    virtual ~PageNetSetWgt();


    void FromUiToClass();
    void FromClassToUi();

protected:
    virtual void changeEvent(QEvent *e);

private:
	DeviceInterface *devinterface;
    Ui::PageNetSetClass *m_ui;
    UpdateThread *updateThread;
	NetworkSettings *pnetsets;
	
private slots:
    
	void OnWrBtnPressed();
    void OnWrPswBtnPressed();
};

#endif // PAGENETSETWGT_H
