#ifndef	COLORBUTTON_H
#define COLORBUTTON_H

#include <QPushButton>
#include <QPainter>
#include <QColorDialog>

class ColorButton : public QPushButton
    {
    Q_OBJECT

public:
    ColorButton(QWidget *parent = 0);
    ~ColorButton();

private:
    QColor BkColor;

public:
	void setColor(QColor);
	QColor getColor();

protected:
    void paintEvent(QPaintEvent *);

protected slots:
	void openDialog();

};

#endif // COLORBUTTON_H
