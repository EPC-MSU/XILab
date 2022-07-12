#ifndef DEADZONESLIDER_H
#define DEADZONESLIDER_H
#include <QtGui>

class DeadZoneSlider : public QSlider {
  Q_OBJECT

public:
	DeadZoneSlider(Qt::Orientation orientation, QWidget *parent = NULL);

protected:
	void paintEvent(QPaintEvent *ev);
	void mouseMoveEvent(QMouseEvent *ev);
	void mousePressEvent(QMouseEvent *ev);
	void mouseReleaseEvent(QMouseEvent *ev);

private slots:
	void VerifyDefaultValue(int value);

public slots:
	void SetWholeRange(int lower, int upper);
	void SetDeadZoneRange(int lower, int upper);

private:
	int default_value_;
	int fullrange;
	int whole_lower, whole_upper, deadzone_lower, deadzone_upper;
};

#endif // DEADZONESLIDER_H
