#ifndef LAUDIOPLAYER_H
#define LAUDIOPLAYER_H

#include <QMultimedia>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QDebug>
#include <QTimer>
#include <QFileInfo>

#include <windows.h>
#include <QtWinExtras/QtWin>
#include <gdiplus.h>
#include "libzplay.h"

class LAudioPlayer:public QMediaPlayer
{
    Q_OBJECT
public:
    LAudioPlayer(QObject *parent = Q_NULLPTR);
    QMediaPlaylist* playlist;
private:
    libZPlay::ZPlay* zplayer;
    QTimer* timer;
    int NotifyInterval=1;//默认1ms一刷新

    libZPlay::TStreamInfo info;
    QMediaPlayer::State L_State;//zplayer->GetStatus();?
    QMediaPlayer::MediaStatus L_MediaStatus;
    unsigned int duration,last_duration;
public Q_SLOTS:
    void LPlay();
    void LPause();
    void LStop();

    bool LSetMedia(const QMediaContent &media);
    bool LSetPlaylist(QMediaPlaylist *temp_playlist);
    void LSetVolume(int);
    void LSetPosition(int);

    void LUpdate();
    State LState();
    QMediaContent LCurrentMedia();
    int LPos();

    //void SaveHDCToFile(libZPlay::TID3InfoExW, HWND);
private slots:
    bool setPlayerIndex(int);
signals:
    //AlbumPicReady();
    positionChanged(int);
};

#endif // LAUDIOPLAYER_H
