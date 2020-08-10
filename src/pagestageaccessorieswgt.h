#ifndef PAGESTAGEACCWGT_H
#define PAGESTAGEACCWGT_H

#include <QtGui/QWidget>
#include <QSettings>
#include <stagesettings.h>

namespace Ui {
    class PageStageAccessoriesWgtClass;
}

class PageStageAccessoriesWgt : public QWidget {
	Q_OBJECT
public:
	explicit PageStageAccessoriesWgt(QWidget *parent, StageSettings *_stageStgs);
    virtual ~PageStageAccessoriesWgt();

	void FromClassToUi();
	void FromUiToClass(StageSettings *lsStgs);
private:
    Ui::PageStageAccessoriesWgtClass *m_ui;
	StageSettings *stageStgs;
public slots:
	void SetDisabled(bool set);
private slots:
	void OnTSTypeChanged(int index);
};

#endif // PAGESTAGEACCWGT_H
