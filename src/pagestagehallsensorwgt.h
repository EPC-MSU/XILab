#ifndef PAGESTAGEHSWGT_H
#define PAGESTAGEHSWGT_H

#include <QtGui/QWidget>
#include <QSettings>
#include <stagesettings.h>

namespace Ui {
    class PageStageHallsensorWgtClass;
}

class PageStageHallsensorWgt : public QWidget {
	Q_OBJECT
public:
	explicit PageStageHallsensorWgt(QWidget *parent, StageSettings *_stageStgs);
    virtual ~PageStageHallsensorWgt();

	void FromClassToUi();
	void FromUiToClass(StageSettings *lsStgs);
private:
    Ui::PageStageHallsensorWgtClass *m_ui;
	StageSettings *stageStgs;
public slots:
	void SetDisabled(bool set);
};

#endif // PAGESTAGEHSWGT_H
