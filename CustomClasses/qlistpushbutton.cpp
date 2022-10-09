#include "qlistpushbutton.h"

QListPushButton::QListPushButton(QWidget* parent)
{
    this->setParent(parent);
    //this->setAttribute(Qt::WA_Hover,true);
    this->installEventFilter(this);
    //this->ori_stylesheet="";
    seq=-1;
    isRightClicked=false;
    isEnableHover=true;
    isEvenGrey=true;

    /*bottomline=new QWidget(this);
    bottomline->setGeometry(width()*0.05,height()-5,width()*0.9,2);
    bottomline->setStyleSheet("background-color:#000000;border:1px solid black;");
    bottomline->show();
    qDebug()<<"bottomline:"<<bottomline->isVisible();*/

    QPushButton::setStyleSheet("QListPushButton{background-color:#ffffff;}");
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
    if(isEnableHover){
        //if(seq%2==1&&isEvenGrey) QPushButton::setStyleSheet("QListPushButton{background-color:#f5f5f5;border-radius:5px;"+styleSheet+"}QListPushButton:hover{background-color:rgb(245,245,245);}");
        //else QPushButton::setStyleSheet("QListPushButton{background-color:#ffffff;border-radius:5px;"+styleSheet+"}QListPushButton:hover{background-color:rgb(245,245,245);}");
        if(seq%2==1&&isEvenGrey) QPushButton::setStyleSheet("QListPushButton{background-color:#f5f5f5;border-radius:5px;"+styleSheet+"}");
        else QPushButton::setStyleSheet("QListPushButton{background-color:#ffffff;border-radius:5px;"+styleSheet+"}");
    }
    else{
        if(seq%2==1&&isEvenGrey) QPushButton::setStyleSheet("QListPushButton{background-color:#f5f5f5;"+styleSheet+"}");
        else QPushButton::setStyleSheet("QListPushButton{background-color:#ffffff;"+styleSheet+"}");
    }
}

void QListPushButton::leftClick(){
    isRightClicked=false;
    emit clicked(seq);
}

/*void QListPushButton::setHover(bool on){
    if(on){
        isEnableHover=true;
        QString s=styleSheet();
        s.replace("QListPushButton:hover{background-color:rgb(245,245,245);border-radius:5px;}","");
        setStyleSheet(s);
    }
    else{
        isEnableHover=false;
        QString s=styleSheet();
        s.replace("QListPushButton:hover{background-color:rgb(245,245,245);border-radius:5px;}QListPushButton{","QListPushButton{border-bottom:1px solid black;");
        qDebug()<<"s:"<<s;
        setStyleSheet(s);
    }
}*/

/*void QListPushButton::paintEvent(QPaintEvent *e){//解决继承后qss失效的问题
    QPushButton::paintEvent(e);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}*/

void QListPushButton::move(int x,int y,int interval){
    QPropertyAnimation* moveani=new QPropertyAnimation(this,"pos");
    moveani->setDuration(240);
    moveani->setStartValue(pos());
    moveani->setEndValue(QPoint(x,y));
    moveani->setEasingCurve(QEasingCurve::InOutCubic);

    QTimer* t=new QTimer();
    connect(t,&QTimer::timeout,[=](){moveani->start();});
    t->setSingleShot(true);
    t->start(interval);
}

void QListPushButton::setSeq(int s){
    QString temp=styleSheet();
    if(s>=0){
        seq=s;
        if(seq%2==1){
            int i=temp.indexOf("{background-color:#ffffff");
            if(i>=0) temp.replace(i,25,"{background-color:#f5f5f5");
        }
        else{
            int i=temp.indexOf("{background-color:#f5f5f5");
            if(i>=0) temp.replace(i,25,"{background-color:#ffffff");
        }

    }
    else{
        seq=-1;
        int i=temp.indexOf("{background-color:#f5f5f5");
        if(i>=0) temp.replace(i,24,"{background-color:#ffffff");
    }
    if(isEvenGrey) QPushButton::setStyleSheet(temp);
}

int QListPushButton::Seq(){
    return seq;
}
