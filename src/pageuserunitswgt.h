#ifndef PAGEUSERUNITSWGT_H
#define PAGEUSERUNITSWGT_H

#include <QtGui/QWidget>
#include <QSettings>
#include <userunitsettings.h>
#include <deviceinterface.h>

// Validation fields section for custom units page
#define MAX_PRECISION 30

namespace Ui {
	class PageUserUnitsWgtClass;
}

class PageUserUnitsWgt : public QWidget {
    Q_OBJECT
	Q_DISABLE_COPY(PageUserUnitsWgt)
public:
	explicit PageUserUnitsWgt(QWidget *parent, UserUnitSettings *uuStgs, DeviceInterface *_devinterface);
    virtual ~PageUserUnitsWgt();

	void FromUiToClass();
	void FromClassToUi(bool _emit=false);
	void SetChecked(bool _emit = true, bool enable = false);
	void FromUiToSettings(QSettings *settings);
	bool enableChkisChecked();
	QString FromSettingsToUi(QSettings *settings, QSettings *default_stgs = NULL);

	Ui::PageUserUnitsWgtClass *m_ui;

public slots:
	void OnLoadTableBtnClicked();
	void OnCloseTableBtnClicked();

signals:
	void SgnChangeUU();

private:
	UserUnitSettings* uuStgs;
	DeviceInterface *devface;
	result_t result;

private slots:
	void OnValidationUserSettings();
};

#endif // PAGEUSERUNITSWGT_H
