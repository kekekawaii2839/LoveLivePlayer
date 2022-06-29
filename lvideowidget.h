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
    Ui::LVideoWidget *ui;

    void setPos(int pos);

protected:
    void paintEvent(QPaintEvent* e);

private slots:
    void on_pushButton_close_clicked();

    void on_pushButton_play_clicked();

private:

signals:
    closed();
};

#endif // LVIDEOWIDGET_H
