#ifndef QSMARTTEXTWIDGET_H
#define QSMARTTEXTWIDGET_H

#include <QLabel>
#include <QPushButton>

class QSmartTextLabel : public QLabel
{
    Q_OBJECT

public:
    QSmartTextLabel(QWidget* parent=0);
    QString adjust_text_overlength(QString text,int mode);
    void setText(QString text,int mode);
    void resizeEvent(QResizeEvent* e);

private:
    QString ori_text;
    int Mode;
};

class QSmartTextPushButton : public QPushButton
{
    Q_OBJECT

public:
    QSmartTextPushButton(QWidget* parent=0);

private:
};

#endif // QSMARTTEXTWIDGET_H
