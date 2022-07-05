#include "qclickwidget.h"
#include <QMouseEvent>
#include <QStyleOption>
#include <QStyle>
#include <QPainter>
#include <QPaintEvent>

QClickWidget::QClickWidget(QWidget* parent)
{
    this->setParent(parent);
    is_small=true;
}

void QClickWidget::mouseReleaseEvent(QMouseEvent *e){
    if(is_small==true){
        QWidget::mouseReleaseEvent(e);
        is_small=false;
        emit clicked();
    }
}

void QClickWidget::paintEvent(QPaintEvent *e){//解决继承后qss失效的问题
    QWidget::paintEvent(e);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
