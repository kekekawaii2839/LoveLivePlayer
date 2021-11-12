#ifndef LAUDIOPLAYER_H
#define LAUDIOPLAYER_H

#include <QMultimedia>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QDebug>
#include <QTimer>

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
    int NotifyInterval=1;//默认10ms一刷新

    libZPlay::TStreamInfo info;
    QMediaPlayer::State L_State;//zplayer->GetStatus();?
    QMediaPlayer::MediaStatus L_MediaStatus;
    int duration,last_duration;
public Q_SLOTS:
    void LPlay();
    void LPause();
    void LStop();

    void LSetMedia(const QMediaContent &media);
    void LSetPlaylist(QMediaPlaylist *temp_playlist);

    void LUpdate();
    State LState();
};

#endif // LAUDIOPLAYER_H
