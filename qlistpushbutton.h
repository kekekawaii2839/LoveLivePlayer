#ifndef QLISTPUSHBUTTON_H
#define QLISTPUSHBUTTON_H

#include <QPushButton>
#include <QStyleOption>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>

class QListPushButton:public QPushButton
{
    Q_OBJECT
public:
    QListPushButton(QWidget* parent=nullptr);
    int seq;
    bool isRightClicked;
    //QString ori_stylesheet;
    //void setStyleSheet(const QString &styleSheet,int mode);
    void setStyleSheet(const QString &styleSheet);//重载 防止QtCreator自动生成ui_mainwindow.h时报错
    void leftClick();
    void setHover(bool);
protected:
    void mousePressEvent(QMouseEvent *e);
    bool eventFilter(QObject *watched, QEvent *event);
    //void paintEvent(QPaintEvent* e);
private:
    bool isEnableHover;
signals:
    clicked(int);
    dblclicked(int);
    hoverEnter(int);
    hoverLeave(int);
};

#endif // QLISTPUSHBUTTON_H
