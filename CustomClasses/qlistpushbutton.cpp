#include "qlistpushbutton.h"

QListPushButton::QListPushButton(QWidget* parent)
{
    this->setParent(parent);
    this->setAttribute(Qt::WA_Hover,true);
    this->installEventFilter(this);
    //this->ori_stylesheet="";
    seq=-1;
    isRightClicked=false;
    isEnableHover=true;
}

void QListPushButton::mousePressEvent(QMouseEvent *e){
    QPushButton::mousePressEvent(e);
    if(e->button()==Qt::RightButton) isRightClicked=true;
    emit clicked(seq);
}

bool QListPushButton::eventFilter(QObject *watched, QEvent *event){
    if(watched==this){
        if(event->type()==QEvent::HoverEnter){
            if(isEnableHover) emit hoverEnter(seq);
        }
        else if(event->type()==QEvent::HoverLeave){
            if(isEnableHover) emit hoverLeave(seq);
        }
        else if(event->type()==QEvent::MouseButtonDblClick){
            emit dblclicked(seq);
        }
    }
    return QPushButton::eventFilter(watched,event);
}

/*void QListPushButton::setStyleSheet(const QString &styleSheet,int mode){
    if(mode==0){//不保存
        QPushButton::setStyleSheet(styleSheet);
    }
    else if(mode==1){//保存
        QPushButton::setStyleSheet(styleSheet);
        ori_stylesheet=styleSheet;
    }
}*/

void QListPushButton::setStyleSheet(const QString &styleSheet){
    if(isEnableHover) QPushButton::setStyleSheet("QListPushButton:hover{background-color:rgb(245,245,245);border:0px;border-radius:5px;}QListPushButton{"+styleSheet+"}");
    else QPushButton::setStyleSheet("QListPushButton{"+styleSheet+"}");
}

void QListPushButton::leftClick(){
    isRightClicked=false;
    emit clicked(seq);
}

void QListPushButton::setHover(bool on){
    if(on){
        isEnableHover=true;
        QString s=styleSheet();
        s.replace("QListPushButton:hover{background-color:rgb(245,245,245);border:0px;border-radius:5px;}","");
        setStyleSheet(s);
    }
    else{
        isEnableHover=false;
        QString s=styleSheet();
        s.replace("QListPushButton:hover{background-color:rgb(245,245,245);border:0px;border-radius:5px;}QListPushButton{","QListPushButton{");
        qDebug()<<"s:"<<s;
        setStyleSheet(s);
    }
}

/*void QListPushButton::paintEvent(QPaintEvent *e){//解决继承后qss失效的问题
    QPushButton::paintEvent(e);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}*/
