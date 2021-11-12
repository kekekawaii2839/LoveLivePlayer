#include "laudioplayer.h"

LAudioPlayer::LAudioPlayer(QObject *parent)
{
    this->setParent(parent);
    zplayer=libZPlay::CreateZPlay();
    playlist=new QMediaPlaylist();
    last_duration=-1;
    duration=-1;
    //this->setMuted(true);

    timer=new QTimer();
    timer->setInterval(NotifyInterval);
    connect(timer,SIGNAL(timeout()),this,SLOT(LUpdate()));
    connect(playlist,SIGNAL(currentMediaChanged(QMediaContent)),this,SLOT(LSetMedia(QMediaContent)));

    qDebug()<<"create LAudioPlayer successfully!";
}

void LAudioPlayer::LPlay(){
    //this->play();
    zplayer->Play();
    if(timer->isActive()==false) timer->start();
    L_State=QMediaPlayer::PlayingState;
    emit stateChanged(L_State);

    qDebug()<<"LPlay()!";
}

void LAudioPlayer::LPause(){
    //this->pause();
    zplayer->Pause();
    if(timer->isActive()==true) timer->stop();
    L_State=QMediaPlayer::PausedState;
    emit stateChanged(L_State);

    qDebug()<<"LPause()!";
}

void LAudioPlayer::LStop(){
    //this->stop();
    zplayer->Stop();
    if(timer->isActive()==true) timer->stop();
    L_State=QMediaPlayer::StoppedState;
    emit stateChanged(L_State);

    qDebug()<<"LStop()!";
}

void LAudioPlayer::LSetMedia(const QMediaContent &media){
    //this->setMedia(media,stream);
    zplayer->Close();
    QString fileUrl=media.canonicalUrl().toString();
    zplayer->OpenFile(fileUrl.toLocal8Bit(),libZPlay::sfAutodetect);

    qDebug()<<"LSetMedia()!";
}

void LAudioPlayer::LSetPlaylist(QMediaPlaylist *temp_playlist){
    playlist=temp_playlist;
    this->LSetMedia(playlist->currentMedia());
    this->LPlay();

    qDebug()<<"LSetPlaylist()!";
}

void LAudioPlayer::LUpdate(){
    timer->setInterval(NotifyInterval);
    timer->start();
    libZPlay::TStreamTime pos;
    zplayer->GetPosition(&pos);
    emit positionChanged(pos.ms);
    //qDebug()<<"pos.ms="<<pos.ms;
    //qDebug()<<"duration="<<duration;
    if(pos.ms+100>=duration&&duration!=0){//zplayer的pos最小间隔貌似是50ms,而且最后一个应该更新的pos反而不更新
        qDebug()<<"end!";
        last_duration=0;
        duration=0;
        L_MediaStatus=QMediaPlayer::EndOfMedia;
        emit mediaStatusChanged(L_MediaStatus);
    }
    if(duration==-1){//一开始CurrentIndex=-1
        playlist->setCurrentIndex(playlist->nextIndex());
        this->LSetMedia(playlist->currentMedia());
        this->LPlay();
    }

    zplayer->GetStreamInfo(&info);
    duration=info.Length.ms;
    if(last_duration!=duration){
        last_duration=duration;
        emit durationChanged(duration);
    }

    //qDebug()<<"LUpdate()!";
}

QMediaPlayer::State LAudioPlayer::LState(){
    return L_State;
}
