#ifndef QSLIDERPRO_H
#define QSLIDERPRO_H

#include <QSlider>

class QSliderPro:public QSlider
{
    Q_OBJECT
public:
    QSliderPro(QWidget* parent=nullptr);
    bool isHorizontal;
    ~QSliderPro(void);

protected:
    void mouseReleaseEvent(QMouseEvent* event);

signals:
    void clicked();
};

#endif // QSLIDERPRO_H
