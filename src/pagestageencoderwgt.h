#ifndef PAGESTAGEENCWGT_H
#define PAGESTAGEENCWGT_H

#include <QtGui/QWidget>
#include <QSettings>
#include <stagesettings.h>

namespace Ui {
    class PageStageEncoderWgtClass;
}

class PageStageEncoderWgt : public QWidget {
	Q_OBJECT
public:
	explicit PageStageEncoderWgt(QWidget *parent, StageSettings *_stageStgs);
    virtual ~PageStageEncoderWgt();

	void FromClassToUi();
	void FromUiToClass(StageSettings *lsStgs);
private:
    Ui::PageStageEncoderWgtClass *m_ui;
	StageSettings *stageStgs;
public slots:
	void SetDisabled(bool set);
private slots:
	void OnDiffOutChanged(int index);
	void OnPushOutChanged(int index);
};

#endif // PAGESTAGEENCWGT_H
