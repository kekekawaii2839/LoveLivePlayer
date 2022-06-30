#pragma execution_character_set("utf-8")
#include "songinfo.h"

SongInfo::SongInfo(QString addr,HWND a)//addr是音频文件的完整绝对路径
{
    /*QTextCodec* codec=QTextCodec::codecForName("");//设置编码
    QTextCodec::setCodecForLocale(codec);*/

    hwnd=a;
    QFileInfo j(addr);
    if(j.isFile()){
        AudioAddr=addr;
        CoverAddr=addr;
        CoverAddr.replace(".mp3",".png").replace("/songs/","/infos/");
        /*QString _t=addr;
        QFileInfo jj(_t.replace(".mp3",".info").replace("/songs/","/infos/"));
        //qDebug()<<jj.absoluteFilePath();
        if(jj.isFile()){//info文件存在则读取
            QFile info(_t);
            if(info.open(QIODevice::ReadOnly|QIODevice::Text)){
                QTextStream ss(&info);
                QStringList content;
                while(!ss.atEnd()){
                    QString line=ss.readLine();
                    content.append(line);
                }

                title=content.at(0);
                artist=content.at(1);
                album=content.at(2);
                CoverAddr=content.at(3);
                MvAddr=content.at(4);

                QFileInfo pic(getRealCoverAddr());
                //qDebug()<<"RealCoverAddr:"<<getRealCoverAddr()<<"\nyes?"<<pic.isFile();
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
        }*/
        QFileInfo pic(getRealCoverAddr());
        get_meta(!pic.isFile());
        QString tt=addr;
        QFileInfo mv(tt.replace(".mp3",".mp4").replace("/songs/","/mv/"));
        if(mv.isFile()) MvAddr=tt;
        else MvAddr="none";
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
    if(isNeedAlbumCover){
        SaveHDCToFile(id3_info,hwnd);//这个函数不能用在读取标题等text数据前 否则将导致以上数据出现随机乱码
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
    picFormat=QString::fromWCharArray(pic.MIMEType);
    qDebug()<<AudioAddr<<pic.CanDrawPicture;
    //if(pic.CanDrawPicture==0) qDebug()<<QString::fromWCharArray(zplayer->GetErrorW());
    zplayer->DrawBitmapToHDC(memDc,0,0,id3_info.Picture.Width,id3_info.Picture.Height,id3_info.Picture.hBitmap);
    zplayer->Close();
    zplayer->Release();

    QPixmap pixmap=QtWin::fromHBITMAP(hBmp,QtWin::HBitmapPremultipliedAlpha);
    QImage img=pixmap.toImage();
    if(picFormat=="image/png"&&pic.CanDrawPicture==1) img.save(getRealCoverAddr(),"PNG",100);
    else if(picFormat=="image/jpeg"&&pic.CanDrawPicture==1) img.save(getRealCoverAddr(),"JPG",100);
    SelectObject(memDc,(HBITMAP)NULL);
    DeleteDC(memDc);
    DeleteObject(hBmp);
    DeleteDC(hdc);
}

/*void SongInfo::writeInfo(){//舍弃.info文件 改为全部由get_meta函数获取元数据
    QString temp=AudioAddr;
    QFile jj(temp.replace(".mp3",".info").replace("/songs/","/infos/"));
    if(jj.open(QIODevice::WriteOnly|QIODevice::Text)){
        QString temp=title+"\n"+artist+"\n"+album+"\n"+CoverAddr+"\n"+MvAddr+"\n";
        jj.write(temp.toLocal8Bit());
    }
    jj.close();
}*/

QString SongInfo::getRealCoverAddr(){
    QString c=getCoverAddr();
    c=c.left(c.length()-4);
    c.replace(QApplication::applicationDirPath()+"/infos/","");
    QByteArray cc=c.toLocal8Bit();
    cc=cc.toBase64();
    if(cc.contains("/")) cc.replace("/","");//避免base64加密结果出现"/"导致文件路径出错
    QString ccc;
    if(picFormat=="image/png") ccc=QApplication::applicationDirPath()+"/infos/"+cc+".png";
    else if(picFormat=="image/jpeg") ccc=QApplication::applicationDirPath()+"/infos/"+cc+".jpg";
    return ccc;
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
