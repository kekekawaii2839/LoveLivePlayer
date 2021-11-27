#include "quickselect.h"
#include "ui_quickselect.h"

QuickSelect::QuickSelect(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QuickSelect)
{
    this->setAttribute(Qt::WA_TranslucentBackground,true);
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::SubWindow);//窗口无标题且置顶且不在任务栏中显示
    this->installEventFilter(this);
    setCursor(Qt::BlankCursor);
    for(int i=0;i<6;++i) themes_addr.append("");
}

QuickSelect::~QuickSelect()
{
    delete ui;
}

bool QuickSelect::eventFilter(QObject *watched, QEvent *event){
    //qDebug()<<"eventFilter!"<<event->type();
    if(event->type()==QEvent::HoverMove){
        QMouseEvent *e = static_cast<QMouseEvent *>(event);
        QMainWindow::mouseMoveEvent(e);
        QPoint g=mapToGlobal(QPoint(0,0));
        int x=g.x()+200;
        int y=g.y()+200;
        //qDebug()<<"x:"<<x<<"y:"<<y;
        //qDebug()<<"xe:"<<e->globalX()<<"ye:"<<e->globalY();
        float k=(e->globalY()-y)*1.0/(e->globalX()-x);
        if(y>=e->globalY()){
           if(k>0&&k<=1.732){
               restore();
               selected=3;
               ui->line_3->setStyleSheet("background-color: qlineargradient(spread:pad, x1:1, y1:0, x2:0, y2:1, stop:0.48 rgba(255,255,255,0),stop:0.5 "+color+",stop:0.52 rgba(255,255,255,0));");
               ui->line_3r->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(255,255,255,0),stop:1 "+color+");");
           }
           else if(k<0&&k>=-1.732){
               restore();
               selected=1;
               ui->line_1->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(255,255,255,0),stop:1 "+color+");");
               ui->line_1r->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0.48 rgba(255,255,255,0),stop:0.5 "+color+",stop:0.52 rgba(255,255,255,0));");
           }
           else if(k<-1.732||k>1.732){
               restore();
               selected=2;
               ui->line_2->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0.48 rgba(255,255,255,0),stop:0.5 "+color+",stop:0.52 rgba(255,255,255,0));");
               ui->line_2r->setStyleSheet("background-color: qlineargradient(spread:pad, x1:1, y1:0, x2:0, y2:1, stop:0.48 rgba(255,255,255,0),stop:0.5 "+color+",stop:0.52 rgba(255,255,255,0));");
           }
        }
        else{
            if(k>0&&k<=1.732){
                restore();
                selected=6;
                ui->line_6->setStyleSheet("background-color: qlineargradient(spread:pad, x1:1, y1:0, x2:0, y2:1, stop:0.48 rgba(255,255,255,0),stop:0.5 "+color+",stop:0.52 rgba(255,255,255,0));");
                ui->line_6r->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 "+color+",stop:1 rgba(255,255,255,0));");
            }
            else if(k<0&&k>=-1.732){
                restore();
                selected=4;
                ui->line_4->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 "+color+",stop:1 rgba(255,255,255,0));");
                ui->line_4r->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0.48 rgba(255,255,255,0),stop:0.5 "+color+",stop:0.52 rgba(255,255,255,0));");
            }
            else if(k<-1.732||k>1.732){
                restore();
                selected=5;
                ui->line_5->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0.48 rgba(255,255,255,0),stop:0.5 "+color+",stop:0.52 rgba(255,255,255,0));");
                ui->line_5r->setStyleSheet("background-color: qlineargradient(spread:pad, x1:1, y1:0, x2:0, y2:1, stop:0.48 rgba(255,255,255,0),stop:0.5 "+color+",stop:0.52 rgba(255,255,255,0));");
            }
        }
    }
    else if(event->type()==QEvent::MouseButtonRelease) this->hide();

    return QMainWindow::eventFilter(watched,event);
}

void QuickSelect::restore(){
    ui->line_1->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(255,255,255,0),stop:1 rgba(100, 100, 100, 155));");
    ui->line_1r->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0.48 rgba(255,255,255,0),stop:0.5 rgba(100, 100, 100, 155),stop:0.52 rgba(255,255,255,0));");
    ui->line_2->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0.48 rgba(255,255,255,0),stop:0.5 rgba(100, 100, 100, 155),stop:0.52 rgba(255,255,255,0));");
    ui->line_2r->setStyleSheet("background-color: qlineargradient(spread:pad, x1:1, y1:0, x2:0, y2:1, stop:0.48 rgba(255,255,255,0),stop:0.5 rgba(100, 100, 100, 155),stop:0.52 rgba(255,255,255,0));");
    ui->line_3->setStyleSheet("background-color: qlineargradient(spread:pad, x1:1, y1:0, x2:0, y2:1, stop:0.48 rgba(255,255,255,0),stop:0.5 rgba(100, 100, 100, 155),stop:0.52 rgba(255,255,255,0));");
    ui->line_3r->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(255,255,255,0),stop:1 rgba(100, 100, 100, 155));");
    ui->line_4->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(100, 100, 100, 155),stop:1 rgba(255,255,255,0));");
    ui->line_4r->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0.48 rgba(255,255,255,0),stop:0.5 rgba(100, 100, 100, 155),stop:0.52 rgba(255,255,255,0));");
    ui->line_5->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0.48 rgba(255,255,255,0),stop:0.5 rgba(100, 100, 100, 155),stop:0.52 rgba(255,255,255,0));");
    ui->line_5r->setStyleSheet("background-color: qlineargradient(spread:pad, x1:1, y1:0, x2:0, y2:1, stop:0.48 rgba(255,255,255,0),stop:0.5 rgba(100, 100, 100, 155),stop:0.52 rgba(255,255,255,0));");
    ui->line_6->setStyleSheet("background-color: qlineargradient(spread:pad, x1:1, y1:0, x2:0, y2:1, stop:0.48 rgba(255,255,255,0),stop:0.5 rgba(100, 100, 100, 155),stop:0.52 rgba(255,255,255,0));");
    ui->line_6r->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(100, 100, 100, 155),stop:1 rgba(255,255,255,0));");
}
