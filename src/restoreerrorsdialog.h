#ifndef RESTOREERRORSDIALOG_H
#define RESTOREERRORSDIALOG_H

#include <QtGui/QWidget>
#include <QDialog>

namespace Ui {
	class RestoreErrorsDialogClass;
}

class RestoreErrorsDialog : public QDialog {
public:
	RestoreErrorsDialog(QWidget *parent, QStringList items);
	~RestoreErrorsDialog();

	Ui::RestoreErrorsDialogClass *m_ui;
};

#endif // RESTOREERRORSDIALOG_H
