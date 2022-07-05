#include "scrolllabel.h"

#include <QPainter>
#include <QDebug>

ScrollLabel::ScrollLabel(QWidget* parent)
{
    this->setParent(parent);
    timerid=this->startTimer(40);
}

void ScrollLabel::paintEvent(QPaintEvent *event){
    Q_UNUSED(event)
    QPainter painter(this);
    QRectF rectText=rect();
    rectText.setWidth(width()+m_nTextWidth);
    rectText.setX(-m_nCurrentIndex);
    painter.drawText(rectText,alignment(),m_strDrawText);
}

void ScrollLabel::timerEvent(QTimerEvent *event){
    Q_UNUSED(event)
    if(text().isEmpty()) return;
    if(fontMetrics().width(text())<width()){//无需滚动的时候则是普通的Label
        m_strText=text();
        m_strDrawText=text();
        m_nCurrentIndex=0;
        update(rect());
        return;
    }

    if(m_strText!=text()){
        m_nCurrentIndex=0;
        m_strDrawText="";
        m_strText=text();
        m_nTextWidth=fontMetrics().width(m_strText);
    }

    while(fontMetrics().width(m_strDrawText)<width()+m_nTextWidth){
        m_strDrawText+=m_strText;
    }

    m_nCurrentIndex+=1;
    if(m_nCurrentIndex==m_nTextWidth){
        m_nCurrentIndex=0;
        m_strDrawText="";
    }

    update(rect());
}

void ScrollLabel::setText(const QString text){
    QLabel::setText(text);
    update(rect());//解决更换文字时不刷新的bug
}
