#include "songinfo.h"
#include <QTime>

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

        get_meta();//调用完get_meta()以初始化pic之后才能调用getRealCoverAddr(),不然QString::fromWCharArray(pic.MIMEType)将是空字符串,导致getRealCoverAddr()返回空字符串
        QFileInfo picc(getRealCoverAddr());
        if(!picc.isFile()){
            SaveHDCToFile(pic,hwnd);//这个函数不能用在读取标题等text数据前 否则将导致以上数据出现随机乱码
        }
        else{
            zplayer->Close();
            zplayer->Release();
        }

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

void SongInfo::get_meta(){
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
            pic.picFormat=QString::fromStdWString(id3_info.Picture.MIMEType);
            pic.CanDrawPicture=id3_info.Picture.CanDrawPicture;
            pic.Height=id3_info.Picture.Height;
            pic.Width=id3_info.Picture.Width;
            pic.hBitmap=id3_info.Picture.hBitmap;
        }
        else{
            qDebug()<<"not a ID3";
        }
    }
    else{
        qDebug()<<"zplayer can't open this file!";
    }
}

void SongInfo::SaveHDCToFile(picInfo pic,HWND hwnd){
    HDC hdc=GetDC(hwnd);
    HDC memDc=CreateCompatibleDC(hdc);
    HBITMAP hBmp=CreateCompatibleBitmap(hdc,pic.Width,pic.Height);
    SelectObject(memDc,hBmp);

    //picFormat=QString::fromWCharArray(pic.MIMEType);//在这里才初始化picFormat会出bug的啊喂！
    //qDebug()<<AudioAddr<<pic.CanDrawPicture;
    //if(pic.CanDrawPicture==0) qDebug()<<QString::fromWCharArray(zplayer->GetErrorW());
    zplayer->DrawBitmapToHDC(memDc,0,0,pic.Width,pic.Height,pic.hBitmap);
    zplayer->Close();
    zplayer->Release();

    QPixmap pixmap=QtWin::fromHBITMAP(hBmp,QtWin::HBitmapPremultipliedAlpha);
    QImage img=pixmap.toImage();
    if(pic.picFormat=="image/png"&&pic.CanDrawPicture==1) img.save(getRealCoverAddr(),"PNG",100);
    else if(pic.picFormat=="image/jpeg"&&pic.CanDrawPicture==1) img.save(getRealCoverAddr(),"JPG",100);
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
    if(pic.picFormat=="image/png") ccc=QApplication::applicationDirPath()+"/infos/"+cc+".png";
    else if(pic.picFormat=="image/jpeg") ccc=QApplication::applicationDirPath()+"/infos/"+cc+".jpg";
    else ccc="error!";
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
