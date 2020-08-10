#ifndef	INFOBOX_H
#define INFOBOX_H

#include <QMovie>
#include <QMessageBox>
#include <QDialogButtonBox>

namespace Ui {
    class InfoBoxClass;
}

class InfoBox : public QDialog{
	Q_OBJECT
public:
    InfoBox(QWidget *parent = NULL);
	~InfoBox();

	void setIcon(QMessageBox::Icon icon);
	void setText(QString text);
	void setButtons(QDialogButtonBox::StandardButtons buttons);

	void setButtonsVisible(bool on);
	void setIconVisible(bool on);
	void setMovieVisible(bool on);
	void setTextVisible(bool on);
	void setClosingOnReject(bool on){closingOnReject = on;}
	void show();

signals:
	void cancelled();

private:
    Ui::InfoBoxClass *ui;
	QMovie movie;
	bool pressed;
	bool closingOnReject;
private slots:
	void buttonPressed(QAbstractButton *button);
};

#endif // INFOBOX_H
