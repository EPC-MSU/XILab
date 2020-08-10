#ifndef PAGESTAGEWGT_H
#define PAGESTAGEWGT_H

#include <QtGui/QWidget>
#include <QSettings>
#include <stagesettings.h>

namespace Ui {
    class PageStageWgtClass;
}

class PageStageWgt : public QWidget {
    Q_OBJECT
public:
	explicit PageStageWgt(QWidget *parent, StageSettings *_stageStgs);
    virtual ~PageStageWgt();

	void FromClassToUi();
	void FromUiToClass(StageSettings *sStgs);
private:
    Ui::PageStageWgtClass *m_ui;
	StageSettings *stageStgs;
public slots:
	void testValues();
	void SetDisabled(bool set);
};

#endif // PAGESTAGEWGT_H
