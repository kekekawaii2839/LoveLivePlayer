#include "desktoplyricwindow.h"
#include "ui_desktoplyricwindow.h"

DesktopLyricWindow::DesktopLyricWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DesktopLyricWindow)
{
    this->setAttribute(Qt::WA_TranslucentBackground,true);
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::SubWindow);//窗口无标题且置顶且不在任务栏中显示
    this->installEventFilter(this);
    ui->pushButton_lyric_html->setDefault(false);
    ui->pushButton_lock->setDefault(false);
    ui->pushButton_lock->setVisible(false);

    press_pos=QPoint(0,0);
    ori_pos=QPoint(0,0);
}

DesktopLyricWindow::~DesktopLyricWindow()
{
    delete ui;
}

bool DesktopLyricWindow::eventFilter(QObject *watched, QEvent *event){
    if(watched==this){
        if(event->type()==QEvent::HoverEnter){
            ui->pushButton_lock->setVisible(true);
            if(is_locked==true){
                ui->centralwidget->setStyleSheet("#centralwidget{\nborder:0px solid rgba(0,0,0,50);\nbackground-color:rgba(255,255,255,2);\n}");
            }
            else{
                ui->centralwidget->setStyleSheet("#centralwidget{\nborder:0px solid rgba(0,0,0,50);\nbackground-color:rgba(100,100,100,100);\n}");
            }
        }
        else if(event->type()==QEvent::HoverLeave){
            ui->pushButton_lock->setVisible(false);
            ui->centralwidget->setStyleSheet("#centralwidget{\nborder:0px solid rgba(0,0,0,50);\nbackground-color:rgba(255,255,255,2);\n}");
        }
        else if(event->type()==QEvent::MouseButtonPress){
            QMouseEvent *e = static_cast<QMouseEvent *>(event);
            QMainWindow::mousePressEvent(e);
            this->state=1;
            QWidget* w=window();
            press_pos=e->globalPos();
            ori_pos=w->pos();
        }
        else if(event->type()==QEvent::MouseButtonRelease){
            QMouseEvent *e = static_cast<QMouseEvent *>(event);
            QMainWindow::mouseReleaseEvent(e);
            this->state=0;
            press_pos=QPoint(0,0);
        }
        else if(event->type()==QEvent::MouseMove){
            if(this->state==1&&is_locked==false){
                QMouseEvent *e = static_cast<QMouseEvent *>(event);
                QMainWindow::mouseMoveEvent(e);
                QWidget* w=window();
                int x=e->globalPos().x()-press_pos.x()+ori_pos.x();
                int y=e->globalPos().y()-press_pos.y()+ori_pos.y();
                QDesktopWidget* desktopWidget = QApplication::desktop();
                QRect screenRect = desktopWidget->screenGeometry();
                if(x+this->width()>screenRect.width()) x=screenRect.width()-this->width();
                if(y+this->height()>screenRect.height()) y=screenRect.height()-this->height();
                w->move(x,y);
            }
        }
        else if(event->type()==QEvent::Paint){//使可以动态更新
            QStyleOption opt;
            opt.init(this);
            QPainter p(this);
            style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
        }
        else{
            event->accept();
        }
    }
}

void DesktopLyricWindow::on_pushButton_lock_clicked()
{
    if(is_locked==true){
        is_locked=false;
        ui->pushButton_lock->setStyleSheet("border-image: url(:/new/prefix1/unlocked.png);");
        ui->centralwidget->setStyleSheet("#centralwidget{\nborder:0px solid rgba(0,0,0,50);\nbackground-color:rgba(100,100,100,100);\n}");
    }
    else{
        is_locked=true;
        ui->pushButton_lock->setStyleSheet("border-image: url(:/new/prefix1/locked.png);");
        ui->centralwidget->setStyleSheet("#centralwidget{\nborder:0px solid rgba(0,0,0,50);\nbackground-color:rgba(255,255,255,2);\n}");
    }
}
