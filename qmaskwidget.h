#ifndef QMASKWIDGET_H
#define QMASKWIDGET_H

#include <QWidget>
#include <QPainter>

namespace Ui {
class QMaskWidget;
}

class QMaskWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QMaskWidget(QWidget *parent = 0);
    ~QMaskWidget();

private:
    Ui::QMaskWidget *ui;

protected:
    void paintEvent(QPaintEvent* e);
};

#endif // QMASKWIDGET_H
