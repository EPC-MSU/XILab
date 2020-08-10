#ifndef PAGESTAGEDCWGT_H
#define PAGESTAGEDCWGT_H

#include <QtGui/QWidget>
#include <QSettings>
#include <stagesettings.h>

namespace Ui {
    class PageStageDCMotorWgtClass;
}

class PageStageDCMotorWgt : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(PageStageDCMotorWgt)
public:
	explicit PageStageDCMotorWgt(QWidget *parent, StageSettings *_stageStgs);
    virtual ~PageStageDCMotorWgt();

	void FromClassToUi();
	void FromUiToClass(StageSettings *lsStgs);
private:
    Ui::PageStageDCMotorWgtClass *m_ui;
	StageSettings *stageStgs;
public slots:
	void SetDisabled(bool set);
private slots:
	void OnMotorTypeChanged(int index);
};

#endif // PAGESTAGEDCWGT_H
