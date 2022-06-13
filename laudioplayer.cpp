#include "laudioplayer.h"

LAudioPlayer::LAudioPlayer(QObject *parent)
{
    this->setParent(parent);
    zplayer=libZPlay::CreateZPlay();
    playlist=new QMediaPlaylist();
    last_duration=-1;
    duration=-1;

    timer=new QTimer();
    timer->setInterval(NotifyInterval);
    //zplayer->SetRate(101);//不可等于100
    connect(timer,SIGNAL(timeout()),this,SLOT(LUpdate()));
    connect(playlist,SIGNAL(currentMediaChanged(QMediaContent)),this,SLOT(LSetMedia(QMediaContent)));
    connect(playlist,SIGNAL(currentIndexChanged(int)),this,SLOT(setPlayerIndex(int)));

    qDebug()<<"create LAudioPlayer successfully!";
}

void LAudioPlayer::LPlay(){
    //qDebug()<<"LPlay()0";
    //this->play();
    zplayer->Play();
    if(timer->isActive()==false) timer->start();
    L_State=QMediaPlayer::PlayingState;
    emit stateChanged(L_State);

    //qDebug()<<"LPlay()1!";
}

void LAudioPlayer::LPause(){
    //this->pause();
    zplayer->Pause();
    if(timer->isActive()==true) timer->stop();
    L_State=QMediaPlayer::PausedState;
    emit stateChanged(L_State);

    //qDebug()<<"LPause()!";
}

void LAudioPlayer::LStop(){
    //this->stop();
    zplayer->Stop();
    if(timer->isActive()==true) timer->stop();
    L_State=QMediaPlayer::StoppedState;
    emit stateChanged(L_State);

    //qDebug()<<"LStop()!";
}

void LAudioPlayer::LSetMedia(const QMediaContent &media){
    //this->setMedia(media,stream);
    zplayer->Close();
    QString fileUrl=media.canonicalUrl().toString();
    wchar_t* wstr=new wchar_t[1024];
    for(int i=0;i<1024;++i){
        wstr[i]=0;
    }
    fileUrl.toWCharArray(wstr);
    zplayer->OpenFileW(wstr,libZPlay::sfAutodetect);
    this->LPlay();

    //qDebug()<<"LSetMedia()!";
}

void LAudioPlayer::LSetPlaylist(QMediaPlaylist *temp_playlist){
    playlist=temp_playlist;
    this->LSetMedia(playlist->currentMedia());
    this->LPlay();

    //qDebug()<<"LSetPlaylist()!";
}

void LAudioPlayer::LUpdate(){
    timer->setInterval(NotifyInterval);
    timer->start();
    libZPlay::TStreamTime pos;
    zplayer->GetPosition(&pos);
    emit positionChanged(pos.ms);
    //qDebug()<<"pos.ms="<<pos.ms;
    //qDebug()<<"duration="<<duration;

    libZPlay::TStreamStatus status;
    zplayer->GetStatus(&status);
    //qDebug()<<status.fPlay<<status.fPause;

    if((pos.ms+51>=duration&&duration!=0)||(status.fPlay==0&&duration!=0&&pos.ms==0)){//zplayer的pos最小间隔貌似是50ms,而且最后一个应该更新的pos反而不更新
        qDebug()<<"end!";
        last_duration=0;
        duration=0;
        L_MediaStatus=QMediaPlayer::EndOfMedia;
        emit mediaStatusChanged(L_MediaStatus);
    }
    /*if(duration==-1){//一开始CurrentIndex=-1
        qDebug()<<"duration==-1";
        playlist->setCurrentIndex(0);
        this->LSetMedia(playlist->currentMedia());
        this->LPlay();
    }*/
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

QMediaContent LAudioPlayer::LCurrentMedia(){
    return this->playlist->currentMedia();
}

void LAudioPlayer::LSetVolume(int v){
    //zplayer->SetPlayerVolume(v,v);//有卡顿
    zplayer->SetMasterVolume(v,v);
}

void LAudioPlayer::setPlayerIndex(int index){
    //qDebug()<<"setPlayerIndex()!";
    this->LSetMedia(playlist->media(index));
}

void LAudioPlayer::LSetPosition(int pos){
    libZPlay::TStreamTime currentTime;
    zplayer->GetPosition(&currentTime);

    libZPlay::TStreamTime setTime;

    if(pos>currentTime.ms){
        setTime.ms=pos-currentTime.ms;
        zplayer->Seek(libZPlay::tfMillisecond,&setTime,libZPlay::smFromCurrentForward);
    }
    else if(pos<currentTime.ms){
        setTime.ms=currentTime.ms-pos;
        zplayer->Seek(libZPlay::tfMillisecond,&setTime,libZPlay::smFromCurrentBackward);
    }
    else return;

    emit positionChanged(pos);

    //qDebug()<<"LSetPosition()!"<<"  pos="<<pos<<"  currentTime.ms="<<currentTime.ms<<"  setTime.ms="<<setTime.ms;
}
