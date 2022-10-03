#ifndef SONGINFO_H
#define SONGINFO_H

#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QApplication>
#include <QTextCodec>
#include <QTextStream>

#include <windows.h>
#include <QtWinExtras/QtWin>
#include <gdiplus.h>
#include "libzplay.h"

struct picInfo{
    QString picFormat;
    unsigned int CanDrawPicture;
    unsigned int Width;
    unsigned int Height;
    void* hBitmap;//for temporary
};

class SongInfo
{
public:
    SongInfo(QString,HWND);
    QString getRealCoverAddr();
    QString Ltitle();
    QString Lartist();
    QString Lalbum();
    QString LAudioAddr();

private:
    QString title,artist,album;
    QString CoverAddr;
    QString AudioAddr;
    QString MvAddr;
    HWND hwnd;
    libZPlay::ZPlay* zplayer;
    //libZPlay::TID3PictureW pic;//这玩意存的是地址啊！！！
    picInfo pic;

private slots:
    void get_meta();
    void SaveHDCToFile(picInfo,HWND);
    //void writeInfo();
    QString getCoverAddr();
};

#endif // SONGINFO_H
