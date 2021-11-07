#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_desktoplyricwindow.h"//不写就无法调用子窗口的控件
#include "ui_quickselect.h"

libZPlay::ZPlay* zplayer;
QString title,artist,album;
HWND hwnd;
int valid_lyric;//储存有效lyric的数量
QStringList name_list;//播放列表
int play_progress;//列表播放进度
QList<QListPushButton*> playlist_buttons;
QList<QListPushButton*> playlist_singers_buttons;
QList<QGroupBox*> groupboxes;
QIcon* empty_icon;
bool isRandomPlay,isAutoPlay,isTray,isAutoShowDesktopLyric;
QPoint ori_pos;
QList<int> random_seq;
int randomplay_progress;
QStringList themes;
QStringList songlist;//歌单文件名列表
bool isPlaylistShowing=false;

QMediaPlayer* player;
QMediaPlaylist* playlist;
int duration;
LRC lyric[200];
LRC lyric_translate[200];
config conf;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->setAttribute(Qt::WA_TranslucentBackground,true);
    ui->setupUi(this);
    hwnd=(HWND)this->window()->winId();
    setWindowFlags(Qt::FramelessWindowHint);//删除主窗口标题
    setMouseTracking(true);
    //setFocusPolicy(Qt::StrongFocus);
    //installEventFilter(this);//安装事件过滤器

    QGraphicsDropShadowEffect* shadow=new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(Qt::black);
    shadow->setBlurRadius(15);//设置阴影圆角
    ui->widget_shadow->setGraphicsEffect(shadow);//QGraphicsDropShadowEffect只能安装一次 再次调用setGraphicsEffect时 之前安装的效果将被卸载

    empty_icon=new QIcon;
    mSysTrayIcon = new QSystemTrayIcon(this);//必须放在load_single_song()的前面
    ui->pushButton_lyric->setDefault(false);
    ui->pushButton_lyric_html->setDefault(false);
    ui->pushButton_lyric_translate->setDefault(false);
    ui->pushButton_minimize->setDefault(false);
    ui->label_singers->hide();
    ui->Slider_volume->isHorizontal=false;
    //ui->widget_settings->hide();
    ui->label_settings_info->hide();

    player=new QMediaPlayer();
    playlist=new QMediaPlaylist();
    player->setPlaylist(playlist);
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    isRandomPlay=false;
    isAutoPlay=false;
    isTray=true;
    player->setNotifyInterval(10);//10ms一刷新
    ui->label_volume->setFont(QFont(font_string,10));
    read_userdata();

    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(time_change(qint64)));
    connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(get_duration(qint64)));
    connect(ui->horizontalSlider,SIGNAL(clicked()),this,SLOT(QSliderProClicked()));
    connect(ui->Slider_volume,SIGNAL(clicked()),this,SLOT(SliderVolumeClicked()));
    connect(player,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(player_state_change(QMediaPlayer::State)));

    QString path=QApplication::applicationDirPath()+"/songs/";
    QDirIterator iter_song(path,QStringList()<<"*.mp3",
                      QDir::Files|QDir::NoSymLinks,
                      QDirIterator::Subdirectories);
    while(iter_song.hasNext()){
        iter_song.next();
        name_list.append(iter_song.fileName());
    }

    QFont font1,font2;
    font1.setPointSize(12);
    font2.setPointSize(8);
    for(int i=0;i<name_list.length();++i){
        playlist->addMedia(QUrl(QApplication::applicationDirPath()+"/songs/"+name_list.at(i)));
        QListPushButton* pb_temp=new QListPushButton(ui->scrollAreaWidgetContents);
        QListPushButton* pb_temp2=new QListPushButton(ui->scrollAreaWidgetContents);
        pb_temp->setGeometry(QRect(0,i*80+5,210,80));
        pb_temp2->setGeometry(QRect(0,i*80+50,210,30));
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
        playlist_singers_buttons.append(pb_temp2);
    }
    ui->label_playlist->setText("<html><head/><body><p><span style=\" font-size:12pt;\">播放列表</span></p><p><span style=\" color:#a3a3a3;font-size:9pt;\">共"
                                +QString::number(name_list.count())+"首歌曲</span></p></body></html>");
    for(int i=0;i<playlist_buttons.length();++i){
        connect(playlist_buttons.at(i),SIGNAL(clicked(int)),this,SLOT(playlist_buttons_clicked(int)));
        playlist_buttons.at(i)->show();
    }
    ui->scrollAreaWidgetContents->setMinimumHeight(name_list.length()*80+5);
    play_progress=0;
    load_single_song(name_list.at(0));
    playlist_buttons.at(0)->setStyleSheet("color:"+conf.theme_color+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
    //设置焦点按钮的字体颜色一定要在load_single_song()函数之后 否则conf.theme_color将不正确

    player->play();//必须放在conncet的前面 否则程序将崩溃
    connect(player,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(change_song(QMediaPlayer::MediaStatus)));
    if(isAutoPlay==false) player->pause();

    QIcon icon = QIcon(":/new/prefix1/icon.png");
    mSysTrayIcon->setIcon(icon);
    mSysTrayIcon->setToolTip(title+" - "+artist);
    QMenu* menu=new QMenu();
    ui->action_exit->setFont(QFont(font_string,8));
    menu->addAction(ui->action_exit);
    mSysTrayIcon->setContextMenu(menu);
    connect(mSysTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));
    mSysTrayIcon->show();

    if(isAutoShowDesktopLyric==true){
        dd.show();
        ui->pushButton_DesktopLyric->setStyleSheet("background-color: rgba(0,0,0,0);\ncolor:"+conf.theme_color+";");
    }

    QString pathh=QApplication::applicationDirPath()+"/resources/";
    QDirIterator iter_config(pathh,QStringList()<<"*.config",
                      QDir::Files|QDir::NoSymLinks,
                      QDirIterator::Subdirectories);
    while(iter_config.hasNext()){
        iter_config.next();
        if(iter_config.fileName()!="default.config") themes.append(iter_config.fileName());
        //qDebug()<<themes.last();
    }

    //ui->widget_playlist->hide();
    on_pushButton_hideplayer_clicked();
    connect(ui->widget_cover,SIGNAL(clicked()),this,SLOT(show_player()));

    QString path_songlist=QApplication::applicationDirPath()+"/songlists/";
    QDirIterator iter_songlist(path_songlist,QStringList()<<"*.llist",
                      QDir::Files|QDir::NoSymLinks,
                      QDirIterator::Subdirectories);
    while(iter_songlist.hasNext()){
        iter_songlist.next();
        songlist.append(iter_songlist.fileName());
    }
    if(songlist.isEmpty()==true){
        qDebug()<<"no songlist!";
    }
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
    for(int i=0;i<valid_lyric-1;++i){
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

                    QString dda=lyric[i].text;
                    dda.replace("color:#000000","color:#7f7f7f");
                    dda.replace("font-size:14pt;","font-size:13pt;");
                    //qDebug()<<dda; e.g.<span style=" font-size:14pt; font-weight:600; color:#55aaff;">aaaaa</span><span style=" font-size:14pt; font-weight:600; color:#ffaaff;">aaaaaaa</span>
                    QTextDocument htmll;
                    htmll.setHtml(dda);
                    QPixmap pixmapp(htmll.size().width(),htmll.size().height());
                    pixmapp.fill(Qt::transparent);
                    QPainter painterr(&pixmapp);
                    htmll.drawContents(&painterr,pixmapp.rect());
                    QIcon iconn(pixmapp);
                    dd.ui->pushButton_lyric_html->setText("");
                    dd.ui->pushButton_lyric_html->setIcon(iconn);
                    dd.ui->pushButton_lyric_html->setIconSize(pixmapp.rect().size());
                    dd.ui->pushButton_lyric->setText("");

                    if(!lyric_translate[i].text.contains("\n")){
                        dd.ui->label_lyric_translate->setText(lyric_translate[i].text);
                    }
                    else{
                        QStringList tem=lyric_translate[i].text.split("\n");
                        QString aaa;
                        for(int m=0;m<tem.count();++m){
                            aaa.append(tem.at(m)+" ");
                        }
                        dd.ui->label_lyric_translate->setText(adjust_text_overlength(aaa,dd.ui->label_lyric_translate,1));
                    }
                }
                else{
                    ui->pushButton_lyric_html->setIcon(*empty_icon);
                    dd.ui->pushButton_lyric_html->setIcon(*empty_icon);
                    ui->pushButton_lyric->setText(lyric[i].text);
                    if(!lyric[i].text.contains("\n")){
                        dd.ui->pushButton_lyric->setText(lyric[i].text);
                    }
                    else{
                        QStringList tem=lyric[i].text.split("\n");
                        QString aaa;
                        for(int m=0;m<tem.count();++m){
                            aaa.append(tem.at(m)+" ");
                        }
                        dd.ui->pushButton_lyric->setText(adjust_text_overlength(aaa,dd.ui->pushButton_lyric,1));
                    }
                    ui->pushButton_lyric_translate->setText(lyric_translate[i].text);
                    if(!lyric_translate[i].text.contains("\n")){
                        dd.ui->label_lyric_translate->setText(lyric_translate[i].text);
                    }
                    else{
                        QStringList tem=lyric_translate[i].text.split("\n");
                        QString aaa;
                        for(int m=0;m<tem.count();++m){
                            aaa.append(tem.at(m)+" ");
                        }
                        dd.ui->label_lyric_translate->setText(adjust_text_overlength(aaa,dd.ui->label_lyric_translate,1));
                    }
                    if(lyric[i].color_num==0||lyric[i].color_num==conf.num){
                        ui->pushButton_lyric->setStyleSheet("color:black;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                        dd.ui->pushButton_lyric->setStyleSheet("color:#7f7f7f;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                    }
                    else if(lyric[i].color_num==1){
                        for(int u=0;u<conf.num;++u){
                            if(lyric[i].color[u]==true){
                                ui->pushButton_lyric->setStyleSheet("color:"+conf.member.at(u).left(7)+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                                dd.ui->pushButton_lyric->setStyleSheet("color:"+conf.member.at(u).left(7)+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                            }
                        }
                    }
                    else{
                        QString style_prefix="color:qlineargradient(spread:pad x1:0, y1:0, x2:1, y2:0, ";
                        QString style_suffix=");\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);";
                        QString temp="";
                        int progress=0;
                        for(int ii=0;ii<lyric[i].color_num;++ii){
                            temp+="stop:"+QString::number(ii*1.0/(lyric[i].color_num-1))+" ";
                            for(int iii=progress;iii<conf.num;++iii){
                                if(lyric[i].color[iii]==true){
                                    temp+=conf.member.at(iii).left(7)+",";
                                    iii++;//防止break导致的iii不变
                                    progress=iii;//一定要在iii++后再赋值，否则progress不变，一直卡在一个颜色
                                    break;
                                }
                            }
                        }
                        temp=temp.mid(0,temp.length()-1);
                        ui->pushButton_lyric->setStyleSheet(style_prefix+temp+style_suffix);
                        dd.ui->pushButton_lyric->setStyleSheet(style_prefix+temp+style_suffix);
                    }
                }

                //开始设置歌手显示
                QString singers="";
                if(lyric[i].color_num==conf.num){
                    for(int k=0;k<conf.num;++k){
                        singers+=conf.member.at(k).right(conf.member.at(k).length()-7)+"、";
                    }
                }
                else if(lyric[i].color_num==0) singers="、";
                else{
                    for(int k=0;k<conf.num;++k){
                        if(lyric[i].color[k]==true){
                            singers+=conf.member.at(k).right(conf.member.at(k).length()-7)+"、";
                        }
                    }
                }
                singers=singers.left(singers.length()-1);
                if(singers==""){
                    ui->label_singers->hide();
                }
                else if(ui->label_singers->text()==""){
                    ui->label_singers->show();
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

                    QString dda=lyric[valid_lyric-1].text;
                    dda.replace("color:#000000","color:#7f7f7f");
                    dda.replace("font-size:14pt;","font-size:13pt;");
                    //qDebug()<<dda; e.g.<span style=" font-size:14pt; font-weight:600; color:#55aaff;">aaaaa</span><span style=" font-size:14pt; font-weight:600; color:#ffaaff;">aaaaaaa</span>
                    QTextDocument htmll;
                    htmll.setHtml(dda);
                    QPixmap pixmapp(htmll.size().width(),htmll.size().height());
                    pixmapp.fill(Qt::transparent);
                    QPainter painterr(&pixmapp);
                    htmll.drawContents(&painterr,pixmapp.rect());
                    QIcon iconn(pixmapp);
                    dd.ui->pushButton_lyric_html->setText("");
                    dd.ui->pushButton_lyric_html->setIcon(iconn);
                    dd.ui->pushButton_lyric_html->setIconSize(pixmapp.rect().size());
                    dd.ui->pushButton_lyric->setText("");
                    if(!lyric_translate[valid_lyric-1].text.contains("\n")){
                        dd.ui->label_lyric_translate->setText(lyric_translate[valid_lyric-1].text);
                    }
                    else{
                        QStringList tem=lyric_translate[valid_lyric-1].text.split("\n");
                        QString aaa;
                        for(int m=0;m<tem.count();++m){
                            aaa.append(tem.at(m)+" ");
                        }
                        dd.ui->label_lyric_translate->setText(adjust_text_overlength(aaa,dd.ui->label_lyric_translate,1));
                    }
                }
                else{
                    ui->pushButton_lyric_html->setIcon(*empty_icon);
                    ui->pushButton_lyric->setText(lyric[valid_lyric-1].text);
                    ui->pushButton_lyric_translate->setText(lyric_translate[valid_lyric-1].text);

                    dd.ui->pushButton_lyric_html->setIcon(*empty_icon);
                    if(!lyric[valid_lyric-1].text.contains("\n")){
                        dd.ui->pushButton_lyric->setText(lyric[valid_lyric-1].text);
                    }
                    else{
                        QStringList tem=lyric[valid_lyric-1].text.split("\n");
                        QString aaa;
                        for(int m=0;m<tem.count();++m){
                            aaa.append(tem.at(m)+" ");
                        }
                        dd.ui->pushButton_lyric->setText(adjust_text_overlength(aaa,dd.ui->pushButton_lyric,1));
                    }
                    if(!lyric_translate[valid_lyric-1].text.contains("\n")){
                        dd.ui->label_lyric_translate->setText(lyric_translate[valid_lyric-1].text);
                    }
                    else{
                        QStringList tem=lyric_translate[valid_lyric-1].text.split("\n");
                        QString aaa;
                        for(int m=0;m<tem.count();++m){
                            aaa.append(tem.at(m)+" ");
                        }
                        dd.ui->label_lyric_translate->setText(adjust_text_overlength(aaa,dd.ui->label_lyric_translate,1));
                    }

                    //开始调色
                    if(lyric[valid_lyric-1].color_num==0){
                        ui->pushButton_lyric->setStyleSheet("color:black;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                        dd.ui->pushButton_lyric->setStyleSheet("color:#7f7f7f;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                    }
                    else if(lyric[valid_lyric-1].color_num==conf.num){
                        ui->pushButton_lyric->setStyleSheet("color:black;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                        dd.ui->pushButton_lyric->setStyleSheet("color:#7f7f7f;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                    }
                    else if(lyric[valid_lyric-1].color_num==1){
                        for(int u=0;u<conf.num;++u){
                            if(lyric[valid_lyric-1].color[u]==true){
                                ui->pushButton_lyric->setStyleSheet("color:"+conf.member.at(u).left(7)+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                                dd.ui->pushButton_lyric->setStyleSheet("color:"+conf.member.at(u).left(7)+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                            }
                        }
                    }
                    else{
                        QString style_prefix="color:qlineargradient(spread:pad x1:0, y1:0, x2:1, y2:0, ";
                        QString style_suffix=");\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);";
                        QString temp="";
                        int progress=0;
                        for(int ii=0;ii<lyric[valid_lyric-1].color_num;++ii){
                            temp+="stop:"+QString::number(ii*1.0/(lyric[valid_lyric-1].color_num-1))+" ";
                            for(int iii=progress;iii<conf.num;++iii){
                                if(lyric[valid_lyric-1].color[iii]==true){
                                    temp+=conf.member.at(iii).left(7)+",";
                                    iii++;//防止break导致的iii不变
                                    progress=iii;//一定要在iii++后再赋值，否则progress不变，一直卡在一个颜色
                                    break;
                                }
                            }
                        }
                        temp=temp.mid(0,temp.length()-1);
                        ui->pushButton_lyric->setStyleSheet(style_prefix+temp+style_suffix);
                        dd.ui->pushButton_lyric->setStyleSheet(style_prefix+temp+style_suffix);
                    }

                }

                //开始设置歌手显示
                QString singers="";
                if(lyric[valid_lyric-1].color_num==conf.num){
                    for(int k=0;k<conf.num;++k){
                        singers+=conf.member.at(k).right(conf.member.at(k).length()-7)+"、";
                    }
                }
                else if(lyric[valid_lyric-1].color_num==0) singers="、";
                else{
                    for(int k=0;k<conf.num;++k){
                        if(lyric[valid_lyric-1].color[k]==true){
                            singers+=conf.member.at(k).right(conf.member.at(k).length()-7)+"、";
                        }
                    }
                }
                singers=singers.left(singers.length()-1);
                if(singers==""){
                    ui->label_singers->hide();
                }
                else if(ui->label_singers->text()==""){
                    ui->label_singers->show();
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
    for(int i=0;i<199;++i){
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

                    QString dda=lyric[i].text;
                    dda.replace("color:#000000","color:#7f7f7f");
                    dda.replace("font-size:14pt;","font-size:13pt;");
                    //qDebug()<<dda; e.g.<span style=" font-size:14pt; font-weight:600; color:#55aaff;">aaaaa</span><span style=" font-size:14pt; font-weight:600; color:#ffaaff;">aaaaaaa</span>
                    QTextDocument htmll;
                    htmll.setHtml(dda);
                    QPixmap pixmapp(htmll.size().width(),htmll.size().height());
                    pixmapp.fill(Qt::transparent);
                    QPainter painterr(&pixmapp);
                    htmll.drawContents(&painterr,pixmapp.rect());
                    QIcon iconn(pixmapp);
                    dd.ui->pushButton_lyric_html->setText("");
                    dd.ui->pushButton_lyric_html->setIcon(iconn);
                    dd.ui->pushButton_lyric_html->setIconSize(pixmapp.rect().size());
                    dd.ui->pushButton_lyric->setText("");
                    if(!lyric_translate[i].text.contains("\n")){
                        dd.ui->label_lyric_translate->setText(lyric_translate[i].text);
                    }
                    else{
                        QStringList tem=lyric_translate[i].text.split("\n");
                        QString aaa;
                        for(int m=0;m<tem.count();++m){
                            aaa.append(tem.at(m)+" ");
                        }
                        dd.ui->label_lyric_translate->setText(adjust_text_overlength(aaa,dd.ui->label_lyric_translate,1));
                    }
                }
                else{
                    ui->pushButton_lyric_html->setIcon(*empty_icon);
                    ui->pushButton_lyric->setText(lyric[i].text);
                    ui->pushButton_lyric_translate->setText(lyric_translate[i].text);

                    dd.ui->pushButton_lyric_html->setIcon(*empty_icon);
                    if(!lyric[i].text.contains("\n")){
                        dd.ui->pushButton_lyric->setText(lyric[i].text);
                    }
                    else{
                        QStringList tem=lyric[i].text.split("\n");
                        QString aaa;
                        for(int m=0;m<tem.count();++m){
                            aaa.append(tem.at(m)+" ");
                        }
                        dd.ui->pushButton_lyric->setText(adjust_text_overlength(aaa,dd.ui->pushButton_lyric,1));
                    }
                    if(!lyric_translate[i].text.contains("\n")){
                        dd.ui->label_lyric_translate->setText(lyric_translate[i].text);
                    }
                    else{
                        QStringList tem=lyric_translate[i].text.split("\n");
                        QString aaa;
                        for(int m=0;m<tem.count();++m){
                            aaa.append(tem.at(m)+" ");
                        }
                        dd.ui->label_lyric_translate->setText(adjust_text_overlength(aaa,dd.ui->label_lyric_translate,1));
                    }
                    if(lyric[i].color_num==0){
                        ui->pushButton_lyric->setStyleSheet("color:black;s\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                        dd.ui->pushButton_lyric->setStyleSheet("color:#7f7f7f;s\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                    }
                    else if(lyric[i].color_num==conf.num){
                        ui->pushButton_lyric->setStyleSheet("color:black;s\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                        dd.ui->pushButton_lyric->setStyleSheet("color:#7f7f7f;s\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                    }
                    else if(lyric[i].color_num==1){
                        for(int u=0;u<conf.num;++u){
                            if(lyric[i].color[u]==true){
                                ui->pushButton_lyric->setStyleSheet("color:"+conf.member.at(u).left(7)+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                                dd.ui->pushButton_lyric->setStyleSheet("color:"+conf.member.at(u).left(7)+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                            }
                        }
                    }
                    else{
                        QString style_prefix="color:qlineargradient(spread:pad x1:0, y1:0, x2:1, y2:0, ";
                        QString style_suffix=");\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);";
                        QString temp="";
                        int progress=0;
                        for(int ii=0;ii<lyric[i].color_num;++ii){
                            temp+="stop:"+QString::number(ii*1.0/(lyric[i].color_num-1))+" ";
                            for(int iii=progress;iii<conf.num;++iii){
                                if(lyric[i].color[iii]==true){
                                    temp+=conf.member.at(iii).left(7)+",";
                                    iii++;//防止break导致的iii不变
                                    progress=iii;//一定要在iii++后再赋值，否则progress不变，一直卡在一个颜色
                                    break;
                                }
                            }
                        }
                        temp=temp.mid(0,temp.length()-1);
                        ui->pushButton_lyric->setStyleSheet(style_prefix+temp+style_suffix);
                        dd.ui->pushButton_lyric->setStyleSheet(style_prefix+temp+style_suffix);
                    }
                }


                //开始设置歌手显示
                QString singers="";
                if(lyric[i].color_num==conf.num){
                    for(int k=0;k<conf.num;++k){
                        singers+=conf.member.at(k).right(conf.member.at(k).length()-7)+"、";
                    }
                }
                else if(lyric[i].color_num==0) singers="、";
                else{
                    for(int k=0;k<conf.num;++k){
                        if(lyric[i].color[k]==true){
                            singers+=conf.member.at(k).right(conf.member.at(k).length()-7)+"、";
                        }
                    }
                }
                singers=singers.left(singers.length()-1);
                if(singers==""){
                    ui->label_singers->hide();
                }
                else if(ui->label_singers->text()==""){
                    ui->label_singers->show();
                }
                ui->label_singers->setText(singers);
            }
            break;
        }
        else if(real_time>=lyric[valid_lyric-1].time){
            if(ui->pushButton_lyric->text()!=lyric[valid_lyric-1].text){//最后一句歌词
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

                    QString dda=lyric[valid_lyric-1].text;
                    dda.replace("color:#000000","color:#7f7f7f");
                    dda.replace("font-size:14pt;","font-size:13pt;");
                    //qDebug()<<dda; e.g.<span style=" font-size:14pt; font-weight:600; color:#55aaff;">aaaaa</span><span style=" font-size:14pt; font-weight:600; color:#ffaaff;">aaaaaaa</span>
                    QTextDocument htmll;
                    htmll.setHtml(dda);
                    QPixmap pixmapp(htmll.size().width(),htmll.size().height());
                    pixmapp.fill(Qt::transparent);
                    QPainter painterr(&pixmapp);
                    htmll.drawContents(&painterr,pixmapp.rect());
                    QIcon iconn(pixmapp);
                    dd.ui->pushButton_lyric_html->setText("");
                    dd.ui->pushButton_lyric_html->setIcon(iconn);
                    dd.ui->pushButton_lyric_html->setIconSize(pixmapp.rect().size());
                    dd.ui->pushButton_lyric->setText("");
                    if(!lyric_translate[valid_lyric-1].text.contains("\n")){
                        dd.ui->label_lyric_translate->setText(lyric_translate[valid_lyric-1].text);
                    }
                    else{
                        QStringList tem=lyric_translate[valid_lyric-1].text.split("\n");
                        QString aaa;
                        for(int m=0;m<tem.count();++m){
                            aaa.append(tem.at(m)+" ");
                        }
                        dd.ui->label_lyric_translate->setText(adjust_text_overlength(aaa,dd.ui->label_lyric_translate,1));
                    }
                }
                else{
                    ui->pushButton_lyric_html->setIcon(*empty_icon);
                    ui->pushButton_lyric->setText(lyric[valid_lyric-1].text);
                    ui->pushButton_lyric_translate->setText(lyric_translate[valid_lyric-1].text);

                    dd.ui->pushButton_lyric_html->setIcon(*empty_icon);
                    if(!lyric[valid_lyric-1].text.contains("\n")){
                        dd.ui->pushButton_lyric->setText(lyric[valid_lyric-1].text);
                    }
                    else{
                        QStringList tem=lyric[valid_lyric-1].text.split("\n");
                        QString aaa;
                        for(int m=0;m<tem.count();++m){
                            aaa.append(tem.at(m)+" ");
                        }
                        dd.ui->pushButton_lyric->setText(adjust_text_overlength(aaa,dd.ui->pushButton_lyric,1));
                    }
                    if(!lyric_translate[valid_lyric-1].text.contains("\n")){
                        dd.ui->label_lyric_translate->setText(lyric_translate[valid_lyric-1].text);
                    }
                    else{
                        QStringList tem=lyric_translate[valid_lyric-1].text.split("\n");
                        QString aaa;
                        for(int m=0;m<tem.count();++m){
                            aaa.append(tem.at(m)+" ");
                        }
                        dd.ui->label_lyric_translate->setText(adjust_text_overlength(aaa,dd.ui->label_lyric_translate,1));
                    }

                    //开始调色
                    if(lyric[valid_lyric-1].color_num==0){
                        ui->pushButton_lyric->setStyleSheet("color:black;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                        dd.ui->pushButton_lyric->setStyleSheet("color:#7f7f7f;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                    }
                    else if(lyric[valid_lyric-1].color_num==conf.num){
                        ui->pushButton_lyric->setStyleSheet("color:black;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                        dd.ui->pushButton_lyric->setStyleSheet("color:#7f7f7f;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                    }
                    else if(lyric[valid_lyric-1].color_num==1){
                        for(int u=0;u<conf.num;++u){
                            if(lyric[valid_lyric-1].color[u]==true){
                                ui->pushButton_lyric->setStyleSheet("color:"+conf.member.at(u).left(7)+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                                dd.ui->pushButton_lyric->setStyleSheet("color:"+conf.member.at(u).left(7)+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
                            }
                        }
                    }
                    else{
                        QString style_prefix="color:qlineargradient(spread:pad x1:0, y1:0, x2:1, y2:0, ";
                        QString style_suffix=");\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);";
                        QString temp="";
                        int progress=0;
                        for(int ii=0;ii<lyric[valid_lyric-1].color_num;++ii){
                            temp+="stop:"+QString::number(ii*1.0/(lyric[valid_lyric-1].color_num-1))+" ";
                            for(int iii=progress;iii<conf.num;++iii){
                                if(lyric[valid_lyric-1].color[iii]==true){
                                    temp+=conf.member.at(iii).left(7)+",";
                                    iii++;//防止break导致的iii不变
                                    progress=iii;//一定要在iii++后再赋值，否则progress不变，一直卡在一个颜色
                                    break;
                                }
                            }
                        }
                        temp=temp.mid(0,temp.length()-1);
                        ui->pushButton_lyric->setStyleSheet(style_prefix+temp+style_suffix);
                        dd.ui->pushButton_lyric->setStyleSheet(style_prefix+temp+style_suffix);
                    }
                }

                //开始设置歌手显示
                QString singers="";
                if(lyric[valid_lyric-1].color_num==conf.num){
                    for(int k=0;k<conf.num;++k){
                        singers+=conf.member.at(k).right(conf.member.at(k).length()-7)+"、";
                    }
                }
                else if(lyric[valid_lyric-1].color_num==0) singers="、";
                else{
                    for(int k=0;k<conf.num;++k){
                        if(lyric[valid_lyric-1].color[k]==true){
                            singers+=conf.member.at(k).right(conf.member.at(k).length()-7)+"、";
                        }
                    }
                }
                singers=singers.left(singers.length()-1);
                if(singers==""){
                    ui->label_singers->hide();
                }
                else if(ui->label_singers->text()==""){
                    ui->label_singers->show();
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
    qDebug()<<"got duration:"<<duration;
}

void MainWindow::SaveHDCToFile(libZPlay::TID3InfoExW id3_info){
    libZPlay::TID3PictureW pic;
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

bool MainWindow::read_lyric(QString path,int mode){
    if(mode==0){//原文
        QFile lrc(path);
        if(lrc.open(QIODevice::ReadOnly)){
            QString all=lrc.readAll();
            QStringList list=all.split("\n");
            if(list.at(0).at(0)!="["){
                list.removeFirst();//不将第一行的config id作为歌词读取
            }
            QRegularExpression re("\\[(\\d+)?:(\\d+)?(\\.\\d+)?(\\S+)?\\]");
            int lyric_seq=0;
            for(int i=0;i<list.count();++i){
                QString text;
                QString color;
                qint64 lrc_time;

                QRegularExpressionMatch mat=re.match(list.at(i));
                lrc_time=mat.captured(1).toInt()*60000+mat.captured(2).toInt()*1000+mat.captured(3).mid(1).toInt()*10;
                text=QString(list.at(i)).right(QString(list.at(i)).length()-mat.capturedLength());
                color=text.mid(1,conf.num*2-1);
                text=text.right(text.length()-conf.num*2-1);

                int color_num=0;
                bool colorr[conf.num]={0};
                QStringList color_list=color.split(",");
                for(int u=0;u<conf.num;++u){
                    if(color_list[u]=="1"){
                        ++color_num;
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
                    for(int u=0;u<conf.num;++u){
                        if(colorr[u]==true){
                            lyric[lyric_seq].color[u]=true;
                        }
                    }
                    ++lyric_seq;
                }
            }
            return true;
        }
        else{
            for(int i=0;i<200;++i){
                lyric[i].time=0;
                lyric[i].text="暂无歌词";
                lyric[i].color_num=0;
            }
            return false;
        }
    }
    else if(mode==1){//翻译
        QFile lrc(path);
        if(lrc.open(QIODevice::ReadOnly)){
            QString all=lrc.readAll();
            QStringList list=all.split("\n");
            QRegularExpression re("\\[(\\d+)?:(\\d+)?(\\.\\d+)?(\\S+)?\\]");
            int lyric_seq=0;
            for(int i=0;i<list.count();++i){
                QString text;
                QString color;
                qint64 lrc_time;

                QRegularExpressionMatch mat=re.match(list.at(i));
                lrc_time=mat.captured(1).toInt()*60000+mat.captured(2).toInt()*1000+mat.captured(3).mid(1).toInt()*10;
                text=QString(list.at(i)).right(QString(list.at(i)).length()-mat.capturedLength());
                color=text.mid(1,conf.num*2-1);
                text=text.right(text.length()-conf.num*2-1);

                int color_num=0;
                bool colorr[conf.num]={0};
                QStringList color_list=color.split(",");
                for(int u=0;u<conf.num;++u){
                    if(color_list[u]=="1"){
                        ++color_num;
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
                    for(int u=0;u<conf.num;++u){
                        if(colorr[u]==true){
                            lyric_translate[lyric_seq].color[u]=true;
                        }
                    }
                    ++lyric_seq;
                }
            }
            return true;
        }
        else{
            for(int i=0;i<200;++i){
                lyric_translate[i].time=0;
                lyric_translate[i].text="暂无翻译";
                lyric_translate[i].color_num=0;
            }
            return false;
        }
    }
    else if(mode==2){//.lrc文件读取
        QFile lrc(path);
        if(lrc.open(QIODevice::ReadOnly)){
            QString all=lrc.readAll();
            QStringList list=all.split("\n");
            QRegularExpression re("\\[(\\d+)?:(\\d+)?(\\.\\d+)?(\\S+)?\\]");
            int lyric_seq=0;
            qDebug()<<"list.count()="<<list.count();
            for(int i=0;i<list.count();++i){
                QString text;
                qint64 lrc_time;

                QRegularExpressionMatch mat=re.match(list.at(i));
                lrc_time=mat.captured(1).toInt()*60000+mat.captured(2).toInt()*1000+mat.captured(3).mid(1).toInt()*10;
                text=list.at(i).right(QString(list.at(i)).length()-mat.capturedLength());

                for(int k=0;k<9;++k){
                    lyric[i].color[k]=false;
                }
                if(text!=""){
                    lyric[lyric_seq].time=lrc_time;
                    lyric[lyric_seq].text=adjust_text_overlength(text,ui->pushButton_lyric,0);
                    lyric[lyric_seq].color_num=0;
                    ++lyric_seq;
                }
            }
            return true;
        }
        else{
            for(int i=0;i<200;++i){
                lyric[i].time=0;
                lyric[i].text="暂无歌词";
                lyric[i].color_num=0;
            }
            return false;
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
    }
    else if(player->state()==QMediaPlayer::PlayingState){
        player->pause();
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
    title="unknown";
    artist="unknown";
    album="unknown";
    zplayer=libZPlay::CreateZPlay();
    libZPlay::TID3InfoExW id3_info;
    //const wchar_t* wstr=reinterpret_cast<const wchar_t>(path.utf16());
    wchar_t* wstr=new wchar_t[1024];
    for(int i=0;i<1024;++i){
        wstr[i]=0;
    }
    path.toWCharArray(wstr);
    if(zplayer->OpenFileW(wstr,libZPlay::sfAutodetect)){
        if(zplayer->LoadID3ExW(&id3_info,1)){
            if(isNeedAlbumCover==true) SaveHDCToFile(id3_info);
            title=QString::fromWCharArray(id3_info.Title);
            artist=QString::fromWCharArray(id3_info.Artist);
            album=QString::fromWCharArray(id3_info.Album);
        }
        else{
            title="unknown";
            artist="unknown";
            album="unknown";
            qDebug()<<"not a ID3";
        }
    }
    else{
        qDebug()<<"zplayer can't open this file!";
    }
    zplayer->Close();
    zplayer->Release();
    if(isNeedAlbumCover==true){
        ui->label_info_title->setText(adjust_text_overlength(title,ui->label_info_title,1));
        ui->label_info_artist->setText(adjust_text_overlength("歌手："+artist,ui->label_info_artist,1));
        ui->label_info_album->setText(adjust_text_overlength("专辑："+album,ui->label_info_album,1));
    }
    //读取封面并显示
    if(isNeedAlbumCover==true) ui->widget_cover->setStyleSheet("border-image:url("+QApplication::applicationDirPath()+"/album.png);");
}

void MainWindow::load_single_song(QString name){
    QString path_audio=QApplication::applicationDirPath()+"/songs/"+name;
    QStringList list_temp=name.split(".");
    QString list_temp_name;
    if(list_temp.count()>=2){
        for(int i=0;i<list_temp.count()-1;++i){
            list_temp_name+=list_temp.at(i)+".";
        }
        list_temp_name=list_temp_name.left(list_temp_name.count()-1);
        //qDebug()<<"list_temp_name:"<<list_temp_name;
    }
    else{
        qDebug()<<"an error in name_list has been found!";
    }

    QString path_lyric=QApplication::applicationDirPath()+"/songs/"+list_temp_name+".ll";
    QString path_lyric_backup=QApplication::applicationDirPath()+"/songs/"+list_temp_name+".lrc";
    QString path_lyric_translate=QApplication::applicationDirPath()+"/songs/"+list_temp_name+".trans";

    //以下是读取主题
    QFile temp(path_lyric);
    if(temp.open(QIODevice::ReadOnly)){
        QString id_temp=temp.readLine();
        for(int i=0;i<id_temp.count();++i){//删去最后的n个换行符
            if(id_temp.at(i)=="\r"||id_temp.at(i)=="\n"){
                id_temp=id_temp.left(i);
                break;
            }
        }
        //qDebug()<<"id_temp:"<<id_temp;
        if(id_temp.at(0)=="["){
            get_config(QApplication::applicationDirPath()+"/resources/default.config");
        }
        else if(conf.id!=id_temp){
            get_config(QApplication::applicationDirPath()+"/resources/"+id_temp+".config");
        }
    }
    else{
        get_config(QApplication::applicationDirPath()+"/resources/default.config");
    }
    temp.close();

    //以下获取歌曲元数据并提取封面
    get_meta(path_audio,true);

    //以下是调整托盘的标题
    mSysTrayIcon->setToolTip(title+" - "+artist);//当鼠标移动到托盘上的图标时，会显示此处设置的内容

    //以下是更改设置中滚动标题的内容
    QString info=title+" - "+artist+"  ";
    ui->label_settings_info->setText(info);
    ui->label_settings_info->setInterVal(100);

    //以下是调整播放列表滚动条的值
    if(isRandomPlay==false){
        if(ui->scrollArea->verticalScrollBar()->value()+320<80*play_progress){
            ui->scrollArea->verticalScrollBar()->setValue(80*play_progress-320);
        }
        else if(ui->scrollArea->verticalScrollBar()->value()>80*play_progress){
            ui->scrollArea->verticalScrollBar()->setValue(80*play_progress);
        }
    }
    else{
        if(ui->scrollArea->verticalScrollBar()->value()+320<80*random_seq.at(randomplay_progress)){
            ui->scrollArea->verticalScrollBar()->setValue(80*random_seq.at(randomplay_progress)-320);
        }
        else if(ui->scrollArea->verticalScrollBar()->value()>80*random_seq.at(randomplay_progress)){
            ui->scrollArea->verticalScrollBar()->setValue(80*random_seq.at(randomplay_progress));
        }
    }

    for(int i=0;i<200;++i){
        lyric[i].color_num=0;
        lyric[i].text="";
        lyric[i].time=-1;
        lyric_translate[i].color_num=0;
        lyric_translate[i].text="";
        lyric_translate[i].time=-1;
        for(int ii=0;ii<9;++ii){
            lyric[i].color[ii]=false;
            lyric_translate[i].color[ii]=false;
        }
    }
    if(!read_lyric(path_lyric,0)){
        for(int i=0;i<200;++i){
            lyric[i].color_num=0;
            lyric[i].text="";
            lyric[i].time=-1;
        }
        read_lyric(path_lyric_backup,2);//兼容.ll和.lrc歌词文件
    }
    read_lyric(path_lyric_translate,1);
    ui->pushButton_lyric_translate->setText(lyric_translate[0].text);
    for(int i=0;i<200;++i){
        if(lyric[i].text==""&&lyric[i].time==-1){
            valid_lyric=i;
            break;
        }
    }
    qDebug()<<"finish loading!";
}

void MainWindow::QSliderProClicked(){
    int pos=ui->horizontalSlider->value();
    disconnect(player,SIGNAL(positionChanged(qint64)),this,SLOT(time_change(qint64)));
    player->setPosition(pos);
    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(time_change(qint64)));
}

void MainWindow::change_song(QMediaPlayer::MediaStatus status){
    //qDebug()<<"MediaStatus:"<<status;
    if(status==QMediaPlayer::EndOfMedia){
        if(playlist->playbackMode()==QMediaPlaylist::CurrentItemInLoop){

        }
        else if(playlist->playbackMode()==QMediaPlaylist::Loop&&isRandomPlay==false){
            for(int i=0;i<200;++i){
                lyric[i].time=-1;
                lyric[i].text="";
                lyric[i].color_num=0;
                for(int u=0;u<9;++u){
                    lyric[i].color[u]=false;
                }
            }
            if(name_list.length()>play_progress+1){
                ++play_progress;
            }
            else if(play_progress+1==name_list.length()){
                play_progress=0;
            }
            ui->label_singers->setText("");
            ui->label_singers->hide();
            load_single_song(name_list[play_progress]);
            for(int i=0;i<playlist_buttons.count();++i){
                playlist_buttons.at(i)->setStyleSheet("border-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
            }
            playlist_buttons.at(play_progress)->setStyleSheet("color:"+conf.theme_color+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
            for(int i=0;i<playlist_buttons.count();++i){
                playlist_buttons.at(i)->show();
            }
        }
        else if(playlist->playbackMode()==QMediaPlaylist::Loop&&isRandomPlay==true){
            for(int i=0;i<200;++i){
                lyric[i].time=-1;
                lyric[i].text="";
                lyric[i].color_num=0;
                for(int u=0;u<9;++u){
                    lyric[i].color[u]=false;
                }
            }
            ui->label_singers->setText("");
            ui->label_singers->hide();
            ++randomplay_progress;
            load_single_song(name_list.at(random_seq.at(randomplay_progress)));
            for(int i=0;i<playlist_buttons.count();++i){
                playlist_buttons.at(i)->setStyleSheet("border-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
            }
            playlist_buttons.at(random_seq.at(randomplay_progress))->setStyleSheet("color:"+conf.theme_color+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
            for(int i=0;i<playlist_buttons.count();++i){
                playlist_buttons.at(i)->show();
            }
            playlist->setCurrentIndex(random_seq.at(randomplay_progress));
        }
    }
}

void MainWindow::on_Slider_volume_valueChanged(int value)
{
    player->setVolume(value);
    ui->label_volume->setText(QString::number(value));
    QFont font(font_string,10);
    ui->label_volume->setFont(font);
}

void MainWindow::SliderVolumeClicked(){
    int pos=ui->Slider_volume->value();
    player->setVolume(pos);
}

void MainWindow::on_pushButton_next_clicked()
{
    if(playlist->playbackMode()==QMediaPlaylist::Loop&&isRandomPlay==false){
        if(play_progress+1==name_list.count()){
            play_progress=0;
        }
        else if(play_progress+1<name_list.count()){
            ++play_progress;
        }
        playlist->setCurrentIndex(play_progress);
        load_single_song(name_list[play_progress]);
        for(int i=0;i<playlist_buttons.count();++i){
            playlist_buttons.at(i)->setStyleSheet("border-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
        }
        playlist_buttons.at(play_progress)->setStyleSheet("color:"+conf.theme_color+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
        for(int i=0;i<playlist_buttons.count();++i){
            playlist_buttons.at(i)->show();
        }
    }
    else if(playlist->playbackMode()==QMediaPlaylist::Loop&&isRandomPlay==true){
        if(randomplay_progress+1==name_list.count()){
            randomplay_progress=0;
        }
        else if(randomplay_progress+1<name_list.count()){
            ++randomplay_progress;
        }
        playlist->setCurrentIndex(random_seq.at(randomplay_progress));
        load_single_song(name_list[random_seq.at(randomplay_progress)]);
        for(int i=0;i<playlist_buttons.count();++i){
            playlist_buttons.at(i)->setStyleSheet("border-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
        }
        playlist_buttons.at(random_seq.at(randomplay_progress))->setStyleSheet("color:"+conf.theme_color+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
        for(int i=0;i<playlist_buttons.count();++i){
            playlist_buttons.at(i)->show();
        }
    }
    ui->pushButton_lyric->setText("");
    dd.ui->pushButton_lyric->setText("");
    ui->label_singers->setText("");
    ui->label_singers->hide();
    ui->pushButton_lyric_html->setIcon(*empty_icon);
    dd.ui->pushButton_lyric_html->setIcon(*empty_icon);
}

void MainWindow::player_state_change(QMediaPlayer::State state){
    if(state==QMediaPlayer::PausedState){
        ui->pushButton_play->setStyleSheet("border-image: url(:/new/prefix1/player-play-circle.png);");
    }
    else if(state==QMediaPlayer::PlayingState){
        ui->pushButton_play->setStyleSheet("border-image: url(:/new/prefix1/player-pause-circle.png);");
    }
}

QString MainWindow::adjust_text_overlength(QString text,QPushButton* obj,int mode){//自动换行 但不修改含html标签的字符串
    if(mode==0){//自动换行模式
        if(!text.contains("<")){
            QString result=text;
            QString final;
            QStringList list=result.split("\n");
            result=text;
            int buttonwidth=obj->width();
            for(int u=0;u<list.count();++u){
                int textwidth=obj->fontMetrics().width(list.at(u));
                if(textwidth>=buttonwidth){
                    QStringList temp=text.split(" ");
                    if(temp.count()>1){//有空格
                        int sum=0;
                        for(int i=0;i<temp.count();++i){
                            sum+=temp.at(i).length();
                        }
                        int l=0,pos=0;
                        for(int i=0;i<temp.count()-1;++i){
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
                    for(int p=0;p<ooo.count();++p){
                        if(obj->fontMetrics().width(ooo.at(p))>=buttonwidth){
                            copy.append(adjust_text_overlength(ooo.at(p),obj,0));
                        }
                        else copy.append(ooo.at(p));
                    }
                    for(int p=0;p<copy.count();++p){
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
            for(int i=0;i<text.length();++i){
                temp.append(text.at(i));
                tempwidth=obj->fontMetrics().width(temp);
                int ooowidth=obj->fontMetrics().width("...");
                if(tempwidth+ooowidth>buttonwidth){
                    QString res=temp.left(temp.length()-1)+"...";
                    return res;
                }
            }
        }
    }
}

QString MainWindow::adjust_text_overlength(QString text,QLabel* obj,int mode){
    if(mode==0){//自动换行模式
        if(!text.contains("<")){
            QString result=text;
            QString final;
            QStringList list=result.split("\n");
            result=text;
            int buttonwidth=obj->width();
            for(int u=0;u<list.count();++u){
                int textwidth=obj->fontMetrics().width(list.at(u));
                if(textwidth>=buttonwidth){
                    QStringList temp=text.split(" ");
                    if(temp.count()>1){//有空格
                        int sum=0;
                        for(int i=0;i<temp.count();++i){
                            sum+=temp.at(i).length();
                        }
                        int l=0,pos=0;
                        for(int i=0;i<temp.count()-1;++i){
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
                    for(int p=0;p<ooo.count();++p){
                        if(obj->fontMetrics().width(ooo.at(p))>=buttonwidth){
                            copy.append(adjust_text_overlength(ooo.at(p),obj,0));
                        }
                        else copy.append(ooo.at(p));
                    }
                    for(int p=0;p<copy.count();++p){
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
            for(int i=0;i<text.length();++i){
                temp.append(text.at(i));
                tempwidth=obj->fontMetrics().width(temp);
                int ooowidth=obj->fontMetrics().width("...");
                if(tempwidth+ooowidth>buttonwidth){
                    QString res=temp.left(temp.length()-1)+"...";
                    return res;
                }
            }
        }
    }
}

void MainWindow::on_pushButton_last_clicked()
{
    if(playlist->playbackMode()==QMediaPlaylist::Loop&&isRandomPlay==false){
        if(play_progress==0){
            play_progress=name_list.count()-1;
        }
        else if(play_progress<name_list.count()&&play_progress>0){
            --play_progress;
        }
        playlist->setCurrentIndex(play_progress);
        load_single_song(name_list[play_progress]);
        for(int i=0;i<playlist_buttons.count();++i){
            playlist_buttons.at(i)->setStyleSheet("border-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
        }
        playlist_buttons.at(play_progress)->setStyleSheet("color:"+conf.theme_color+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
        for(int i=0;i<playlist_buttons.count();++i){
            playlist_buttons.at(i)->show();
        }
    }
    else if(playlist->playbackMode()==QMediaPlaylist::Loop&&isRandomPlay==true){
        if(randomplay_progress==0){
            randomplay_progress=name_list.count()-1;
        }
        else if(randomplay_progress<name_list.count()&&randomplay_progress>0){
            --randomplay_progress;
        }
        playlist->setCurrentIndex(random_seq.at(randomplay_progress));
        load_single_song(name_list[random_seq.at(randomplay_progress)]);
        for(int i=0;i<playlist_buttons.count();++i){
            playlist_buttons.at(i)->setStyleSheet("border-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
        }
        playlist_buttons.at(random_seq.at(randomplay_progress))->setStyleSheet("color:"+conf.theme_color+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
        for(int i=0;i<playlist_buttons.count();++i){
            playlist_buttons.at(i)->show();
        }
    }
    ui->pushButton_lyric->setText("");
    dd.ui->pushButton_lyric->setText("");
    ui->label_singers->setText("");
    ui->label_singers->hide();
    ui->pushButton_lyric_html->setIcon(*empty_icon);
    dd.ui->pushButton_lyric_html->setIcon(*empty_icon);
}

void MainWindow::playlist_buttons_clicked(int seq){
    if(playlist_buttons.at(seq)->isRightClicked==false){
        for(int i=0;i<playlist_buttons.count();++i){
            playlist_buttons.at(i)->setStyleSheet("border-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
        }
        ui->pushButton_lyric->setText("");
        ui->pushButton_lyric->setStyleSheet("color:black;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
        ui->pushButton_lyric_html->setIcon(*empty_icon);

        dd.ui->pushButton_lyric->setText("");
        dd.ui->pushButton_lyric->setStyleSheet("color:#7f7f7f;\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);");
        dd.ui->pushButton_lyric_html->setIcon(*empty_icon);

        if(isRandomPlay==false){
            play_progress=seq;
            load_single_song(name_list.at(seq));
            playlist_buttons.at(seq)->setStyleSheet("color:"+conf.theme_color+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
            playlist->setCurrentIndex(seq);
        }
        else{
            for(int i=0;i<name_list.count();++i){
                if(random_seq.at(i)==seq){
                    randomplay_progress=i;
                    break;
                }
            }
            load_single_song(name_list.at(seq));
            playlist_buttons.at(seq)->setStyleSheet("color:"+conf.theme_color+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
            playlist->setCurrentIndex(seq);
        }
    }
    else{
        //

        playlist_buttons.at(seq)->isRightClicked=false;
    }

}

void MainWindow::on_pushButton_minimize_clicked()
{
    this->showMinimized();
}

void MainWindow::on_pushButton_close_clicked()
{
    QFile data(QApplication::applicationDirPath()+"/resources/userdata");
    if(data.open(QIODevice::WriteOnly)){
        QString temp=QString::number(ui->Slider_volume->value())+"\n";
        if(isAutoPlay==false) temp+="0\n";
        else temp+="1\n";
        if(isTray==false) temp+="0\n";
        else temp+="1\n";
        if(dd.is_locked==false) temp+="0\n";
        else temp+="1\n";
        temp+=QString::number(dd.geometry().x())+" "+QString::number(dd.geometry().y())+"\n";
        if(isAutoShowDesktopLyric==false) temp+="0\n";
        else temp+="1\n";
        data.write(temp.toUtf8());
    }
    data.close();

    QFile config(QApplication::applicationDirPath()+"/resources/config");
    if(config.open(QIODevice::WriteOnly)){
        QString temp;
        if(isQuickSelect==false) temp+="0\n";
        else temp+="1\n";
        config.write(temp.toUtf8());
    }
    config.close();

    if(isTray==true){
        this->setVisible(false);
    }
    else{
        mSysTrayIcon->hide();
        this->close();
    }
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

        random_seq.clear();
        for(int i=0;i<name_list.count();++i){
            random_seq.append(i);
        }
        qsrand(QTime::currentTime().msec());
        for(int i=name_list.count()-1;i>0;--i){
            int r=qrand()%i;
            int t=random_seq[r];
            random_seq[r]=random_seq[i];
            random_seq[i]=t;
        }
        for(int i=0;i<name_list.count();++i){
            if(random_seq.at(i)==play_progress){
                randomplay_progress=i;
                break;
            }
        }
    }
    else if(playlist->playbackMode()==QMediaPlaylist::Loop&&isRandomPlay==true){//从随机播放到列表循环
        ui->pushButton_playmode->setStyleSheet("border-image: url(:/new/prefix1/loop.png);");
        isRandomPlay=false;
    }
}

void MainWindow::get_config(QString id){
    QFile con(id);
    conf.member.clear();
    if(con.open(QIODevice::ReadOnly)){
        QString text=con.readAll();
        QStringList t=text.split("\n");
        conf.theme_color=t.at(0);
        conf.title=t.at(1);
        conf.num=t.at(2).toInt();
        conf.icon_addr=QApplication::applicationDirPath()+"/resources/"+t.at(3);
        for(int i=4;i<t.count();++i){
            QString temp=t.at(i);
            temp=temp.left(temp.length()-1);
            conf.member.append(temp);//写config文件时最后一行一定要再留一个空换行
        }
    }
    else{
        get_config(QApplication::applicationDirPath()+"/resources/default.config");
        return;
    }
    conf.id=con.fileName().left(con.fileName().length()-7);
    conf.id=conf.id.right(conf.id.length()-QApplication::applicationDirPath().length()-11);
    conf.theme_color=conf.theme_color.left(conf.theme_color.length()-1);
    conf.title=conf.title.left(conf.title.length()-1);
    conf.icon_addr=conf.icon_addr.left(conf.icon_addr.length()-1);

    con.close();
    //qDebug()<<"conf.theme_color:"<<conf.theme_color<<"\nconf.title:"<<conf.title<<"\nconf.num:"<<conf.num<<"\nconf.icon_addr:"<<conf.icon_addr;

    ui->widget_title->setStyleSheet("background-color:"+conf.theme_color+";");
    ui->horizontalSlider->setStyleSheet("QSlider::groove:horizontal {\nborder: 1px solid #d9d9d9;\nborder-radius:0.5px;\nheight: 3px;\nbackground-color:#d9d9d9;\nmargin: 2px 0;\n}\n\nQSlider::handle:horizontal {\nbackground-color:"+conf.theme_color+";\nwidth: 10px;\nmargin: -4px 0;\nborder-radius: 5px;\n}\n\nQSlider::sub-page:horizontal {\nbackground-color:"+conf.theme_color+";\nmargin:2px 0;\nborder-radius:1px;\n}");
    ui->Slider_volume->setStyleSheet("QSlider::groove:vertical {\nborder: 1px solid #d9d9d9;\nwidth: 3px;\nbackground-color:#d9d9d9;\nmargin: 0 2px;\n}\n\nQSlider::handle:vertical {\nbackground-color:"+conf.theme_color+";\nheight: 10px;\nmargin: 0 -4px;\nborder-radius: 5px;\n}\n\nQSlider::add-page:vertical {\nbackground-color:"+conf.theme_color+";\nmargin:0 2px;\n}");
    this->setStyleSheet("QScrollBar:vertical {\nbackground:#f5f5f5;\nwidth: 10px;\n}\nQScrollBar::handle:vertical {\nbackground: "+conf.theme_color+";\nborder-radius:5px;\n}\nQScrollBar::add-line:vertical {\nbackground: #f5f5f5;\nheight: 20px;\nsubcontrol-position: bottom;\nsubcontrol-origin: margin;\n}\n\nQScrollBar::sub-line:vertical {\nbackground: #f5f5f5;\nheight: 20px;\nsubcontrol-position: top;\nsubcontrol-origin: margin;\n}\n\nQScrollArea{\nborder:0px;\n}");
    this->setWindowTitle(conf.title);
    if(conf.icon_addr!=QApplication::applicationDirPath()+"/resources/none"){
        ui->widget_icon->setStyleSheet("border-image: url("+conf.icon_addr+");");
    }
    else{
        ui->widget_icon->setStyleSheet("background-color:rgba(0,0,0,0);");
    }
    if(dd.isVisible()) ui->pushButton_DesktopLyric->setStyleSheet("background-color: rgba(0,0,0,0);\ncolor:"+conf.theme_color+";");

    int count=groupboxes.count();
    for(int p=0;p<count;++p){
        groupboxes.at(p)->setVisible(false);
    }
    while(groupboxes.isEmpty()!=true){
        groupboxes.removeFirst();
    }
    if(conf.member.count()>1){//.config文件最后有一个空的换行导致conf.member至少有一个元素
        QFont font(font_string,10);
        for(int i=0;i<conf.num;++i){
            QGroupBox* tempbox=new QGroupBox(ui->widget_examples);
            tempbox->setGeometry(QRect((i-i/2*2)*120,i/2*50,120,50));
            tempbox->setStyleSheet("border:0px;");
            QLabel* tempblock=new QLabel(tempbox);
            tempblock->setGeometry(QRect(10,12,10,10));
            tempblock->setStyleSheet("background-color:"+conf.member.at(i).left(7)+";");
            QLabel* tempname=new QLabel(tempbox);
            tempname->setGeometry(QRect(30,9,90,16));
            tempname->setStyleSheet("color:"+conf.member.at(i).left(7)+";");
            tempname->setText(conf.member.at(i).right(conf.member.at(i).length()-7));
            tempname->setFont(font);

            tempbox->setAccessibleName(tempname->text());
            groupboxes.append(tempbox);
        }
        for(int i=0;i<conf.num;++i){
            groupboxes.at(i)->setVisible(true);
        }
    }
    qDebug()<<"got config!";
}

void MainWindow::on_pushButton_refresh_clicked()
{
    for(int i=0;i<playlist_buttons.count();++i){
        playlist_buttons.at(i)->setVisible(false);
        playlist_buttons.at(i)->hide();
    }
    for(int i=0;i<playlist_singers_buttons.count();++i){
        playlist_singers_buttons.at(i)->setVisible(false);
        playlist_singers_buttons.at(i)->hide();
    }
    playlist_buttons.clear();
    playlist_singers_buttons.clear();
    name_list.clear();
    playlist->clear();

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
    for(int i=0;i<name_list.length();++i){
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
        playlist_singers_buttons.append(pb_temp2);
    }
    ui->label_playlist->setText("<html><head/><body><p><span style=\" font-size:12pt;\">播放列表</span></p><p><span style=\" color:#a3a3a3;font-size:9pt;\">共"
                                +QString::number(name_list.count())+"首歌曲</span></p></body></html>");
    for(int i=0;i<playlist_buttons.length();++i){
        connect(playlist_buttons.at(i),SIGNAL(clicked(int)),this,SLOT(playlist_buttons_clicked(int)));
        playlist_buttons.at(i)->show();
    }
    ui->scrollAreaWidgetContents->setMinimumHeight(name_list.length()*80+5);
    play_progress=0;
    player->pause();
    load_single_song(name_list.at(0));
    playlist_buttons.at(0)->setStyleSheet("color:"+conf.theme_color+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
    player->play();
}

void MainWindow::on_pushButton_settings_clicked()
{
    if(ui->widget_settings->geometry().width()<1050){
        ui->widget_settings->setGeometry(0,100,1050,380);
        ui->widget_settings->raise();
        on_pushButton_hideplayer_clicked();
    }
    else{
        on_pushButton_settings_return_clicked();
    }
}

void MainWindow::read_userdata(){
    //userdata文件第一行是音量大小 第二行表示是否自动播放 第三行表示是否最小化到托盘
    //第四行表示桌面歌词是否锁定 第五行表示桌面歌词的坐标 第六行表示是否在程序启动时显示桌面歌词
    QFile data(QApplication::applicationDirPath()+"/resources/userdata");
    if(data.open(QIODevice::ReadOnly)){
        QString temp=data.readAll();
        QStringList list=temp.split("\n");
        ui->Slider_volume->setValue(list.at(0).toInt());
        isAutoPlay=list.at(1).toInt();
        isTray=list.at(2).toInt();
        dd.is_locked=list.at(3).toInt();

        QStringList xy=list.at(4).split(" ");
        dd.move(xy.at(0).toInt(),xy.at(1).toInt());

        isAutoShowDesktopLyric=list.at(5).toInt();
    }
    data.close();
    if(isAutoPlay==true){
        ui->checkBox_isAutoPlay->setCheckState(Qt::Checked);
    }
    if(isTray==true){
        ui->radioButton_settings_tray_1->setChecked(true);
    }
    else{
        ui->radioButton_settings_tray_0->setChecked(true);
    }
    if(dd.is_locked==true){
        dd.ui->pushButton_lock->setStyleSheet("border-image: url(:/new/prefix1/locked.png);");
    }
    else{
        dd.ui->pushButton_lock->setStyleSheet("border-image: url(:/new/prefix1/unlocked.png);");
    }
    if(isQuickSelect==true){
        ui->checkBox_quickselect->setCheckState(Qt::Checked);
    }
}

void MainWindow::on_pushButton_settings_return_clicked()
{
    ui->widget_settings->setGeometry(0,100,0,380);
    ui->widget_settings->lower();
    /*if(ui->widget_cover->is_small==true){
        show_player();
        ui->widget_cover->is_small=false;
    }*/
}

void MainWindow::show_player(){
    ui->horizontalSlider->setGeometry(QRect(20,525,780,22));
    ui->label_time->move(810,515);
    ui->label_duration->move(867,515);
    ui->pushButton_play->move(670,440);
    ui->pushButton_next->setGeometry(QRect(760,450,30,30));
    ui->pushButton_last->setGeometry(QRect(590,450,30,30));
    ui->pushButton_playmode->setGeometry(QRect(500,450,30,30));
    ui->widget_cover->setGeometry(QRect(55,135,230,230));
    ui->label_settings_info->hide();
    ui->pushButton_DesktopLyric->move(830,450);
    ui->pushButton_playlist->setGeometry(QRect(950,524,30,28));
    ui->widget_player->setGeometry(0,100,1050,480);

    //if(ui->widget_cover->is_small==true){
        QPropertyAnimation* show_player=new QPropertyAnimation(ui->widget_player,"geometry");
        show_player->setDuration(800);
        show_player->setStartValue(QRect(0,580,1050,0));
        show_player->setEndValue(QRect(0,100,1050,480));
        show_player->setEasingCurve(QEasingCurve::InQuart);
        show_player->start();
    //}

    ui->widget_player->lower();//调整前后遮挡问题
    ui->widget_settings->lower();
    ui->widget_settings->setGeometry(0,100,0,380);
    ui->widget_songlist->setGeometry(QRect(0,100,0,390));
}

/*bool MainWindow::eventFilter(QObject* object, QEvent* event){
    if(event->type()==QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->key()==Qt::Key_Space){
            this->on_pushButton_play_clicked();
        }
    }
    return false;
}*/

void MainWindow::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason){
    switch(reason){
    case QSystemTrayIcon::DoubleClick:
        this->show();
        //mSysTrayIcon->hide();
        break;
    }
}

void MainWindow::on_action_exit_triggered()
{
    qDebug()<<"action_exit_triggered";
    on_pushButton_close_clicked();
    mSysTrayIcon->hide();
    this->close();
}

void MainWindow::on_checkBox_isAutoPlay_clicked(bool checked)
{
    if(checked==true){
        isAutoPlay=true;
    }
    else{
        isAutoPlay=false;
    }
}

void MainWindow::on_radioButton_settings_tray_1_toggled(bool checked)
{
    if(checked==true) isTray=true;
    else isTray=false;
}

void MainWindow::on_pushButton_DesktopLyric_clicked()
{
    if(!dd.isVisible()){
        dd.show();
        ui->pushButton_DesktopLyric->setStyleSheet("background-color: rgba(0,0,0,0);\ncolor:"+conf.theme_color+";");
        isAutoShowDesktopLyric=true;
    }
    else{
        dd.hide();
        ui->pushButton_DesktopLyric->setStyleSheet("background-color: rgba(0,0,0,0);");
        isAutoShowDesktopLyric=false;
    }
}

void MainWindow::show_quickcontrol(){
    QPoint p=QCursor::pos();
    ori_pos=p;
    qs.setGeometry(QRect(p.x()-200,p.y()-200,400,400));
    qs.color=conf.theme_color;

    qsrand(QTime::currentTime().msec());
    QStringList ppp;
    for(int i=0;i<6;++i){
        int r=qrand()%themes.count();
        qs.themes_addr[i]=themes.at(r).left(themes.at(r).length()-7);
        QFile con(QApplication::applicationDirPath()+"/resources/"+qs.themes_addr.at(i)+".config");
        QString icon_addr;
        if(con.open(QIODevice::ReadOnly)){
            QString text=con.readAll();
            QStringList t=text.split("\n");
            icon_addr=QApplication::applicationDirPath()+"/resources/"+t.at(3);
        }
        con.close();
        icon_addr=icon_addr.left(icon_addr.length()-1);
        ppp.append(icon_addr);
        //qDebug()<<ppp.last()<<qs.themes_addr.at(i);
        themes.removeAt(r);
    }
    qs.ui->label_1->setStyleSheet("background-color: rgba(255, 255, 255, 0);\nborder-image: url("+ppp.at(0)+");");
    qs.ui->label_2->setStyleSheet("background-color: rgba(255, 255, 255, 0);\nborder-image: url("+ppp.at(1)+");");
    qs.ui->label_3->setStyleSheet("background-color: rgba(255, 255, 255, 0);\nborder-image: url("+ppp.at(2)+");");
    qs.ui->label_4->setStyleSheet("background-color: rgba(255, 255, 255, 0);\nborder-image: url("+ppp.at(3)+");");
    qs.ui->label_5->setStyleSheet("background-color: rgba(255, 255, 255, 0);\nborder-image: url("+ppp.at(4)+");");
    qs.ui->label_6->setStyleSheet("background-color: rgba(255, 255, 255, 0);\nborder-image: url("+ppp.at(5)+");");

    themes.clear();
    QString pathh=QApplication::applicationDirPath()+"/resources/";
    QDirIterator iterr(pathh,QStringList()<<"*.config",
                      QDir::Files|QDir::NoSymLinks,
                      QDirIterator::Subdirectories);
    while(iterr.hasNext()){
        iterr.next();
        if(iterr.fileName()!="default.config") themes.append(iterr.fileName());
    }

    if(qs.isVisible()==false) qs.show();
}

void MainWindow::hide_quickcontrol(){
    SetCursorPos(ori_pos.x(),ori_pos.y());

    qsrand(QTime::currentTime().msec());
    int r=qrand();
    QList<int> templist;
    for(int i=0;i<name_list.count();++i){
        QString path=QApplication::applicationDirPath()+"/songs/"+name_list.at(i).left(name_list.at(i).length()-4)+".ll";
        QFile file(path);
        if(file.open(QIODevice::ReadOnly)){
            QString temp=file.readAll();
            QStringList ttemp=temp.split("\n");
            qDebug()<<ttemp.at(0)<<qs.themes_addr.at(qs.selected-1);
            if(ttemp.first()[ttemp.first().length()-1]=="\r"||ttemp.first()[ttemp.first().length()-1]=="\n"){
                ttemp[0]=ttemp.at(0).left(ttemp.at(0).length()-1);
            }
            if(ttemp.at(0)==qs.themes_addr.at(qs.selected-1)){
                templist.append(i);
                qDebug()<<"i:"<<i;
            }
        }
        file.close();
    }
    qDebug()<<"templist.count():"<<templist.count();
    r=r%templist.count();
    for(int i=0;i<playlist_buttons.count();++i){
        playlist_buttons.at(i)->setStyleSheet("border-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
    }
    if(isRandomPlay==false){
        play_progress=templist.at(r);
        playlist->setCurrentIndex(play_progress);
        load_single_song(name_list.at(templist.at(r)));
        playlist_buttons.at(play_progress)->setStyleSheet("color:"+conf.theme_color+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
    }
    else{
        for(int i=0;i<random_seq.count();++i){
            if(random_seq.at(i)==templist.at(r)){
                randomplay_progress=i;
                playlist->setCurrentIndex(random_seq.at(i));
                load_single_song(name_list.at(random_seq.at(i)));
                playlist_buttons.at(random_seq.at(i))->setStyleSheet("color:"+conf.theme_color+";\nborder-color:rgba(0,0,0,0);\nbackground-color:rgba(0,0,0,0);\ntext-align:left;",1);
                break;
            }
        }
    }

    if(qs.isVisible()==true) qs.hide();
}

void MainWindow::on_checkBox_quickselect_clicked(bool checked)
{
    if(checked==true){
        isQuickSelect=true;
    }
    else{
        isQuickSelect=false;
    }
}

void MainWindow::on_pushButton_playlist_clicked()
{
    int x=ui->widget_playlist->geometry().x();
    QPropertyAnimation* show_playlist=new QPropertyAnimation(ui->widget_playlist,"geometry");
    show_playlist->setDuration(1000);
    show_playlist->setStartValue(QRect(x,10,0,580));
    show_playlist->setEndValue(QRect(840,10,220,580));
    show_playlist->setEasingCurve(QEasingCurve::OutQuart);

    QPropertyAnimation* hide_playlist=new QPropertyAnimation(ui->widget_playlist,"geometry");
    hide_playlist->setDuration(1000);
    hide_playlist->setStartValue(QRect(x,10,220,580));
    hide_playlist->setEndValue(QRect(1061,10,0,580));
    hide_playlist->setEasingCurve(QEasingCurve::OutQuart);

    if(isPlaylistShowing==false){
        show_playlist->start();
        isPlaylistShowing=true;
    }
    else{
        hide_playlist->start();
        isPlaylistShowing=false;
    }
}

void MainWindow::on_pushButton_hideplaylist_clicked()
{
    QPropertyAnimation* hide_playlist=new QPropertyAnimation(ui->widget_playlist,"geometry");
    hide_playlist->setDuration(1000);
    hide_playlist->setStartValue(QRect(840,10,220,580));
    hide_playlist->setEndValue(QRect(1061,10,0,580));
    hide_playlist->setEasingCurve(QEasingCurve::OutQuart);
    hide_playlist->start();
    isPlaylistShowing=false;
}

void MainWindow::on_pushButton_hideplayer_clicked()
{
    ui->horizontalSlider->setParent(ui->widget_shadow);
    ui->horizontalSlider->setGeometry(QRect(0,490,1050,22));
    ui->label_time->setParent(ui->widget_shadow);
    ui->label_time->move(890,520);
    ui->label_duration->setParent(ui->widget_shadow);
    ui->label_duration->move(947,520);
    ui->pushButton_play->setParent(ui->widget_shadow);
    ui->pushButton_play->move(521,516);
    ui->pushButton_next->setParent(ui->widget_shadow);
    ui->pushButton_next->setGeometry(QRect(586,531,20,20));
    ui->pushButton_last->setParent(ui->widget_shadow);
    ui->pushButton_last->setGeometry(QRect(481,531,20,20));
    ui->pushButton_playmode->setParent(ui->widget_shadow);
    ui->pushButton_playmode->setGeometry(QRect(441,531,20,20));
    ui->widget_cover->setParent(ui->widget_shadow);
    ui->widget_cover->setGeometry(QRect(40,515,50,50));
    QString info=title+" - "+artist+"  ";
    ui->label_settings_info->setText(info);
    ui->label_settings_info->setInterVal(100);
    QFont font(font_string,10);
    ui->label_settings_info->setFont(font);
    ui->label_settings_info->show();
    ui->pushButton_DesktopLyric->move(616,526);
    ui->pushButton_playlist->setGeometry(QRect(656,531,20,20));

    if(ui->widget_cover->is_small==false){
        QPropertyAnimation* hide_player=new QPropertyAnimation(ui->widget_player,"geometry");
        hide_player->setDuration(1000);
        hide_player->setStartValue(QRect(0,100,1050,480));
        hide_player->setEndValue(QRect(0,580,1050,0));
        hide_player->setEasingCurve(QEasingCurve::InQuart);
        hide_player->start();
    }

    ui->widget_settings->raise();
    ui->widget_player->raise();
    ui->widget_cover->is_small=true;
    ui->widget_settings->setGeometry(0,100,1050,380);
    ui->widget_songlist->setGeometry(QRect(0,100,1050,390));
}
