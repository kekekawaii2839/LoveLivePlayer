#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qsliderpro.h"
#include "qlistpushbutton.h"
#include "libzplay.h"
#include <windows.h>
#include <QtWinExtras/QtWin>
#include <gdiplus.h>
#include "globalvariable.h"

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

private slots:
    void time_change(qint64);
    void time_change(int);
    void get_duration(qint64);
    void SaveHDCToFile(libZPlay::TID3InfoExW);
    bool read_lyric(QString,int);
    void on_pushButton_lyric_clicked();
    //void on_pushButton_play_clicked();
    void on_horizontalSlider_sliderPressed();
    void on_horizontalSlider_sliderReleased();
    void time_change_manual(int);
    void get_meta(QString,bool);
    void load_single_song(QString);
    void QSliderProClicked();
    void change_song(QMediaPlayer::MediaStatus);
    void on_Slider_volume_valueChanged(int value);
    void SliderVolumeClicked();
    //void on_pushButton_next_clicked();
    void player_state_change(QMediaPlayer::State);
    QString adjust_text_overlength(QString,QPushButton*,int);
    QString adjust_text_overlength(QString,QLabel*,int);
    //void on_pushButton_last_clicked();
    void playlist_buttons_clicked(int);
    void on_pushButton_minimize_clicked();
    void on_pushButton_close_clicked();
    void on_pushButton_playmode_clicked();
    void get_config(QString);
    void on_pushButton_refresh_clicked();
    void on_pushButton_settings_clicked();
    void read_userdata();
    void on_pushButton_settings_return_clicked();
    //bool eventFilter(QObject* object, QEvent* event);
    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason);
    void on_action_exit_triggered();
    void on_checkBox_isAutoPlay_clicked(bool checked);
    void on_radioButton_settings_tray_1_toggled(bool checked);
    void on_pushButton_DesktopLyric_clicked();
    void on_checkBox_quickselect_clicked(bool checked);
    void on_pushButton_playlist_clicked();
    void on_pushButton_hideplaylist_clicked();
};

#endif // MAINWINDOW_H
