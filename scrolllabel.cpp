#include "scrolllabel.h"

#include <QPainter>
#include <QDebug>

ScrollLabel::ScrollLabel(QWidget* parent)
{
    this->setParent(parent);
    timerid=this->startTimer(10);
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
        m_strDrawText=text();
        m_nCurrentIndex=0;
        return;
    }

    if(m_strText!=text()){
        m_strText=text();
        m_nCurrentIndex=0;
        m_strDrawText="";
        m_nTextWidth=fontMetrics().width(m_strText);
    }

    while(fontMetrics().width(m_strDrawText)<width()+m_nTextWidth){
        m_strDrawText+=m_strText;
    }

    m_nCurrentIndex++;
    if(m_nCurrentIndex==m_nTextWidth){
        m_nCurrentIndex=0;
        m_strDrawText="";
    }

    update();
}
