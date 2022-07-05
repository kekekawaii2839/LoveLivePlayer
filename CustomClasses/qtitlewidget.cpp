#include "qtitlewidget.h"
#include <QMouseEvent>
#include <QPoint>
#include <QStyleOption>
#include <QStyle>
#include <QPainter>
#include <QPaintEvent>

QTitleWidget::QTitleWidget(QWidget* parent)
{
    this->setParent(parent);
    press_pos=QPoint(0,0);
    ori_pos=QPoint(0,0);
}

void QTitleWidget::mousePressEvent(QMouseEvent *e){
    QWidget::mousePressEvent(e);
    this->state=1;
    QWidget* w=window();
    press_pos=e->globalPos();
    ori_pos=w->pos();
}

void QTitleWidget::mouseMoveEvent(QMouseEvent *e){
    if(this->state==1){
        QWidget::mouseMoveEvent(e);

        QWidget* w=window();
        w->move(e->globalPos().x()-press_pos.x()+ori_pos.x(),e->globalPos().y()-press_pos.y()+ori_pos.y());
    }
}

void QTitleWidget::mouseReleaseEvent(QMouseEvent *e){
    QWidget::mouseReleaseEvent(e);
    this->state=0;
    press_pos=QPoint(0,0);
}

void QTitleWidget::mouseDoubleClickEvent(QMouseEvent *e){
    QWidget::mouseDoubleClickEvent(e);
    emit dblclicked();
}

void QTitleWidget::paintEvent(QPaintEvent *e){//解决继承后qss失效的问题
    QWidget::paintEvent(e);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
