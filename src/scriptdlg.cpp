#include <scriptdlg.h>
#include <mainwindow.h>
#include <functions.h>
#include <QFileDialog>
#include <QMessageBox>
#include <loggedfile.h>

extern bool save_configs;
ScriptDlg::ScriptDlg(QWidget *parent, MotorSettings *motorStgs, QString title)
    : QDialog(parent)
{
	local_motorStgs = motorStgs;

	ui.setupUi(this);

	QObject::connect(this, SIGNAL(finished(int)), this, SLOT(close()));

	connect(ui.startBtn, SIGNAL(clicked()), this, SLOT(OnStartBtnClicked()));
	connect(ui.stopBtn,  SIGNAL(clicked()), this, SLOT(OnStopBtnClicked()));
	connect(ui.loadBtn,  SIGNAL(clicked()), this, SLOT(OnLoadBtnClicked()));
	connect(ui.saveBtn,  SIGNAL(clicked()), this, SLOT(OnSaveBtnClicked()));

	statusBar = new QStatusBar(this);
	ui.mainVLayout->addWidget(statusBar);
	statusBar->resize(statusBar->size().width(), 20);

	QString wndTitle = "Scripting " + title;
	setWindowTitle(wndTitle);

 }

ScriptDlg::~ScriptDlg()
{
	if (save_configs)
		SaveScratch();
}

void ScriptDlg::SaveScratch()
{
	LoggedFile file(DefaultScriptScratchName());
	file.open(QIODevice::WriteOnly);
	file.write(ui.codeEdit->toPlainText().toUtf8().data());
	file.close();
}

void ScriptDlg::InitUI()
{
	stateLbl.setFixedWidth(65);
	stateLbl.setAlignment(Qt::AlignCenter);
	statusBar->addPermanentWidget(&stateLbl);

	LoggedFile file(DefaultScriptScratchName());
	if (!file.exists()) { // no scratch file, let's try loading default script
		file.setFileName(getDefaultUserfilesInstallPath() + "/scripts/cyclic.txt");
	}
	file.open(QIODevice::ReadOnly);
	QTextStream in(&file);
	in.setCodec("UTF-8");
	ui.codeEdit->setPlainText(in.readAll());
	ui.codeEdit->setAcceptRichText(false);
	file.close();

	Highlighter *highlighter = new Highlighter(ui.codeEdit->document());
	Q_UNUSED(highlighter)
}

void ScriptDlg::close()
{
}

void ScriptDlg::OnStartBtnClicked()
{
	try {
		SaveScratch();
		QScriptSyntaxCheckResult check = QScriptEngine::checkSyntax(ui.codeEdit->toPlainText().toUtf8().data());
		int errline = check.errorLineNumber(); // returns -1 if no error

		QTextBlockFormat blkFormat, errFormat;
		blkFormat.setBackground(QBrush(Qt::white, Qt::SolidPattern));
		errFormat.setBackground(QBrush(Qt::red, Qt::SolidPattern));

		ui.codeEdit->setUpdatesEnabled(false);
		ui.codeEdit->textCursor().beginEditBlock();

		if (check.state() != QScriptSyntaxCheckResult::Valid)
			statusBar->showMessage("Error " + check.errorMessage() + " on line " + toStr(errline));
		else {
			statusBar->showMessage("Syntax check OK, starting script...", MESSAGE_TIMEOUT);
			emit start();
		}

		blocks.clear();
		ui.codeEdit->moveCursor(QTextCursor::Start);
		int blockcount = ui.codeEdit->document()->blockCount();
		for (int i=0; i<blockcount; i++) {
			blocks.append(ui.codeEdit->textCursor());
			if (i+1 == errline)
				ui.codeEdit->textCursor().setBlockFormat(errFormat);
			else
				ui.codeEdit->textCursor().setBlockFormat(blkFormat);
			ui.codeEdit->moveCursor(QTextCursor::NextBlock);
		}
		blocks.append(ui.codeEdit->textCursor()); // final one

		ui.codeEdit->textCursor().endEditBlock();
		ui.codeEdit->setUpdatesEnabled(true);
	} catch (...) {
	//	Log("Caught exception in MainWindow.Scripts", SOURCE_XILAB, LOGLEVEL_ERROR);
	}
}

