#include <deadzoneslider.h>

DeadZoneSlider::DeadZoneSlider(Qt::Orientation orientation, QWidget *parent) : QSlider(orientation, parent), default_value_(-1)
{
	connect(this, SIGNAL(valueChanged(int)), SLOT(VerifyDefaultValue(int)));
	fullrange = 10000;
	setMinimum(0);
	setMaximum(fullrange);
}

void DeadZoneSlider::mouseMoveEvent(QMouseEvent *ev)
{
	Q_UNUSED(ev)
;
}

void DeadZoneSlider::mousePressEvent(QMouseEvent *ev)
{
	Q_UNUSED(ev)
;
}

void DeadZoneSlider::mouseReleaseEvent(QMouseEvent *ev)
{
	Q_UNUSED(ev)
;
}

void DeadZoneSlider::paintEvent(QPaintEvent *ev)
{
	int position = 
		QStyle::sliderPositionFromValue(minimum(),
                                                   maximum(),
                                                   default_value_,
                                                   width());
	Q_UNUSED(position)
	QPainter painter(this);
	painter.setPen(QPen(QColor(128,128,128)));

	painter.setBrush(QBrush(QColor(192,192,192), Qt::SolidPattern));
	painter.drawRect(0, 1*height()/6, width()-1, 2*height()/6);
	painter.drawRect(0, 3*height()/6, width()-1, 2*height()/6);

	painter.setBrush(QBrush(QColor(255,0,0), Qt::SolidPattern));
	painter.drawRect(deadzone_lower*width()/fullrange, 1*height()/6, (deadzone_upper-deadzone_lower)*width()/fullrange-1, 2*height()/6);

	painter.setBrush(QBrush(QColor(0,255,0), Qt::SolidPattern));
	painter.drawRect(whole_lower*width()/fullrange, 3*height()/6, (whole_upper-whole_lower)*width()/fullrange-1, 2*height()/6);

	QSlider::paintEvent(ev);
}

void DeadZoneSlider::VerifyDefaultValue(int value)
{
	if (default_value_ == -1) {
		default_value_ = value;
		update();
	}
}

void DeadZoneSlider::SetWholeRange(int lower, int upper)
{
	whole_lower = lower;
	whole_upper = upper;
}

void DeadZoneSlider::SetDeadZoneRange(int lower, int upper)
{
	deadzone_lower = lower;
	deadzone_upper = upper;
}
