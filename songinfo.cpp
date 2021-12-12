#pragma execution_character_set("utf-8")
#include "songinfo.h"

SongInfo::SongInfo(QString addr,HWND a)//addr是音频文件的完整绝对路径
{
    QTextCodec* codec=QTextCodec::codecForName("UTF-8");//设置编码
    QTextCodec::setCodecForLocale(codec);

    hwnd=a;
    QFileInfo j(addr);
    if(j.isFile()){
        AudioAddr=addr;
        CoverAddr=addr;
        CoverAddr.replace(".mp3",".png").replace("/songs/","/infos/");
        QString _t=addr;
        QFileInfo jj(_t.replace(".mp3",".info").replace("/songs/","/infos/"));
        qDebug()<<jj.absoluteFilePath();
        if(jj.isFile()){//info文件存在则读取
            QFile info(_t);
            if(info.open(QIODevice::ReadOnly|QIODevice::Text)){
                QString t=codec->toUnicode(info.readAll());
                qDebug()<<"t:"<<QString::fromUtf8(t.toUtf8())<<"\n|||\n";
                QStringList content=t.split("\n");
                for(int i=0;i<content.count();++i){
                    if(content.at(i).contains("\r")) content[i].replace("\r","");
                }
                title=content.at(0);
                artist=content.at(1);
                album=content.at(2);
                CoverAddr=content.at(3);
                MvAddr=content.at(4);

                QFileInfo pic(getRealCoverAddr());
                get_meta(!pic.isFile());
            }
            else{
                QFileInfo pic(getRealCoverAddr());
                get_meta(!pic.isFile());
                QString tt=addr;
                QFileInfo mv(tt.replace(".mp3",".mp4").replace("/songs/","/mv/"));
                if(mv.isFile()) MvAddr=tt;
                else MvAddr="none";
                writeInfo();
            }
            info.close();
        }
        else{//info文件不存在则写入
            QFileInfo pic(getRealCoverAddr());
            get_meta(!pic.isFile());
            QString tt=addr;
            QFileInfo mv(tt.replace(".mp3",".mp4").replace("/songs/","/mv/"));
            if(mv.isFile()) MvAddr=tt;
            else MvAddr="none";
            writeInfo();
        }
    }
    else{
        AudioAddr="error";
        title=artist=album=CoverAddr=MvAddr="";
    }
}

void SongInfo::get_meta(bool isNeedAlbumCover){
    title="unknown";
    artist="unknown";
    album="unknown";
    zplayer=libZPlay::CreateZPlay();
    libZPlay::TID3InfoExW id3_info;
    wchar_t* wstr=new wchar_t[1024];
    for(int i=0;i<1024;++i){
        wstr[i]=0;
    }
    AudioAddr.toWCharArray(wstr);
    if(zplayer->OpenFileW(wstr,libZPlay::sfAutodetect)){
        if(zplayer->LoadID3ExW(&id3_info,1)){
            if(isNeedAlbumCover){
                SaveHDCToFile(id3_info,hwnd);
            }
            title=QString::fromWCharArray(id3_info.Title);
            artist=QString::fromWCharArray(id3_info.Artist);
            album=QString::fromWCharArray(id3_info.Album);
        }
        else{
            qDebug()<<"not a ID3";
        }
    }
    else{
        qDebug()<<"zplayer can't open this file!";
    }
    if(!isNeedAlbumCover){
        zplayer->Close();
        zplayer->Release();
    }
}

void SongInfo::SaveHDCToFile(libZPlay::TID3InfoExW id3_info,HWND hwnd){
    libZPlay::TID3PictureW pic;
    HDC hdc=GetDC(hwnd);
    HDC memDc=CreateCompatibleDC(hdc);
    HBITMAP hBmp=CreateCompatibleBitmap(hdc,id3_info.Picture.Width,id3_info.Picture.Height);
    SelectObject(memDc,hBmp);

    pic=id3_info.Picture;
    zplayer->DrawBitmapToHDC(memDc,0,0,id3_info.Picture.Width,id3_info.Picture.Height,id3_info.Picture.hBitmap);
    zplayer->Close();
    zplayer->Release();

    QPixmap pixmap=QtWin::fromHBITMAP(hBmp,QtWin::HBitmapNoAlpha);
    QImage img=pixmap.toImage();
    img.save(getRealCoverAddr(),"PNG",100);
    SelectObject(memDc,(HBITMAP)NULL);
    DeleteDC(memDc);
    DeleteObject(hBmp);
    DeleteDC(hdc);
}

void SongInfo::writeInfo(){
    QTextCodec* codec=QTextCodec::codecForName("UTF-8");//设置编码
    QTextCodec::setCodecForLocale(codec);
    QFile jj(AudioAddr.replace(".mp3",".info").replace("/songs/","/infos/"));
    if(jj.open(QIODevice::WriteOnly|QIODevice::Text)){
        QString temp=title+"\n"+artist+"\n"+album+"\n"+CoverAddr+"\n"+MvAddr+"\n";
        jj.write(codec->fromUnicode(temp));
    }
    jj.close();
}

QString SongInfo::getRealCoverAddr(){
    QString c=getCoverAddr();
    c.replace(".png","");
    c.replace(QApplication::applicationDirPath()+"/infos/","");
    c=c.toLocal8Bit().toBase64();
    c=QApplication::applicationDirPath()+"/infos/"+c+".png";
    return c;
}

QString SongInfo::getCoverAddr(){
    return CoverAddr;
}

QString SongInfo::Ltitle(){
    return title;
}

QString SongInfo::Lartist(){
    return artist;
}

QString SongInfo::Lalbum(){
    return album;
}

QString SongInfo::LAudioAddr(){
    return AudioAddr;
}
