#include "qlistpushbutton.h"

QListPushButton::QListPushButton(QWidget* parent)
{
    this->setParent(parent);
    this->setAttribute(Qt::WA_Hover,true);
    this->installEventFilter(this);
    //this->ori_stylesheet="";
    seq=-1;
    isRightClicked=false;
}

void QListPushButton::mousePressEvent(QMouseEvent *e){
    QPushButton::mousePressEvent(e);
    if(e->button()==Qt::RightButton) isRightClicked=true;
    emit clicked(seq);
}

bool QListPushButton::eventFilter(QObject *watched, QEvent *event){
    if(watched==this){
        if(event->type()==QEvent::HoverEnter){
            emit hoverEnter(seq);
        }
        else if(event->type()==QEvent::HoverLeave){
            emit hoverLeave(seq);
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
    QPushButton::setStyleSheet("QListPushButton:hover{background-color:rgb(245,245,245);border:0px;border-radius:5px;}\nQListPushButton{"+styleSheet+"}");
}

void QListPushButton::leftClick(){
    isRightClicked=false;
    emit clicked(seq);
}
