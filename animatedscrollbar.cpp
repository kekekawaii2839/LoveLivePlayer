#include "animatedscrollbar.h"

#include <QDebug>

AnimatedScrollBar::AnimatedScrollBar(QWidget* parent)
{
    this->setParent(parent);
}

void AnimatedScrollBar::wheelEvent(QWheelEvent *event){
    int deg=event->delta();//滚动的角度 *8就是滚动距离
    int value=this->value();
    //qDebug()<<"deg="<<deg<<"  value="<<value;

    QPropertyAnimation* ScrollBarAnime=new QPropertyAnimation(this,"value");
    ScrollBarAnime->setDuration(700);
    ScrollBarAnime->setEasingCurve(QEasingCurve::OutQuad);

    if(ScrollBarAnime->state()==QAbstractAnimation::Stopped){
        ScrollBarAnime->setStartValue(value);
        ScrollBarAnime->setEndValue(value-deg*2);
        ScrollBarAnime->start();
    }
}
