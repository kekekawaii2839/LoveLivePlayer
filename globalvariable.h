#ifndef GLOBALVARIABLE_H
#define GLOBALVARIABLE_H

#include <QtMultimedia>
#include "lrc.h"
#include "config.h"
#include "desktoplyricwindow.h"
#include "quickselect.h"
#include "CustomClasses\laudioplayer.h"

extern LAudioPlayer* player;
//extern QMediaPlaylist* playlist;
extern int duration;
extern LRC lyric[200];
extern LRC lyric_translate[200];
extern config conf;
extern DesktopLyricWindow dd;
extern QuickSelect qs;
extern bool isQuickSelect;
extern QString font_string;

#endif // GLOBALVARIABLE_H
