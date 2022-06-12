#include "qmaskwidget.h"
#include "ui_qmaskwidget.h"

QMaskWidget::QMaskWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QMaskWidget)
{
    ui->setupUi(this);
    this->setGeometry(x(),y(),parent->width(),parent->height());
    setAttribute(Qt::AA_SynthesizeMouseForUnhandledTabletEvents);
}

QMaskWidget::~QMaskWidget()
{
    delete ui;
}
