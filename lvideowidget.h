#ifndef LVIDEOWIDGET_H
#define LVIDEOWIDGET_H

#include <QWidget>

namespace Ui {
class LVideoWidget;
}

class LVideoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LVideoWidget(QWidget *parent = 0);
    ~LVideoWidget();

private slots:
    void on_btnOpen_clicked();

private:
    Ui::LVideoWidget *ui;
};

#endif // LVIDEOWIDGET_H
