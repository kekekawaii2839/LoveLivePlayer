#ifndef ANIMATEDSCROLLBAR_H
#define ANIMATEDSCROLLBAR_H

#include <QScrollArea>
#include <QScrollBar>
#include <QWheelEvent>
#include <QPropertyAnimation>

class AnimatedScrollBar:public QScrollBar
{
public:
    AnimatedScrollBar(QWidget* parent=nullptr);
protected:
    void wheelEvent(QWheelEvent* event);
    //void paintEvent(QPaintEvent *e);
private:
    //QPropertyAnimation* ScrollBarAnime;
};

#endif // ANIMATEDSCROLLBAR_H
