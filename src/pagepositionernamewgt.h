#ifndef PAGEPOSITIONERNAMEWGT_H
#define PAGEPOSITIONERNAMEWGT_H

#include <QtGui/QWidget>
#include <QSettings>
#include <stagesettings.h>

namespace Ui {
    class PagePositionerNameWgtClass;
}

class PagePositionerNameWgt : public QWidget {
    Q_OBJECT
public:
	explicit PagePositionerNameWgt(QWidget *parent, StageSettings *_stageStgs);
    virtual ~PagePositionerNameWgt();

	void FromClassToUi();
	void FromUiToClass(StageSettings *sStgs);

private:
    Ui::PagePositionerNameWgtClass *m_ui;
	StageSettings *stageStgs;
public slots:
	void SetDisabled(bool set);
};

#endif // PAGEPOSITIONERNAMEWGT_H
