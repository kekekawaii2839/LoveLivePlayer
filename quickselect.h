#ifndef QUICKSELECT_H
#define QUICKSELECT_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QPoint>
#include <QPainter>
#include <QPaintEvent>
#include <QDesktopWidget>
#include <QDebug>

namespace Ui {
class QuickSelect;
}

class QuickSelect : public QMainWindow
{
    Q_OBJECT

public:
    explicit QuickSelect(QWidget *parent = 0);
    ~QuickSelect();
    Ui::QuickSelect *ui;
    QString color;
    QStringList themes_addr;
    int selected=-1;

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void restore();

private:

};

#endif // QUICKSELECT_H