void ScriptDlg::OnStopBtnClicked()
{
	statusBar->showMessage("Stopping script...", MESSAGE_TIMEOUT);
	if (ui.stopBtn->text() == "Stop")
		emit stop(false); // soft-stop, qengine->abortEvaluation
	else if (ui.stopBtn->text() == "Force stop")
		emit stop(true); // hard-stop, kill thread
}

void ScriptDlg::OnLoadBtnClicked()
{
	if(default_load_path == "")
		default_load_path = getDefaultScriptsPath();
	
	QString filename;
	filename = QFileDialog::getOpenFileName(this, tr("Open script file"), default_load_path, tr("Text files (*.txt);;Script files (*.js);;All files (*.*)"));
	
	if(filename.isEmpty()) {
		statusBar->showMessage("Loading cancelled", MESSAGE_TIMEOUT);
		return;
	}

	LoggedFile file(filename);
	file.open(QIODevice::ReadOnly);
	QTextStream in(&file);
	in.setCodec("UTF-8");
	ui.codeEdit->setPlainText(in.readAll());
	file.close();
}

void ScriptDlg::OnSaveBtnClicked()
{
	if(default_save_path == "")
		default_save_path = getDefaultScriptsPath();

	QString filename;
	filename = QFileDialog::getSaveFileName(this, tr("Save script file"), default_save_path, tr("Text files (*.txt);;Script files (*.js);;All files (*.*)"));

	if(filename.isEmpty()) {
		statusBar->showMessage("Saving cancelled", MESSAGE_TIMEOUT);
		return;
	}

	LoggedFile file(filename);
	file.open(QIODevice::WriteOnly);
	file.write(ui.codeEdit->toPlainText().toUtf8().data());
	file.close();
}

void ScriptDlg::OnScriptExecPositionChanged(int line)
{
	QTextBlockFormat blkFormat;
	blkFormat.setBackground(QBrush(Qt::white, Qt::SolidPattern));
	ui.codeEdit->textCursor().setBlockFormat(blkFormat);
	blkFormat.setBackground(QBrush(Qt::lightGray, Qt::SolidPattern));
	if (blocks.length() > line && line > 0) {
		ui.codeEdit->setTextCursor(blocks.at(line-1));
		ui.codeEdit->textCursor().setBlockFormat(blkFormat);
	}
}

void ScriptDlg::OnExceptionThrown(QScriptValue exception)
{
	statusBar->showMessage(exception.toString(), MESSAGE_TIMEOUT);
	emit script_exception(exception.toString());
	this->OnScriptExecPositionChanged(0);
}

void ScriptDlg::FromUiToSettings(QSettings *settings)
{
	//положение окна
	settings->beginGroup("scriptWindow_params");
	settings->setValue("position", pos());
	settings->setValue("size", size());
	settings->setValue("last_visible", isVisible());
    //дефолтные пути
    settings->setValue("load_path", default_load_path);
    settings->setValue("save_path", default_save_path);

	settings->endGroup();
}
void ScriptDlg::FromSettingsToUi(QSettings *my_settings, QSettings *default_stgs)
{
	QSettings *settings;
	if (my_settings != NULL)
		settings = my_settings;
	else if (default_stgs != NULL)
		settings = default_stgs;
	else
		return;

	settings->beginGroup("scriptWindow_params");

	//положение окна
	if(settings->contains("position"))
		move(settings->value("position", QPoint(100, 200)).toPoint());

	if(settings->contains("size"))
		resize(settings->value("size", QSize(615, 260)).toSize());
/*
	if(settings->contains("last_visible"))
		setVisible(settings->value("last_visible", false).toBool());
*/
	//дефолтные пути
	default_load_path = settings->value("load_path", "").toString();
	default_save_path = settings->value("save_path", "").toString();

	settings->endGroup();
}