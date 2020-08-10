#ifndef PAGESTAGEGEARWGT_H
#define PAGESTAGEGEARWGT_H

#include <QtGui/QWidget>
#include <QSettings>
#include <stagesettings.h>

namespace Ui {
    class PageStageGearWgtClass;
}

class PageStageGearWgt : public QWidget {
    Q_OBJECT
public:
	explicit PageStageGearWgt(QWidget *parent, StageSettings *_stageStgs);
    virtual ~PageStageGearWgt();

	void FromClassToUi();
	void FromUiToClass(StageSettings *lsStgs);
private:
    Ui::PageStageGearWgtClass *m_ui;
	StageSettings *stageStgs;
public slots:
	void SetDisabled(bool set);
};

#endif // PAGESTAGEGEARWGT_H
