#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QWidget>
#include <QMovie>
#include <QTimer>
#include <QMouseEvent>
#include <devicethread.h>
#include <devicesearchsettings.h>
#include <mainwindow.h>
#include "bindy.h"

namespace Ui {
    class StartWindowClass;
}

//состояния стартового окна
#define STATE_QUESTION	1
#define STATE_LIST		2
#define STATE_THINKING	4

//Values of hint management timers.
#define TIME_DOUBLE_CLICK 400
#define TIME_SHOW 1200

namespace Columns
{
	enum
	{
		COLUMN_URI,
		COLUMN_SERIAL,
		COLUMN_FRIENDLY_NAME,
		COLUMN_STAGE
	};
};

class StartWindow : public QWidget {
    Q_OBJECT
public:
    StartWindow(QWidget *parent = 0);
    ~StartWindow();
	
	void startSearching();
	QList<QString> getSelectedDevices();

	void LoadAxisConfig(QList<uint32_t> serials);
	bool inited;
private:

    Ui::StartWindowClass *m_ui;
	DeviceThread *devicethread;
	QList<QString> selectedDevices;
	QList<uint32_t> serials;
	QMovie movie;
	int state;
	volatile int targetSpeed;
	int lastFrame;
	QTimer timer, timer1, timer2;
	QPoint lastPoint;
	bool b_move;
	DeviceSearchSettings *dss;
	PageProgramConfigWgt * pagepc;
	QMutex mutex;

	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void hideEvent(QHideEvent * event);

	void loadTableSettings(QString filename);
	void saveTableSettings(QString filename);

	void loadWindowGeometry(QString filename);
	void saveWindowGeometry(QString filename);

	void showRight();
	void hideRight();

	void SetPicture();

	void  ShowDeviseSelClic();

public slots:
void deviceListRecieved(bool enum_ok, QStringList names, QStringList descriptions, QStringList friendlyNames, QStringList positionerNames, QList<uint32_t> serials, QList<Qt::ItemFlags> flags);
	void itemDoubleClicked(QTableWidgetItem*);
	void itemClicked(QTableWidgetItem *item);
	void itemPressed(QTableWidgetItem *item);
	void selectBtnClicked();
	void retryBtnClicked();
	void cancelBtnClicked();
	void openLastConfigBtnClicked();
	void exBtnClicked();
	void noDevicesLinuxHelperClicked();
	void timerUpdate();
	bool return_probe();
	void timer1Full();
	void timer2Full();
	void SelClic();
};







#endif