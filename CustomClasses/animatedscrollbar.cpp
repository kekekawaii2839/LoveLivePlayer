#include "animatedscrollbar.h"

#include <QDebug>

AnimatedScrollBar::AnimatedScrollBar(QWidget* parent)
{
    this->setParent(parent);
}

void AnimatedScrollBar::wheelEvent(QWheelEvent *event){
    int deg=event->delta();//滚动的角度 *8就是滚动距离
    int value=this->value();
    int max=maximum();
    //qDebug()<<"deg="<<deg<<"  value="<<value<<"  max="<<max;

    QPropertyAnimation* ScrollBarAnime=new QPropertyAnimation(this,"value");
    ScrollBarAnime->setDuration(max*0.25);
    ScrollBarAnime->setEasingCurve(QEasingCurve::OutQuad);

    if(ScrollBarAnime->state()==QAbstractAnimation::Stopped){
        ScrollBarAnime->setStartValue(value);
        ScrollBarAnime->setEndValue(value-deg*max*0.001);
        ScrollBarAnime->start();
    }
}
