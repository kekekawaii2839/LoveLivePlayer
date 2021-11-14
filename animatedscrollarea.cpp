#include "animatedscrollarea.h"

#include <QDebug>

AnimatedScrollArea::AnimatedScrollArea(QWidget* parent)
{
    this->setParent(parent);
    QScrollBar* scrollbar=this->verticalScrollBar();
    ScrollBarAnime=new QPropertyAnimation(scrollbar,"value");
    ScrollBarAnime->setDuration(650);
    ScrollBarAnime->setEasingCurve(QEasingCurve::OutQuad);
}

void AnimatedScrollArea::wheelEvent(QWheelEvent *event){
    int deg=event->delta();//滚动的角度 *8就是滚动距离
    int value=this->verticalScrollBar()->value();
    //qDebug()<<"deg="<<deg<<"  value="<<value;

    if(ScrollBarAnime->state()==QAbstractAnimation::Stopped){
        ScrollBarAnime->setStartValue(value);
        ScrollBarAnime->setEndValue(value-deg*2.7);
        ScrollBarAnime->start();
    }
}
