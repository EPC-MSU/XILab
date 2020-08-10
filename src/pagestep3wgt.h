#ifndef PAGESTEP3WGT_H
#define PAGESTEP3WGT_H

#include <QtGui/QWidget>
#include <deviceinterface.h>
#include <updatethread.h>
#include <controllersettings.h>
#include <vector>
#include <aes.h>

namespace Ui {
    class PageStep3WgtClass;
}

class PageStep3Wgt : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PageStep3Wgt)
public:
    explicit PageStep3Wgt(QWidget *parent, UpdateThread *_updateThread, ControllerSettings *_controllerStgs, DeviceInterface *_devinterface);
    virtual ~PageStep3Wgt();

	QString GetKey();
	void SetKey(QString);
	void FromUiToClass();

protected:
    virtual void changeEvent(QEvent *e);

private:
	DeviceInterface *devinterface;
    Ui::PageStep3WgtClass *m_ui;
    UpdateThread *updateThread;
	ControllerSettings *controllerStgs;
	std::vector<uint8_t> key;
	std::vector<uint8_t> encrypted_key(libximc::init_random_t irnd, std::vector<uint8_t> key);
	bootloader_version_t boot_need_encrypted_key_version;

public slots:
    void OnSetKeyBtnPressed();
	void OnWriteSNBtnPressed();
	void OnRebootToBoot();

signals:
	void keyChangedSgn(unsigned int key);
	void serialUpdatedSgn();
};

#endif // PAGESTEP3WGT_H
