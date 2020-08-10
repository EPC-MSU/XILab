#ifndef MYLISTWIDGET_H
#define MYLISTWIDGET_H

#include <QListWidget>

class myListWidget : public QListWidget {
public:
	QSize sizeHint() const;
	myListWidget(QWidget *parent);
private:
	myListWidget(const myListWidget&);
	myListWidget& operator=(const myListWidget&);
};

#endif //MYLISTWIDGET_H
