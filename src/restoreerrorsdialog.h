#ifndef RESTOREERRORSDIALOG_H
#define RESTOREERRORSDIALOG_H

#include <QtGui/QWidget>
#include <QWidget>
#include <QDialog>
#include <QMovie>
#include <QTimer>

namespace Ui {
	class RestoreErrorsDialogClass;
}

class RestoreErrorsDialog : public QDialog {


public:
	RestoreErrorsDialog(QWidget *parent, QStringList items);
	~RestoreErrorsDialog();

	
	Ui::RestoreErrorsDialogClass *m_ui;
	//QMovie *movie;

public slots:
	void restartmovie();

private:
	QMovie movie;
};

#endif // RESTOREERRORSDIALOG_H
