﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "CustomClasses\qsliderpro.h"
#include "CustomClasses\qlistpushbutton.h"
#include "CustomClasses\qclickwidget.h"
#include "CustomClasses\qmaskwidget.h"
#include "globalvariable.h"
#include "CustomClasses\laudioplayer.h"
#include "CustomClasses\lvideowidget.h"
#include "CustomClasses\animatedscrollbar.h"
#include "songinfo.h"
#include "CustomClasses/qsmarttextwidget.h"
#include "CustomClasses/qtitlewidget.h"

#include <QMainWindow>
#include <QUrl>
#include <QDebug>
#include <QString>
#include <QAbstractAudioInput>
#include <QFile>
#include <QStringList>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QApplication>
#include <QMouseEvent>
#include <QPushButton>
#include <QTextDocument>
#include <QGraphicsDropShadowEffect>
#include <QGroupBox>
#include <QLabel>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QScrollBar>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QMessageBox>
#include <QMap>
#include <QString>
#include <QPainterPath>
#include <QDesktopWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void show_quickcontrol();
    void hide_quickcontrol();
    QuickSelect qs;

public slots:
    void on_pushButton_play_clicked();
    void on_pushButton_next_clicked();
    void on_pushButton_last_clicked();

protected:

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon* mSysTrayIcon;
    DesktopLyricWindow dd;
    QTimer* t,*hoverTimer;
    bool isInitiated,isEnableHover,mousePressed,isMaximized,isResizing;
    QPoint posBeforeMax;
    QSize sizeBeforeMax;
    int sortBySongname=-1;//表示歌单中歌曲排序方式 -1表示自然顺序 0表示升序 1表示降序

    LAudioPlayer* player;
    int duration;
    LRC lyric[200];
    LRC lyric_translate[200];
    LRC lyric_romaji[200];
    config conf;
    LVideoWidget* mv;
    QMap<QString,SongInfo*> infos;

    libZPlay::ZPlay* zplayer;
    QString title,artist,album;
    HWND hwnd;
    int valid_lyric;//储存有效lyric的数量
    int current_lyric_index;//LRC数组当前显示的元素的下标
    QList<QGroupBox*> groupboxes;
    QIcon* empty_icon;
    bool isRandomPlay,isAutoPlay,isTray,isAutoShowDesktopLyric,isLog;
    QPoint ori_pos;
    QList<int> random_seq;
    int randomplay_progress;
    QStringList themes;
    int subLRC;//0表示中文翻译 1表示日文罗马音

    QStringList songlist;//歌单文件名列表
    QStringList songlist_detail;//现在选中的歌单中音频文件的完整绝对路径
    int current_songlist_seq;//现在选中的歌单在songlist中序号 -2代表全部音乐 -1代表我喜欢
    int playing_songlist_seq;//正在播放的歌单序号
    int current_songlist_detail_seq;//当前选中的歌单中正在播放的歌曲的序号 -1表示无正在播放
    QList<QListPushButton*> songlist_buttons;
    QList<QListPushButton*> current_songlist_buttons;
    QList<QListPushButton*> hearts;//目前显示的歌单每首歌曲的红心按钮 用于添加到"我喜欢"
    QList<QLabel*> current_songlist_labels;
    QList<QLabel*> current_songlist_labels2;
    QList<QLabel*> current_songlist_labels3;
    //QList<QWidget*> songlist_detail_containers;
    QListPushButton* play_singlesong_songlist;//鼠标悬浮时在当前歌单中显示的"播放"小按钮
    QListPushButton* playnext_singlesong_songlist;//鼠标悬浮时在当前歌单中显示的"下一首播放"小按钮
    QListPushButton* add_singlesong_songlist;//鼠标悬浮时在当前歌单中显示的"添加到"小按钮
    QListPushButton* del_singlesong_songlist;//鼠标悬浮时在当前歌单中显示的"删除"小按钮

    QStringList name_list;//播放列表
    QStringList all_list;//储存全部音乐的文件名的列表
    int play_progress;//列表播放进度
    QList<QListPushButton*> playlist_buttons;
    QList<QListPushButton*> playlist_singers_buttons;
    bool isPlaylistShowing=false;
    QList<QWidget*> playlist_containers;

    int whatInMainPage=0;//0代表歌单界面 1代表设置界面 2代表播放器界面
    QGraphicsOpacityEffect* effect_for_title_bg;

    QMaskWidget* maskw;

    QString QMenuStyleSheet="QMenu{color:#000000;background-color:#ffffff;border:1px solid #7f7f7f;} QMenu::item{color:#000000;background-color:#ffffff;width:150px;height:40px;padding-left:15px;border-bottom:1px solid #eeeeee;}QMenu::item:selected{color:#000000;background-color:#dddddd;}QMenu::separator{height:1px;background-color:rgba(255,255,255,1);margin-left:5px;margin-right:5px;}QMenu::indicator:unchecked{border:1px solid rgb(180,180,180);}";

