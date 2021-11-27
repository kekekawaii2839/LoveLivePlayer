#include "animatedscrollarea.h"

#include <QDebug>

AnimatedScrollArea::AnimatedScrollArea(QWidget* parent)
{
    this->setParent(parent);
}

void AnimatedScrollArea::wheelEvent(QWheelEvent *event){
    int deg=event->delta();//滚动的角度 *8就是滚动距离
    int value=this->verticalScrollBar()->value();
    //qDebug()<<"deg="<<deg<<"  value="<<value;

    QScrollBar* scrollbar=this->verticalScrollBar();
    QPropertyAnimation* ScrollBarAnime=new QPropertyAnimation(scrollbar,"value");
    ScrollBarAnime->setDuration(700);
    ScrollBarAnime->setEasingCurve(QEasingCurve::OutQuad);

    if(ScrollBarAnime->state()==QAbstractAnimation::Stopped){
        ScrollBarAnime->setStartValue(value);
        ScrollBarAnime->setEndValue(value-deg*2);
        ScrollBarAnime->start();
    }
}
