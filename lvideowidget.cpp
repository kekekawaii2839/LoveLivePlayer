#pragma execution_character_set("utf-8")
#include "lvideowidget.h"
#include "ui_lvideowidget.h"

LVideoWidget::LVideoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LVideoWidget)
{
    ui->setupUi(this);

    QStringList urls;
    urls<<"D:/01.mp4";
    ui->cboxUrl->addItems(urls);
    ui->cboxUrl->setCurrentIndex(0);
}

LVideoWidget::~LVideoWidget()
{
    delete ui;
}

void LVideoWidget::on_btnOpen_clicked()
{
    if(ui->btnOpen->text() == "打开") {
        ui->btnOpen->setText("关闭");
        QString url = ui->cboxUrl->currentText().trimmed();
        ui->playWidget->setUrl(url);
        ui->playWidget->open();
    }
    else{
        ui->btnOpen->setText("打开");
        ui->playWidget->close();
    }
}
