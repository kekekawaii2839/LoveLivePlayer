#ifndef QMASKWIDGET_H
#define QMASKWIDGET_H

#include <QWidget>

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
};

#endif // QMASKWIDGET_H
