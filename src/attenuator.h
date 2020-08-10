#ifndef ATTENUATOR_H
#define ATTENUATOR_H

#include <QWidget>
#include <QDebug>
#include <QPoint>
#include <QMouseEvent>
#include <QPixmap>
#include <QSignalMapper>
#include <QPushButton>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <attensettings.h>
#include <infobox.h>

namespace Ui {
class Attenuator;
}

class Attenuator : public QWidget
{
    Q_OBJECT
    
public:
	bool use_one_wheel;
	bool should_move;
    explicit Attenuator(AttenSettings& intset,QWidget *parent = 0);
    void resetOneWheel(int i);
	void changeSettings();
	void get_movePosit(unsigned int& Pos1,unsigned int& Pos2);
	void setCalibrationStatus(Calibration::Status calb_status);
	Calibration::Status getCalibrationStatus() { return this->calb_status; }
    ~Attenuator();
public slots:
    void clicked(int buttonId);
    void resetAllBut();
    void find_Best(const QString& pos);
signals:
	void ChangePosition();
	void Calibrate();
    void getPosit(int);
	void error_get(const QString& );
private:
	InfoBox infoBox;
    int WheelPos1;
    int WheelPos2;
    Ui::Attenuator *ui;
    QButtonGroup* groupWheel1;
    QButtonGroup* groupWheel2;
    QSignalMapper* signalMapper;
    QPushButton* filters[FILTER_COUNT*WHEEL_COUNT];
    QLabel* lab_filters[FILTER_COUNT*WHEEL_COUNT];
    QHBoxLayout* vboxWheel[WHEEL_COUNT];//layout for filters
	QVBoxLayout* vboxVWheel[FILTER_COUNT*WHEEL_COUNT];//vertical layout for filters and lab_filters
	void setUiCalibrationStatus(Calibration::Status calb_status);
	Calibration::Status calb_status;
	QPixmap redPix;
	QPixmap yellowPix;
	QPixmap greenPix;
};

#endif // ATTENUATOR_H
