#ifndef QTITLEWIDGET_H
#define QTITLEWIDGET_H

#include <QWidget>

class QTitleWidget:public QWidget
{
    Q_OBJECT
public:
    QTitleWidget(QWidget* parent=nullptr);
protected:
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void paintEvent(QPaintEvent* e);
    int state=0;//0表示未被按下 1表示正在拖拽
    QPoint press_pos,ori_pos;
};

#endif // QTITLEWIDGET_H
