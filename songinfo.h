#ifndef SONGINFO_H
#define SONGINFO_H

#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

#include <windows.h>
#include <QtWinExtras/QtWin>
#include <gdiplus.h>
#include "libzplay.h"

class SongInfo
{
public:
    SongInfo(QString,HWND);
    QString title,artist,album;
    QString CoverAddr;
    QString AudioAddr;
    QString MvAddr;
    HWND hwnd;
    libZPlay::ZPlay* zplayer;

private slots:
    void get_meta(bool);
    void SaveHDCToFile(libZPlay::TID3InfoExW,HWND);
    void writeInfo();
};

#endif // SONGINFO_H
