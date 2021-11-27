#ifndef ANIMATEDSCROLLAREA_H
#define ANIMATEDSCROLLAREA_H

#include <QScrollArea>
#include <QScrollBar>
#include <QWheelEvent>
#include <QPropertyAnimation>

class AnimatedScrollArea:public QScrollArea
{
public:
    AnimatedScrollArea(QWidget* parent=nullptr);
protected:
    void wheelEvent(QWheelEvent* event);
    //void paintEvent(QPaintEvent *e);
private:
    //QPropertyAnimation* ScrollBarAnime;
};

#endif // ANIMATEDSCROLLAREA_H
