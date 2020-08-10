#ifndef SCRIPTDLG_H
#define SCRIPTDLG_H

#include <QStatusBar>
#include <QDialog>
#include <QLabel>
#include <QTimer>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QSettings>
#include <ui_scriptdlg.h>
#include <updatethread.h>
#include <motorsettings.h>
#include <controllersettings.h>
#include <QScriptValue>
#include "highlighter.h"
#include "scriptthread.h"

class ScriptDlg : public QDialog
{
    Q_OBJECT

public:
    ScriptDlg(QWidget *parent, MotorSettings *motorStgs, QString title);
    ~ScriptDlg();

	void InitUI();
	void FromUiToSettings(QSettings *settings);
	void FromSettingsToUi(QSettings *settings, QSettings *default_stgs = NULL);
	void SaveScratch();

private:
    Ui::ScriptDlgClass ui;
	MotorSettings *local_motorStgs;
	QVBoxLayout *scrLayout;

	QStatusBar *statusBar;
	QLabel filesizeLbl;
	QLabel stateLbl;
	QLabel fileopenLbl;
	QString default_load_path;
	QString default_save_path;
	QLabel testLbl;
	QList<QTextCursor> blocks;

signals:
	void start();
	void stop(bool);
	void script_exception(QString);

public slots:
	void close();

	void OnStartBtnClicked();
	void OnStopBtnClicked();
	void OnLoadBtnClicked();
	void OnSaveBtnClicked();

	void OnScriptExecPositionChanged(int line);
	void OnExceptionThrown(QScriptValue exception);
};

#endif // SCRIPTDLG_H
