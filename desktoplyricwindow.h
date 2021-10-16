#ifndef DESKTOPLYRICWINDOW_H
#define DESKTOPLYRICWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QPoint>
#include <QPainter>
#include <QPaintEvent>
#include <QDesktopWidget>

namespace Ui {
class DesktopLyricWindow;
}

class DesktopLyricWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DesktopLyricWindow(QWidget *parent = 0);
    ~DesktopLyricWindow();
    Ui::DesktopLyricWindow *ui;
    bool is_locked;

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    int state=0;//0表示未被按下 1表示正在拖拽
    QPoint press_pos,ori_pos;

private slots:
    void on_pushButton_lock_clicked();

private:
    //Ui::DesktopLyricWindow *ui;
};

#endif // DESKTOPLYRICWINDOW_H
