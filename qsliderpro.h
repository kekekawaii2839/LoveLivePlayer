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

    void setInterval(int ms);//压低刷新频率以减小cpu占用
    void setValue(int value);

protected:
    void mouseReleaseEvent(QMouseEvent* event);

private:
    int Interval;

signals:
    void clicked();
};

#endif // QSLIDERPRO_H
