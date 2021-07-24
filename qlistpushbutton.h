#ifndef QLISTPUSHBUTTON_H
#define QLISTPUSHBUTTON_H

#include <QPushButton>

class QListPushButton:public QPushButton
{
    Q_OBJECT
public:
    QListPushButton(QWidget* parent=nullptr);
    int seq;
    QString ori_stylesheet;
    void setStyleSheet(const QString &styleSheet,int mode);
protected:
    void mousePressEvent(QMouseEvent *e);
    bool eventFilter(QObject *watched, QEvent *event);
signals:
    clicked(int);
};

#endif // QLISTPUSHBUTTON_H
