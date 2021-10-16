#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QStringList>

class config
{
public:
    config();
    QString id;
    QString theme_color;
    QString title;
    int num;
    QString icon_addr;
    QStringList member;
};

#endif // CONFIG_H
