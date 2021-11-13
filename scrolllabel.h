#ifndef SCROLLLABEL_H
#define SCROLLLABEL_H

#include <QLabel>
#include <QTimerEvent>

class ScrollLabel:public QLabel
{
    Q_OBJECT
public:
    ScrollLabel(QWidget* parent=nullptr);
    void setInterVal(int nInterVal){
        this->killTimer(timerid);//一定要killTimer!!! 否则多个timer同时计时 会大大增加timerEvent的触发频率
        timerid=this->startTimer(nInterVal);
    }

protected:
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);
private:
    //int m_nCharWidth=0;
    int m_nCurrentIndex=0;//文本起始x点
    int m_nTextWidth=0;
    QString m_strText;
    QString m_strDrawText;
    int timerid;
};

#endif // SCROLLLABEL_H
