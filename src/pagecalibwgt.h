#ifndef PAGECALIBWGT_H
#define PAGECALIBWGT_H

#include <QtGui/QWidget>
#include <QSettings>
#include <QMovie>
#include <QMessageBox>
#include <QTimer>

#include <deviceinterface.h>
#include <infobox.h>


namespace Ui {
    class PageCalibWgtClass;
}

class PageCalibWgt : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PageCalibWgt)
public:
    explicit PageCalibWgt(QWidget *parent, QString dev_name, DeviceInterface *_devinterface);
    virtual ~PageCalibWgt();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::PageCalibWgtClass *m_ui;
	DeviceInterface *devinterface;
	char device_name[255];
	InfoBox *infoBox;
	QTimer *timer;

public slots:
	void UpdatePageText();
};

#endif // PAGECALIBWGT_H
