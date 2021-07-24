#include "mainwindow.h"
#include "ui_mainwindow.h"

libZPlay::ZPlay* zplayer;
QString title,artist,album;
HWND hwnd;
int valid_lyric;//储存有效lyric的数量
QStringList name_list;//播放列表
int play_progress;//列表播放进度
QList<QListPushButton*> playlist_buttons;
QIcon* empty_icon;
bool isRandomPlay;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    hwnd=(HWND)this->window()->winId();
    setWindowFlags(Qt::FramelessWindowHint);//删除主窗口标题
    empty_icon=new QIcon;
    ui->widget_title->setStyleSheet("background-color: rgb(228, 0, 127);");
    ui->pushButton_lyric->setDefault(false);
    ui->pushButton_lyric_html->setDefault(false);
    ui->pushButton_lyric_translate->setDefault(false);
    ui->pushButton_minimize->setDefault(false);
    ui->label_singers->hide();
    ui->Slider_volume->isHorizontal=false;

    player=new QMediaPlayer(this);
    playlist=new QMediaPlaylist(this);
    player->setPlaylist(playlist);
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    isRandomPlay=false;
    player->setNotifyInterval(10);//10ms一刷新

    color_info[0]="#ffae00";//果皇
    color_info[1]="#00bbff";//绘里
    color_info[2]="#a8a8a8";//小鸟
    color_info[3]="#6d56ff";//海爷
    color_info[4]="#fee155";//凛喵
    color_info[5]="#ff6239";//makimaki
    color_info[6]="#e44fff";//希
    color_info[7]="#44cd71";//花阳
    color_info[8]="#ff62dd";//nico

    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(time_change(qint64)));
    connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(get_duration(qint64)));
    connect(ui->horizontalSlider,SIGNAL(clicked()),this,SLOT(QSliderProClicked()));
    connect(ui->Slider_volume,SIGNAL(clicked()),this,SLOT(SliderVolumeClicked()));
    connect(player,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(player_state_change(QMediaPlayer::State)));

    QString path=QApplication::applicationDirPath()+"/songs/";
    QDirIterator iter(path,QStringList()<<"*.mp3",
                      QDir::Files|QDir::NoSymLinks,
                      QDirIterator::Subdirectories);
    while(iter.hasNext()){
        iter.next();
        name_list.append(iter.fileName());
    }

    QFont font1,font2;
    font1.setPointSize(12);
    font2.setPointSize(8);
    for(int i=0;i<name_list.length();i++){
        playlist->addMedia(QUrl(QApplication::applicationDirPath()+"/songs/"+name_list.at(i)));
        QListPushButton* pb_temp=new QListPushButton(ui->scrollAreaWidgetContents);
        QListPushButton* pb_temp2=new QListPushButton(ui->scrollAreaWidgetContents);
        pb_temp->setGeometry(QRect(0,i*80+5,190,80));
        pb_temp2->setGeometry(QRect(0,i*80+50,190,30));
        get_meta(path+name_list.at(i),false);
        pb_temp->setDefault(false);
        pb_temp->setVisible(true);
        pb_temp2->setDefault(false);
        pb_temp2->setVisible(true);
        pb_temp->setStyleSheet("border-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
        pb_temp->setFont(font1);
        pb_temp->setAttribute(Qt::WA_Hover,true);
        pb_temp2->setStyleSheet("color:#a3a3a3;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
        pb_temp2->setFont(font2);
        pb_temp2->setAttribute(Qt::WA_TransparentForMouseEvents,true);
        pb_temp->setText(adjust_text_overlength(" "+title,pb_temp,1));
        pb_temp2->setText(adjust_text_overlength(" "+artist,pb_temp2,1));
        pb_temp->seq=i;
        pb_temp2->seq=i;
        playlist_buttons.append(pb_temp);
    }
    ui->label_playlist->setText("<html><head/><body><p><span style=\" font-size:12pt;\">播放列表</span></p><p><span style=\" color:#a3a3a3;\">共"
                                +QString::number(name_list.count())+"首歌曲</span></p></body></html>");
    playlist_buttons.at(0)->setStyleSheet("color:#ee5dae;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
    for(int i=0;i<playlist_buttons.length();i++){
        connect(playlist_buttons.at(i),SIGNAL(clicked(int)),this,SLOT(playlist_buttons_clicked(int)));
        playlist_buttons.at(i)->show();
    }
    ui->scrollAreaWidgetContents->setMinimumHeight(name_list.length()*80+5);
    play_progress=0;
    load_single_song(name_list.at(0));

    player->play();
    connect(player,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(change_song(QMediaPlayer::MediaStatus)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::time_change(qint64 time){
    QString temp=tr("%1").arg(time);
    int real_time=temp.toInt();
    int minute=real_time/60000;
    int second=(real_time-minute*60000)/1000;
    QString min,sec;
    if(minute<10){
        min="0"+QString::number(minute);
    }
    else min=QString::number(minute);
    if(second<10){
        sec="0"+QString::number(second);
    }
    else sec=QString::number(second);
    ui->label_time->setText(min+":"+sec);
    ui->horizontalSlider->setSliderPosition(real_time);
    for(int i=0;i<valid_lyric-1;i++){
        if(real_time>=lyric[i].time&&real_time<lyric[i+1].time){
            if(ui->pushButton_lyric->text()!=lyric[i].text){
                if(lyric[i].text.contains("<")){//html
                    QTextDocument html;
                    html.setHtml(lyric[i].text);
                    QPixmap pixmap(html.size().width(),html.size().height());
                    pixmap.fill(Qt::transparent);
                    QPainter painter(&pixmap);
                    html.drawContents(&painter,pixmap.rect());
                    QIcon icon(pixmap);
                    ui->pushButton_lyric_html->setText("");
                    ui->pushButton_lyric_html->setIcon(icon);
                    ui->pushButton_lyric_html->setIconSize(pixmap.rect().size());
                    ui->pushButton_lyric->setText("");
                    ui->pushButton_lyric_translate->setText(lyric_translate[i].text);
                }
                else{
                    ui->pushButton_lyric_html->setIcon(*empty_icon);
                    ui->pushButton_lyric->setText(lyric[i].text);
                    ui->pushButton_lyric_translate->setText(lyric_translate[i].text);
                    if(lyric[i].color_num==0||lyric[i].color_num==9){
                        ui->pushButton_lyric->setStyleSheet("color:black;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                    }
                    else if(lyric[i].color_num==1){
                        for(int u=0;u<9;u++){
                            if(lyric[i].color[u]==true) ui->pushButton_lyric->setStyleSheet("color:"+color_info[u]+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                        }
                    }
                    else{
                        QString style_prefix="color:qlineargradient(spread:pad x1:0, y1:0, x2:1, y2:0, ";
                        QString style_suffix=");\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);";
                        QString temp="";
                        int progress=0;
                        for(int ii=0;ii<lyric[i].color_num;ii++){
                            temp+="stop:"+QString::number(ii*1.0/(lyric[i].color_num-1))+" ";
                            for(int iii=progress;iii<9;iii++){
                                if(lyric[i].color[iii]==true){
                                    temp+=color_info[iii]+",";
                                    iii++;//防止break导致的iii不变
                                    progress=iii;//一定要在iii++后再赋值，否则progress不变，一直卡在一个颜色
                                    break;
                                }
                            }
                        }
                        temp=temp.mid(0,temp.length()-1);
                        ui->pushButton_lyric->setStyleSheet(style_prefix+temp+style_suffix);
                    }
                }

                //开始设置歌手显示
                QString singers="";
                if(lyric[i].color_num==9) singers="高坂穗乃果、绚濑绘里、南小鸟、园田海未、星空凛、西木野真姬、东条希、小泉花阳、矢泽妮可、";
                else if(lyric[i].color_num==0) singers="、";
                else{
                    if(lyric[i].color[0]==true) singers+="高坂穗乃果、";
                    if(lyric[i].color[1]==true) singers+="绚濑绘里、";
                    if(lyric[i].color[2]==true) singers+="南小鸟、";
                    if(lyric[i].color[3]==true) singers+="园田海未、";
                    if(lyric[i].color[4]==true) singers+="星空凛、";
                    if(lyric[i].color[5]==true) singers+="西木野真姬、";
                    if(lyric[i].color[6]==true) singers+="东条希、";
                    if(lyric[i].color[7]==true) singers+="小泉花阳、";
                    if(lyric[i].color[8]==true) singers+="矢泽妮可、";
                }
                singers=singers.left(singers.length()-1);
                if(singers==""){
                    ui->label_singers->hide();
                }
                ui->label_singers->setText(singers);
            }
            break;
        }
        else if(real_time>=lyric[valid_lyric-1].time){
            if(ui->pushButton_lyric->text()!=lyric[valid_lyric-1].text){
                if(lyric[valid_lyric-1].text.contains("<")){//html
                    QTextDocument html;
                    html.setHtml(lyric[valid_lyric-1].text);
                    QPixmap pixmap(html.size().width(),html.size().height());
                    pixmap.fill(Qt::transparent);
                    QPainter painter(&pixmap);
                    html.drawContents(&painter,pixmap.rect());
                    QIcon icon(pixmap);
                    ui->pushButton_lyric_html->setText("");
                    ui->pushButton_lyric_html->setIcon(icon);
                    ui->pushButton_lyric_html->setIconSize(pixmap.rect().size());
                    ui->pushButton_lyric->setText("");
                    ui->pushButton_lyric_translate->setText(lyric_translate[valid_lyric-1].text);
                }
                else{
                    ui->pushButton_lyric_html->setIcon(*empty_icon);
                    ui->pushButton_lyric->setText(lyric[valid_lyric-1].text);
                    ui->pushButton_lyric_translate->setText(lyric_translate[valid_lyric-1].text);

                    //开始调色
                    if(lyric[valid_lyric-1].color_num==0){
                        ui->pushButton_lyric->setStyleSheet("color:black;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                    }
                    else if(lyric[valid_lyric-1].color_num==9) ui->pushButton_lyric->setStyleSheet("color:black;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                    else if(lyric[valid_lyric-1].color_num==1){
                        for(int u=0;u<9;u++){
                            if(lyric[valid_lyric-1].color[u]==true) ui->pushButton_lyric->setStyleSheet("color:"+color_info[u]+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                        }
                    }
                    else{
                        QString style_prefix="color:qlineargradient(spread:pad x1:0, y1:0, x2:1, y2:0, ";
                        QString style_suffix=");\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);";
                        QString temp="";
                        int progress=0;
                        for(int ii=0;ii<lyric[valid_lyric-1].color_num;ii++){
                            temp+="stop:"+QString::number(ii*1.0/(lyric[valid_lyric-1].color_num-1))+" ";
                            for(int iii=progress;iii<9;iii++){
                                if(lyric[valid_lyric-1].color[iii]==true){
                                    temp+=color_info[iii]+",";
                                    iii++;//防止break导致的iii不变
                                    progress=iii;//一定要在iii++后再赋值，否则progress不变，一直卡在一个颜色
                                    break;
                                }
                            }
                        }
                        temp=temp.mid(0,temp.length()-1);
                        ui->pushButton_lyric->setStyleSheet(style_prefix+temp+style_suffix);
                        qDebug()<<ui->pushButton_lyric->styleSheet();
                    }

                }

                //开始设置歌手显示
                QString singers="";
                if(lyric[valid_lyric-1].color_num==9) singers="高坂穗乃果、绚濑绘里、南小鸟、园田海未、星空凛、西木野真姬、东条希、小泉花阳、矢泽妮可、";
                else if(lyric[valid_lyric-1].color_num==0) singers="、";
                else{
                    if(lyric[valid_lyric-1].color[0]==true) singers+="高坂穗乃果、";
                    if(lyric[valid_lyric-1].color[1]==true) singers+="绚濑绘里、";
                    if(lyric[valid_lyric-1].color[2]==true) singers+="南小鸟、";
                    if(lyric[valid_lyric-1].color[3]==true) singers+="园田海未、";
                    if(lyric[valid_lyric-1].color[4]==true) singers+="星空凛、";
                    if(lyric[valid_lyric-1].color[5]==true) singers+="西木野真姬、";
                    if(lyric[valid_lyric-1].color[6]==true) singers+="东条希、";
                    if(lyric[valid_lyric-1].color[7]==true) singers+="小泉花阳、";
                    if(lyric[valid_lyric-1].color[8]==true) singers+="矢泽妮可、";
                }
                singers=singers.left(singers.length()-1);
                if(singers==""){
                    ui->label_singers->hide();
                }
                ui->label_singers->setText(singers);
            }
            break;
        }
    }
}

void MainWindow::time_change(int time){
    int real_time=time;
    int minute=real_time/60000;
    int second=(real_time-minute*60000)/1000;
    QString min,sec;
    if(minute<10){
        min="0"+QString::number(minute);
    }
    else min=QString::number(minute);
    if(second<10){
        sec="0"+QString::number(second);
    }
    else sec=QString::number(second);
    ui->label_time->setText(min+":"+sec);
    for(int i=0;i<199;i++){
        if(real_time>=lyric[i].time&&real_time<lyric[i+1].time){
            if(ui->pushButton_lyric->text()!=lyric[i].text){
                if(lyric[i].text.contains("<")){//html
                    QTextDocument html;
                    html.setHtml(lyric[i].text);
                    QPixmap pixmap(html.size().width(),html.size().height());
                    pixmap.fill(Qt::transparent);
                    QPainter painter(&pixmap);
                    html.drawContents(&painter,pixmap.rect());
                    QIcon icon(pixmap);
                    ui->pushButton_lyric_html->setText("");
                    ui->pushButton_lyric_html->setIcon(icon);
                    ui->pushButton_lyric_html->setIconSize(pixmap.rect().size());
                    ui->pushButton_lyric->setText("");
                    ui->pushButton_lyric_translate->setText(lyric_translate[i].text);
                }
                else{
                    ui->pushButton_lyric_html->setIcon(*empty_icon);
                    ui->pushButton_lyric->setText(lyric[i].text);
                    ui->pushButton_lyric_translate->setText(lyric_translate[i].text);
                    if(lyric[i].color_num==0){
                        ui->pushButton_lyric->setStyleSheet("color:black;s\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                    }
                    else if(lyric[i].color_num==9) ui->pushButton_lyric->setStyleSheet("color:black;s\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                    else if(lyric[i].color_num==1){
                        for(int u=0;u<9;u++){
                            if(lyric[i].color[u]==true) ui->pushButton_lyric->setStyleSheet("color:"+color_info[u]+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                        }
                    }
                    else{
                        QString style_prefix="color:qlineargradient(spread:pad x1:0, y1:0, x2:1, y2:0, ";
                        QString style_suffix=");\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);";
                        QString temp="";
                        int progress=0;
                        for(int ii=0;ii<lyric[i].color_num;ii++){
                            temp+="stop:"+QString::number(ii*1.0/(lyric[i].color_num-1))+" ";
                            for(int iii=progress;iii<9;iii++){
                                if(lyric[i].color[iii]==true){
                                    temp+=color_info[iii]+",";
                                    iii++;//防止break导致的iii不变
                                    progress=iii;//一定要在iii++后再赋值，否则progress不变，一直卡在一个颜色
                                    break;
                                }
                            }
                        }
                        temp=temp.mid(0,temp.length()-1);
                        ui->pushButton_lyric->setStyleSheet(style_prefix+temp+style_suffix);
                    }
                }


                //开始设置歌手显示
                QString singers;
                if(lyric[i].color_num==9) singers="高坂穗乃果、绚濑绘里、南小鸟、园田海未、星空凛、西木野真姬、东条希、小泉花阳、矢泽妮可、";
                else if(lyric[i].color_num==0) singers="、";
                else{
                    if(lyric[i].color[0]==true) singers+="高坂穗乃果、";
                    if(lyric[i].color[1]==true) singers+="绚濑绘里、";
                    if(lyric[i].color[2]==true) singers+="南小鸟、";
                    if(lyric[i].color[3]==true) singers+="园田海未、";
                    if(lyric[i].color[4]==true) singers+="星空凛、";
                    if(lyric[i].color[5]==true) singers+="西木野真姬、";
                    if(lyric[i].color[6]==true) singers+="东条希、";
                    if(lyric[i].color[7]==true) singers+="小泉花阳、";
                    if(lyric[i].color[8]==true) singers+="矢泽妮可、";
                }
                singers=singers.left(singers.length()-1);
                if(singers==""){
                    ui->label_singers->hide();
                }
                ui->label_singers->setText(singers);
            }
            break;
        }
        else if(real_time>=lyric[valid_lyric-1].time){
            if(ui->pushButton_lyric->text()!=lyric[valid_lyric-1].text){
                if(lyric[valid_lyric-1].text.contains("<")){//html
                    QTextDocument html;
                    html.setHtml(lyric[valid_lyric-1].text);
                    QPixmap pixmap(html.size().width(),html.size().height());
                    pixmap.fill(Qt::transparent);
                    QPainter painter(&pixmap);
                    html.drawContents(&painter,pixmap.rect());
                    QIcon icon(pixmap);
                    ui->pushButton_lyric_html->setText("");
                    ui->pushButton_lyric_html->setIcon(icon);
                    ui->pushButton_lyric_html->setIconSize(pixmap.rect().size());
                    ui->pushButton_lyric->setText("");
                    ui->pushButton_lyric_translate->setText(lyric_translate[valid_lyric-1].text);
                }
                else{
                    ui->pushButton_lyric_html->setIcon(*empty_icon);
                    ui->pushButton_lyric->setText(lyric[valid_lyric-1].text);
                    ui->pushButton_lyric_translate->setText(lyric_translate[valid_lyric-1].text);

                    //开始调色
                    if(lyric[valid_lyric-1].color_num==0){
                        ui->pushButton_lyric->setStyleSheet("color:black;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                    }
                    else if(lyric[valid_lyric-1].color_num==9) ui->pushButton_lyric->setStyleSheet("color:black;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                    else if(lyric[valid_lyric-1].color_num==1){
                        for(int u=0;u<9;u++){
                            if(lyric[valid_lyric-1].color[u]==true) ui->pushButton_lyric->setStyleSheet("color:"+color_info[u]+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                        }
                    }
                    else{
                        QString style_prefix="color:qlineargradient(spread:pad x1:0, y1:0, x2:1, y2:0, ";
                        QString style_suffix=");\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);";
                        QString temp="";
                        int progress=0;
                        for(int ii=0;ii<lyric[valid_lyric-1].color_num;ii++){
                            temp+="stop:"+QString::number(ii*1.0/(lyric[valid_lyric-1].color_num-1))+" ";
                            for(int iii=progress;iii<9;iii++){
                                if(lyric[valid_lyric-1].color[iii]==true){
                                    temp+=color_info[iii]+",";
                                    iii++;//防止break导致的iii不变
                                    progress=iii;//一定要在iii++后再赋值，否则progress不变，一直卡在一个颜色
                                    break;
                                }
                            }
                        }
                        temp=temp.mid(0,temp.length()-1);
                        ui->pushButton_lyric->setStyleSheet(style_prefix+temp+style_suffix);
                    }
                }

                //开始设置歌手显示
                QString singers;
                if(lyric[valid_lyric-1].color_num==9) singers="高坂穗乃果、绚濑绘里、南小鸟、园田海未、星空凛、西木野真姬、东条希、小泉花阳、矢泽妮可、";
                else if(lyric[valid_lyric-1].color_num==0) singers="、";
                else{
                    if(lyric[valid_lyric-1].color[0]==true) singers+="高坂穗乃果、";
                    if(lyric[valid_lyric-1].color[1]==true) singers+="绚濑绘里、";
                    if(lyric[valid_lyric-1].color[2]==true) singers+="南小鸟、";
                    if(lyric[valid_lyric-1].color[3]==true) singers+="园田海未、";
                    if(lyric[valid_lyric-1].color[4]==true) singers+="星空凛、";
                    if(lyric[valid_lyric-1].color[5]==true) singers+="西木野真姬、";
                    if(lyric[valid_lyric-1].color[6]==true) singers+="东条希、";
                    if(lyric[valid_lyric-1].color[7]==true) singers+="小泉花阳、";
                    if(lyric[valid_lyric-1].color[8]==true) singers+="矢泽妮可、";
                }
                singers=singers.left(singers.length()-1);
                if(singers==""){
                    ui->label_singers->hide();
                }
                ui->label_singers->setText(singers);
            }
            break;
        }
    }
}

void MainWindow::get_duration(qint64 time){
    int minute,second;
    QString min,sec;
    minute=time/60000;
    second=(time-minute*60000)/1000;
    if(minute<10){
        min="0"+QString::number(minute);
    }
    else{
        min=QString::number(minute);
    }
    if(second<10){
        sec="0"+QString::number(second);
    }
    else{
        sec=QString::number(second);
    }
    ui->label_duration->setText("/"+min+":"+sec);
    duration=time;
    ui->horizontalSlider->setMaximum(duration);
    ui->horizontalSlider->setMinimum(0);
    ui->label_duration->setText("/"+min+":"+sec);
}

void MainWindow::SaveHDCToFile(libZPlay::TID3InfoEx id3_info){
    libZPlay::TID3Picture pic;
    HDC hdc=GetDC(hwnd);
    HDC memDc=CreateCompatibleDC(hdc);
    HBITMAP hBmp=CreateCompatibleBitmap(hdc,id3_info.Picture.Width,id3_info.Picture.Height);
    SelectObject(memDc,hBmp);

    pic=id3_info.Picture;
    zplayer->DrawBitmapToHDC(memDc,0,0,id3_info.Picture.Width,id3_info.Picture.Height,id3_info.Picture.hBitmap);

    QPixmap pixmap=QtWin::fromHBITMAP(hBmp,QtWin::HBitmapNoAlpha);
    QImage img=pixmap.toImage();
    img.save("album.png","PNG",100);
    SelectObject(memDc,(HBITMAP)NULL);
    DeleteDC(memDc);
    DeleteObject(hBmp);
    DeleteDC(hdc);
}

void MainWindow::read_lyric(QString path,int mode){
    if(mode==0){//原文
        QFile lrc(path);
        if(lrc.open(QIODevice::ReadOnly)){
            QString all=lrc.readAll();
            QStringList list=all.split("\n");
            QRegularExpression re("\\[(\\d+)?:(\\d+)?(\\.\\d+)?(\\S+)?\\]");
            int lyric_seq=0;
            for(int i=0;i<list.count();i++){
                QString text;
                QString color;
                qint64 lrc_time;

                QRegularExpressionMatch mat=re.match(list[i]);
                lrc_time=mat.captured(1).toInt()*60000+mat.captured(2).toInt()*1000+mat.captured(3).mid(1).toInt()*10;
                text=QString(list[i]).right(QString(list[i]).length()-mat.capturedLength());
                color=text.mid(1,17);
                text=text.right(text.length()-19);

                int color_num=0;
                bool colorr[9]={0};
                QStringList color_list=color.split(",");
                for(int u=0;u<9;u++){
                    if(color_list[u]=="1"){
                        color_num++;
                        colorr[u]=true;
                    }
                    else{
                        colorr[u]=false;
                    }
                }
                if(text!=""){
                    lyric[lyric_seq].time=lrc_time;
                    lyric[lyric_seq].text=adjust_text_overlength(text,ui->pushButton_lyric,0);
                    lyric[lyric_seq].color_num=color_num;
                    for(int u=0;u<9;u++){
                        if(colorr[u]==true){
                            lyric[lyric_seq].color[u]=true;
                        }
                    }
                    lyric_seq++;
                }
            }
        }
    }
    else if(mode==1){//翻译
        QFile lrc(path);
        if(lrc.open(QIODevice::ReadOnly)){
            QString all=lrc.readAll();
            QStringList list=all.split("\n");
            QRegularExpression re("\\[(\\d+)?:(\\d+)?(\\.\\d+)?(\\S+)?\\]");
            int lyric_seq=0;
            for(int i=0;i<list.count();i++){
                QString text;
                QString color;
                qint64 lrc_time;

                QRegularExpressionMatch mat=re.match(list[i]);
                lrc_time=mat.captured(1).toInt()*60000+mat.captured(2).toInt()*1000+mat.captured(3).mid(1).toInt()*10;
                text=QString(list[i]).right(QString(list[i]).length()-mat.capturedLength());
                color=text.mid(1,17);
                text=text.right(text.length()-19);

                int color_num=0;
                bool colorr[9]={0};
                QStringList color_list=color.split(",");
                for(int u=0;u<9;u++){
                    if(color_list[u]=="1"){
                        color_num++;
                        colorr[u]=true;
                    }
                    else{
                        colorr[u]=false;
                    }
                }
                if(text!=""){
                    lyric_translate[lyric_seq].time=lrc_time;
                    lyric_translate[lyric_seq].text=adjust_text_overlength(text,ui->pushButton_lyric_translate,0);
                    lyric_translate[lyric_seq].color_num=color_num;
                    for(int u=0;u<9;u++){
                        if(colorr[u]==true){
                            lyric_translate[lyric_seq].color[u]=true;
                        }
                    }
                    lyric_seq++;
                }
            }
        }
    }
}

void MainWindow::on_pushButton_lyric_clicked()
{
    if(ui->label_singers->isVisible()==true){
        ui->label_singers->hide();
    }
    else if(ui->label_singers->text()!=""){
        ui->label_singers->show();
    }
}

void MainWindow::on_pushButton_play_clicked()
{
    if(player->state()==QMediaPlayer::PausedState){
        player->play();
        ui->pushButton_play->setStyleSheet("border-image: url(:/new/prefix1/player-pause-circle.png);");
    }
    else if(player->state()==QMediaPlayer::PlayingState){
        player->pause();
        ui->pushButton_play->setStyleSheet("border-image: url(:/new/prefix1/player-play-circle.png);");
    }

}

void MainWindow::on_horizontalSlider_sliderPressed()
{
    disconnect(player,SIGNAL(positionChanged(qint64)),this,SLOT(time_change(qint64)));
    connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(time_change_manual(int)));
    connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(time_change(int)));
}

void MainWindow::on_horizontalSlider_sliderReleased()
{
    player->setPosition(ui->horizontalSlider->value());
    player->play();
    disconnect(ui->horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(time_change_manual(int)));
    disconnect(ui->horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(time_change(int)));
    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(time_change(qint64)));
}

void MainWindow::time_change_manual(int time){
    int minute=time/60000;
    int second=(time-minute*60000)/1000;
    QString min,sec;
    if(minute<10){
        min="0"+QString::number(minute);
    }
    else min=QString::number(minute);
    if(second<10){
        sec="0"+QString::number(second);
    }
    else sec=QString::number(second);
    ui->label_time->setText(min+":"+sec);
}

void MainWindow::get_meta(QString path,bool isNeedAlbumCover){
    zplayer=libZPlay::CreateZPlay();
    libZPlay::TID3InfoEx id3_info;
    if(zplayer->OpenFile(path.toLocal8Bit(),libZPlay::sfAutodetect)){
        if(zplayer->LoadID3Ex(&id3_info,1)){
            if(isNeedAlbumCover==true) SaveHDCToFile(id3_info);
            title=QString::fromLocal8Bit(id3_info.Title);
            artist=QString::fromLocal8Bit(id3_info.Artist);
            album=QString::fromLocal8Bit(id3_info.Album);
        }
    }
    zplayer->Close();
    zplayer->Release();
    if(isNeedAlbumCover==true) ui->label_info->setText(title+"-"+artist+"\n"+album);
    //读取封面并显示
    if(isNeedAlbumCover==true) ui->widget_cover->setStyleSheet("border-image:url("+QApplication::applicationDirPath()+"/../album.png);");
}

void MainWindow::load_single_song(QString name){
    QString path_audio=QApplication::applicationDirPath()+"/songs/"+name;
    QStringList list_temp=name.split(".");
    QString path_lyric=QApplication::applicationDirPath()+"/songs/"+list_temp.at(0)+".ll";
    QString path_lyric_translate=QApplication::applicationDirPath()+"/songs/"+list_temp.at(0)+".trans";

    //以下获取歌曲元数据并提取封面
    get_meta(path_audio,true);

    for(int i=0;i<200;i++){
        lyric[i].color_num=0;
        lyric[i].text="";
        lyric[i].time=-1;
        lyric_translate[i].color_num=0;
        lyric_translate[i].text="";
        lyric_translate[i].time=-1;
        for(int ii=0;ii<9;ii++){
            lyric[i].color[ii]=false;
            lyric_translate[i].color[ii]=false;
        }
    }
    read_lyric(path_lyric,0);
    read_lyric(path_lyric_translate,1);
    //ui->pushButton_lyric->setText(lyric[0].text);
    ui->pushButton_lyric_translate->setText(lyric_translate[0].text);
    for(int i=0;i<200;i++){
        if(lyric[i].text==""&&lyric[i].time==-1){
            valid_lyric=i;
            break;
        }
        //qDebug()<<"seq:"<<i<<"text:"<<lyric[i].text;
    }
}

void MainWindow::QSliderProClicked(){
    int pos=ui->horizontalSlider->value();
    disconnect(player,SIGNAL(positionChanged(qint64)),this,SLOT(time_change(qint64)));
    player->setPosition(pos);
    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(time_change(qint64)));
}

void MainWindow::change_song(QMediaPlayer::MediaStatus status){
    if(status==QMediaPlayer::EndOfMedia){
        if(playlist->playbackMode()==QMediaPlaylist::CurrentItemInLoop){

        }
        else if(playlist->playbackMode()==QMediaPlaylist::Loop&&isRandomPlay==false){
            for(int i=0;i<200;i++){
                lyric[i].time=-1;
                lyric[i].text="";
                lyric[i].color_num=0;
                for(int u=0;u<9;u++){
                    lyric[i].color[u]=false;
                }
            }
            if(name_list.length()>play_progress+1){
                play_progress++;
            }
            else if(play_progress+1==name_list.length()){
                play_progress=0;
            }
            ui->label_singers->setText("");
            ui->label_singers->hide();
            load_single_song(name_list[play_progress]);
            for(int i=0;i<playlist_buttons.count();i++){
                playlist_buttons.at(i)->setStyleSheet("border-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
            }
            playlist_buttons.at(play_progress)->setStyleSheet("color:#ee5dae;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
            for(int i=0;i<playlist_buttons.count();i++){
                playlist_buttons.at(i)->show();
            }
        }
        }
}

void MainWindow::on_Slider_volume_valueChanged(int value)
{
    player->setVolume(value);
    ui->label_volume->setText(QString::number(value));
}

void MainWindow::SliderVolumeClicked(){
    int pos=ui->Slider_volume->value();
    player->setVolume(pos);
}

void MainWindow::on_pushButton_next_clicked()
{
    if(play_progress+1==name_list.length()){
        play_progress=0;
    }
    else if(play_progress+1<name_list.length()){
        play_progress++;
    }
    playlist->setCurrentIndex(play_progress);
    ui->pushButton_lyric->setText("");
    ui->label_singers->setText("");
    ui->label_singers->hide();
    ui->pushButton_lyric_html->setIcon(*empty_icon);
    load_single_song(name_list[play_progress]);
    for(int i=0;i<playlist_buttons.count();i++){
        playlist_buttons.at(i)->setStyleSheet("border-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
    }
    playlist_buttons.at(play_progress)->setStyleSheet("color:#ee5dae;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
    for(int i=0;i<playlist_buttons.count();i++){
        playlist_buttons.at(i)->show();
        qDebug()<<playlist_buttons.at(i)->styleSheet();
    }
}

void MainWindow::player_state_change(QMediaPlayer::State state){
    if(state==QMediaPlayer::PausedState){
        ui->pushButton_play->setStyleSheet("border-image: url(:/new/prefix1/player-play-circle.png);");
    }
    else if(state==QMediaPlayer::PlayingState){
        ui->pushButton_play->setStyleSheet("border-image: url(:/new/prefix1/player-pause-circle.png);");
    }
}

QString MainWindow::adjust_text_overlength(QString text,QPushButton* obj,int mode){//自动换行 但不能修改含html标签的句子
    if(mode==0){//自动换行模式
        if(!text.contains("<")){
            QString result=text;
            QString final;
            QStringList list=result.split("\n");
            result=text;
            int buttonwidth=obj->width();
            for(int u=0;u<list.count();u++){
                int textwidth=obj->fontMetrics().width(list.at(u));
                if(textwidth>=buttonwidth){
                    QStringList temp=text.split(" ");
                    if(temp.count()>1){//有空格
                        int sum=0;
                        for(int i=0;i<temp.count();i++){
                            sum+=temp.at(i).length();
                        }
                        int l=0,pos=0;
                        for(int i=0;i<temp.count()-1;i++){
                            l+=temp.at(i).length();
                            if(l<=sum/2&&(l+temp.at(i+1).length())>sum/2){
                                pos=i;
                                break;
                            }
                        }
                        result.replace(l+pos,1,"\n");
                    }
                    else{//无空格
                        result.insert(result.length()/2,"\n");
                    }

                    //检查换行后是否需要继续换行
                    QStringList ooo=result.split("\n");
                    QStringList copy;
                    for(int p=0;p<ooo.count();p++){
                        if(obj->fontMetrics().width(ooo.at(p))>=buttonwidth){
                            copy.append(adjust_text_overlength(ooo.at(p),obj,0));
                        }
                        else copy.append(ooo.at(p));
                    }
                    for(int p=0;p<copy.count();p++){
                        final+=copy.at(p)+"\n";
                    }
                }
                else final+=result+"\n";
            }
            return final.left(final.length()-1);
        }
        else{
            return text;
        }
    }
    else if(mode==1){//省略模式(替换为...)
        int textwidth=obj->fontMetrics().width(text);
        int buttonwidth=obj->width();
        if(textwidth<=buttonwidth) return text;
        else{
            QString temp;
            int tempwidth;
            for(int i=0;i<text.length();i++){
                temp.append(text.at(i));
                tempwidth=obj->fontMetrics().width(temp);
                int ooowidth=obj->fontMetrics().width("...");
                if(tempwidth+ooowidth>buttonwidth){
                    QString res=temp.left(temp.length()-1)+"...";
                    qDebug()<<res;
                    return res;
                }
            }
        }
    }
}

void MainWindow::on_pushButton_last_clicked()
{
    if(play_progress==0){
        play_progress=name_list.length()-1;
    }
    else if(play_progress<name_list.length()&&play_progress>0){
        play_progress--;
    }
    playlist->setCurrentIndex(play_progress);
    ui->pushButton_lyric->setText("");
    ui->label_singers->setText("");
    ui->label_singers->hide();
    ui->pushButton_lyric_html->setIcon(*empty_icon);
    load_single_song(name_list[play_progress]);
    for(int i=0;i<playlist_buttons.count();i++){
        playlist_buttons.at(i)->setStyleSheet("border-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
    }
    playlist_buttons.at(play_progress)->setStyleSheet("color:#ee5dae;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
    for(int i=0;i<playlist_buttons.count();i++){
        playlist_buttons.at(i)->show();
    }
}

void MainWindow::playlist_buttons_clicked(int seq){
    for(int i=0;i<playlist_buttons.count();i++){
        playlist_buttons.at(i)->setStyleSheet("border-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
    }
    playlist_buttons.at(seq)->setStyleSheet("color:#ee5dae;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
    ui->pushButton_lyric->setText("");
    ui->pushButton_lyric->setStyleSheet("color:black;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
    ui->pushButton_lyric_html->setIcon(*empty_icon);
    play_progress=seq;
    load_single_song(name_list[seq]);
    playlist->setCurrentIndex(seq);
}

void MainWindow::on_pushButton_minimize_clicked()
{
    this->showMinimized();
}

void MainWindow::on_pushButton_close_clicked()
{
    this->close();
}

void MainWindow::on_pushButton_playmode_clicked()
{
    if(playlist->playbackMode()==QMediaPlaylist::Loop&&isRandomPlay==false){//从列表循环到单曲循环
        ui->pushButton_playmode->setStyleSheet("border-image: url(:/new/prefix1/currentitemloop.png);");
        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    }
    else if(playlist->playbackMode()==QMediaPlaylist::CurrentItemInLoop){//从单曲循环到随机播放
        ui->pushButton_playmode->setStyleSheet("border-image: url(:/new/prefix1/randomplay.png);");
        playlist->setPlaybackMode(QMediaPlaylist::Loop);
        isRandomPlay=true;
    }
    else if(playlist->playbackMode()==QMediaPlaylist::Loop&&isRandomPlay==true){//从随机播放到列表循环
        ui->pushButton_playmode->setStyleSheet("border-image: url(:/new/prefix1/loop.png);");
        isRandomPlay=false;
    }
}
