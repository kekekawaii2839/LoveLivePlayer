#include "qsliderpro.h"
#include <QMouseEvent>

QSliderPro::QSliderPro(QWidget* partent)
{
    this->setParent(partent);
    isHorizontal=true;
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