private slots:
    void time_change(int);
    void get_duration(qint64);
    QString duration_convert();
    bool read_lyric(QString,int);
    void on_pushButton_lyric_clicked();
    void on_horizontalSlider_sliderPressed();
    void on_horizontalSlider_sliderReleased();
    void time_change_manual(int);
    //void get_meta(QString,bool);
    void load_single_song(QString);
    void QSliderProClicked();
    void change_song(QMediaPlayer::MediaStatus);
    void on_Slider_volume_valueChanged(int value);
    void SliderVolumeClicked();
    void player_state_change(QMediaPlayer::State);
    QString adjust_text_overlength(QString,QWidget*,int);
    void playlist_buttons_clicked(int);
    void on_pushButton_minimize_clicked();
    void on_pushButton_close_clicked();
    void on_pushButton_playmode_clicked();
    void get_config(QString);
    void on_pushButton_refresh_clicked();
    void on_pushButton_settings_clicked();
    void read_userdata();
    void on_pushButton_settings_return_clicked();
    void Show_player();
    void Show_player_next();
    //bool eventFilter(QObject* object, QEvent* event);
    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason);
    void on_action_exit_triggered();
    void on_checkBox_isAutoPlay_clicked(bool checked);
    void on_radioButton_settings_tray_1_toggled(bool checked);
    void on_pushButton_DesktopLyric_clicked();
    void on_checkBox_quickselect_clicked(bool checked);
    void on_pushButton_playlist_clicked();
    void on_pushButton_hideplaylist_clicked();
    void on_pushButton_hideplayer_clicked();
    void songlist_buttons_clicked(int);
    void current_songlist_buttons_clicked(int);
    void current_songlist_buttons_hoverEnter(int);
    void current_songlist_buttons_hoverLeave(int);
    //void on_pushButton_playall_clicked();
    void on_pushButton_allmusic_clicked();
    void on_pushButton_mylike_clicked();
    void write_log(QString);
    void player_error(QMediaPlayer::Error);
    void on_checkBox_log_clicked(bool checked);
    void addPushbuttonsInPlaylist();
    void addPushbuttonsInSonglist(QStringList,bool);
    void clearItemsInCurrentSonglist();
    bool get_all_list();
    void changeThemeColor(
            int seq_playlist=-1,
            int seq_songlist=-3,
            int seq_currentSongInSonglist=-1);
    void on_pushButton_mv_clicked();
    void on_pushButton_switch_sublyric_clicked();
    void on_pushButton_createsonglist_clicked();
    void rename_songlist();
    void songlists_rightclicked(int);
    void clone_songlist();
    void del_songlist();
    void addSongToSonglist(int);
    void delSongInSonglist(int);
    bool isInMylike(QString);
    void addSongToMyLike(int);
    void addNextSong(int);
    void swapPlaylist(int index,int ori_index);
    void showMaximized();
    void unMaximize();
    void swapSongsInSonglist(int a,int b);
    void on_pushButton_songlist_songname_clicked();

    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void resizeEvent(QResizeEvent* e);
    void closeEvent(QCloseEvent* e);
    void changeEvent(QEvent* e);
};

#endif // MAINWINDOW_H
