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
	QMovie movie;

private:
	QTimer timer;

public slots:
	void restartmovie();
};

#endif // RESTOREERRORSDIALOG_H
