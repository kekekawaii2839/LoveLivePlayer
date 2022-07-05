#ifndef QCLICKWIDGET_H
#define QCLICKWIDGET_H

#include <QWidget>

class QClickWidget:public QWidget
{
    Q_OBJECT
public:
    QClickWidget(QWidget* parent=nullptr);
    bool is_small;
protected:
    //void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void paintEvent(QPaintEvent* e);
signals:
    clicked();
};

#endif // QCLICKWIDGET_H
