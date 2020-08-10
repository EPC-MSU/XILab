#include "mylistwidget.h"

myListWidget::myListWidget(QWidget *parent) : QListWidget(parent)
{
}

QSize myListWidget::sizeHint() const
{
	return QSize(0,0);
}
