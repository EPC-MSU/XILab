#ifndef PAGEABOUTWGT_H
#define PAGEABOUTWGT_H

#include <QtGui/QWidget>
#include <QString>

extern QString xilab_ver;

namespace Ui {
    class PageAboutWgtClass;
}

class PageAboutWgt : public QWidget{
	Q_OBJECT
    Q_DISABLE_COPY(PageAboutWgt)
public:
    PageAboutWgt(QWidget *parent);
    ~PageAboutWgt();

private:
    Ui::PageAboutWgtClass *m_ui;
	char* ximc_ver;

private slots:
	void OnRemoveAllConfigsBtnClicked();

signals:
	void exit_on_remove();
};

#endif // PAGEABOUTWGT_H
