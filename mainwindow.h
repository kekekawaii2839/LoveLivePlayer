#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qsliderpro.h"
#include "qlistpushbutton.h"
#include "qclickwidget.h"
#include "globalvariable.h"
#include "laudioplayer.h"
#include "lvideowidget.h"

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
#include <QMessageBox>

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

    LAudioPlayer* player;
    int duration;
    LRC lyric[200];
    LRC lyric_translate[200];
    config conf;
    LVideoWidget* mv;

    /*QPropertyAnimation* show_player;
    QPropertyAnimation* hide_player;
    QPropertyAnimation* show_playlist;
    QPropertyAnimation* hide_playlist;*/

    libZPlay::ZPlay* zplayer;
    QString title,artist,album;
    HWND hwnd;
    int valid_lyric;//储存有效lyric的数量
    int current_lyric_index;//LRC数组当前显示的元素的下标
    int time_change_mode;//0表示正常播放 1表示拖动进度条导致的positionChanged
    QStringList name_list;//播放列表
    QStringList all_list;//储存全部音乐的文件名的列表
    int play_progress;//列表播放进度
    QList<QListPushButton*> playlist_buttons;
    QList<QListPushButton*> playlist_singers_buttons;
    QList<QGroupBox*> groupboxes;
    QIcon* empty_icon;
    bool isRandomPlay,isAutoPlay,isTray,isAutoShowDesktopLyric,isLog;
    QPoint ori_pos;
    QList<int> random_seq;
    int randomplay_progress;
    QStringList themes;
    QStringList songlist;//歌单文件名列表
    int current_songlist_seq;//现在选中的歌单在songlist中序号 -2代表全部音乐 -1代表我喜欢
    bool isPlaylistShowing=false;
    int whatInMainPage=0;//0代表歌单界面 1代表设置界面
    QList<QListPushButton*> songlist_buttons;
    QList<QListPushButton*> current_songlist_buttons;
    QList<QLabel*> current_songlist_labels;
    QList<QLabel*> current_songlist_labels2;
    QList<QLabel*> current_songlist_labels3;
    QGraphicsOpacityEffect* effect_for_title_bg;

private slots:
    void time_change(int);
    void get_duration(qint64);
    QString duration_convert();
    bool read_lyric(QString,int);
    void on_pushButton_lyric_clicked();
    void on_horizontalSlider_sliderPressed();
    void on_horizontalSlider_sliderReleased();
    void time_change_manual(int);
    void get_meta(QString,bool);
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
    bool eventFilter(QObject* object, QEvent* event);
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
    void on_pushButton_playall_clicked();
    void on_pushButton_allmusic_clicked();
    void on_pushButton_mylike_clicked();
    void write_log(QString);
    void player_error(QMediaPlayer::Error);
    void ShowAlbumPic();
    void on_checkBox_log_clicked(bool checked);
    //void init_animes();
    //void delete_animes();
    void addPushbuttonsInPlaylist();
    void addPushbuttonsInSonglist(QStringList);
    void clearItemsInCurrentSonglist();
    void get_all_list();
    void changeThemeColor(
            int seq_playlist=-1,
            int seq_songlist=-3,
            int seq_currentSongInSonglist=-1);
    void on_pushButton_mv_clicked();
};

#endif // MAINWINDOW_H
