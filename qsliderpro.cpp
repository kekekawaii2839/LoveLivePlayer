#include "qsliderpro.h"
#include <QMouseEvent>

QSliderPro::QSliderPro(QWidget* parent)
{
    this->setParent(parent);
    isHorizontal=true;
    Interval=150;
}

QSliderPro::~QSliderPro(){

}

void QSliderPro::mouseReleaseEvent(QMouseEvent *event){
    QSlider::mouseReleaseEvent(event);
    double pos;
    if(isHorizontal==true){
        pos=event->pos().x()*1.0/double(width());
    }
    else{
        pos=1.0-(event->pos().y()*1.0/double(height()));
    }
    setValue(pos*(maximum()-minimum())-minimum());
    emit clicked();
}

void QSliderPro::setInterval(int ms){
    Interval=ms;
}

void QSliderPro::setValue(int value){
    if(value>=this->value()+Interval){
        QSlider::setValue(value);
    }
    else if(value<this->value()){
        QSlider::setValue(value);
    }
}
