#include "qlistpushbutton.h"
#include <QMouseEvent>
#include <QDebug>

QListPushButton::QListPushButton(QWidget* parent)
{
    this->setParent(parent);
    this->setAttribute(Qt::WA_Hover,true);
    this->installEventFilter(this);
    this->ori_stylesheet="";
    seq=-1;
}

void QListPushButton::mousePressEvent(QMouseEvent *e){
    QPushButton::mousePressEvent(e);
    emit clicked(seq);
}

bool QListPushButton::eventFilter(QObject *watched, QEvent *event){
    if(watched==this){
        if(event->type()==QEvent::HoverEnter){
            QString temp=this->ori_stylesheet;
            temp.replace("background-color:rgba(0,0,0,0);","background-color:rgb(245,245,245);");
            temp.replace("border-color:rgba(0,0,0,0);","border:0px;");
            this->setStyleSheet(temp,0);
            this->setDefault(false);
            return true;
        }
        else if(event->type()==QEvent::HoverLeave){
            this->setStyleSheet(ori_stylesheet,0);
            return true;
        }
    }
    return QPushButton::eventFilter(watched,event);
}

void QListPushButton::setStyleSheet(const QString &styleSheet,int mode){
    if(mode==0){//不保存
        QPushButton::setStyleSheet(styleSheet);
    }
    else if(mode==1){//保存
        QPushButton::setStyleSheet(styleSheet);
        ori_stylesheet=styleSheet;
    }
}
