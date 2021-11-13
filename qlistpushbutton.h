#ifndef QLISTPUSHBUTTON_H
#define QLISTPUSHBUTTON_H

#include <QPushButton>

class QListPushButton:public QPushButton
{
    Q_OBJECT
public:
    QListPushButton(QWidget* parent=nullptr);
    int seq;
    bool isRightClicked;
    QString ori_stylesheet;
    void setStyleSheet(const QString &styleSheet,int mode);
    void setStyleSheet(const QString &styleSheet);//重载 防止QtCreator自动生成ui_mainwindow.h时报错
protected:
    void mousePressEvent(QMouseEvent *e);
    bool eventFilter(QObject *watched, QEvent *event);
signals:
    clicked(int);
};

#endif // QLISTPUSHBUTTON_H
