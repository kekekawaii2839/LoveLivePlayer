#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "lrc.h"
#include "qsliderpro.h"
#include "qlistpushbutton.h"
#include "libzplay.h"
#include <windows.h>
#include <QtWinExtras/QtWin>
#include <gdiplus.h>


#include <QMainWindow>
#include <QtMultimedia>
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

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QMediaPlayer* player;
    QMediaPlaylist* playlist;
    int duration;
    LRC lyric[200];
    LRC lyric_translate[200];
    QString color_info[9];

private slots:
    void time_change(qint64);
    void time_change(int);
    void get_duration(qint64);
    void SaveHDCToFile(libZPlay::TID3InfoEx);
    void read_lyric(QString,int);
    void on_pushButton_lyric_clicked();
    void on_pushButton_play_clicked();
    void on_horizontalSlider_sliderPressed();
    void on_horizontalSlider_sliderReleased();
    void time_change_manual(int);
    void get_meta(QString,bool);
    void load_single_song(QString);
    void QSliderProClicked();
    void change_song(QMediaPlayer::MediaStatus);
    void on_Slider_volume_valueChanged(int value);
    void SliderVolumeClicked();
    void on_pushButton_next_clicked();
    void player_state_change(QMediaPlayer::State);
    QString adjust_text_overlength(QString,QPushButton*,int);
    void on_pushButton_last_clicked();
    void playlist_buttons_clicked(int);
    void on_pushButton_minimize_clicked();
    void on_pushButton_close_clicked();
    void on_pushButton_playmode_clicked();
};

#endif // MAINWINDOW_H
