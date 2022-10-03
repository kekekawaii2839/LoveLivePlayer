#include "lvideowidget.h"
#include "ui_lvideowidget.h"

LVideoWidget::LVideoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LVideoWidget)
{
    ui->setupUi(this);

    ui->pushButton_close->setDefault(false);

    connect(ui->pushButton_close,SIGNAL(clicked(bool)),this,SLOT(on_pushButton_close_clicked()));
}

LVideoWidget::~LVideoWidget()
{
    delete ui;
}

void LVideoWidget::paintEvent(QPaintEvent *e){//解决继承后qss失效的问题
    QWidget::paintEvent(e);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void LVideoWidget::on_pushButton_close_clicked(){
    ui->playWidget->setUrl("");
    ui->playWidget->close();
    emit closed();
    this->close();
}

void LVideoWidget::setPos(int pos){
    //ui->playWidget->thread->setValue();
}

void LVideoWidget::on_pushButton_play_clicked()
{
    if(ui->playWidget->status()==1) ui->playWidget->pause();
    else if(ui->playWidget->status()==0) ui->playWidget->next();
}

void LVideoWidget::resizeEvent(QResizeEvent *e){
    ui->playWidget->setGeometry(100,50,width()-200,height()-100);
    ui->pushButton_play->setGeometry((width()-80)/2,height()-40,80,40);
    ui->pushButton_close->setGeometry(width()-80,20,60,60);
}
