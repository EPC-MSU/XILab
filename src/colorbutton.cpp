#include <colorbutton.h>

ColorButton::ColorButton(QWidget *parent)
	: QPushButton(parent)
{
	connect(this, SIGNAL(clicked()), this, SLOT(openDialog()));
}

ColorButton::~ColorButton()
{

}

void ColorButton::paintEvent(QPaintEvent *paint)
{
	QPushButton::paintEvent(paint);
	QPainter p(this);
	p.save();
	
	p.setPen(Qt::gray);
	p.setBrush(QBrush(BkColor));
	int height = p.viewport().height();
	p.drawRect(height/4, height/4, height*2, height*2/4);

	p.setPen(Qt::black);
	QFont f = p.font();
	f.setPointSize(height*2/5);
	p.setFont(f);
	p.drawText(QPoint(height*2.5, height*3/4),BkColor.name());

	p.restore();
}


void ColorButton::openDialog()
{
	QColor selected_color = QColorDialog::getColor(BkColor, this, "Select a color", 0);
	if (selected_color.isValid()) {
		BkColor = selected_color;
	}
}

void ColorButton::setColor(QColor color)
{
	BkColor = color;
	update();
}

QColor ColorButton::getColor()
{
	return BkColor;
}