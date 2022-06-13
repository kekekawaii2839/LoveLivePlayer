#include "qmaskwidget.h"
#include "ui_qmaskwidget.h"

QMaskWidget::QMaskWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QMaskWidget)
{
    ui->setupUi(this);
    this->setGeometry(0,150,parent->width(),parent->height()-150);//150是标题栏的高度
    //setAttribute(Qt::AA_SynthesizeMouseForUnhandledTabletEvents);
}

QMaskWidget::~QMaskWidget()
{
    delete ui;
}

void QMaskWidget::paintEvent(QPaintEvent *e){//解决继承后qss失效的问题
    QWidget::paintEvent(e);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
