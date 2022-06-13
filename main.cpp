#include "mainwindow.h"
#include <QApplication>

static HHOOK keyHook=nullptr;
MainWindow* w;
bool isQuickSelect;
QString font_string;

LRESULT CALLBACK keyProc(int nCode,WPARAM wParam,LPARAM lParam){//键盘钩子实现
    KBDLLHOOKSTRUCT *pkbhs=(KBDLLHOOKSTRUCT*)lParam;
    if(nCode == HC_ACTION){
        if(wParam==WM_KEYUP){
            //qDebug()<<"pkbhs->vkCode="<<pkbhs->vkCode;
            if(pkbhs->vkCode == VK_MEDIA_PLAY_PAUSE){
                qDebug() << "VK_MEDIA_PLAY_PAUSE";
                w->on_pushButton_play_clicked();
                return 1;
            }
            else if(pkbhs->vkCode == VK_MEDIA_NEXT_TRACK){
                qDebug() << "VK_MEDIA_NEXT_TRACK";
                w->on_pushButton_next_clicked();
                return 1;
            }
            else if(pkbhs->vkCode == VK_MEDIA_PREV_TRACK){
                qDebug() << "VK_MEDIA_PREV_TRACK";
                w->on_pushButton_last_clicked();
                return 1;
            }
            else if(pkbhs->vkCode == VK_LSHIFT&&isQuickSelect==true&&w->qs.isVisible()==true){
                qDebug() << "VK_LSHIFT is up";
                w->hide_quickcontrol();
                return 0;
            }
        }
        if(wParam==WM_KEYDOWN&&isQuickSelect==true){
            if(pkbhs->vkCode == VK_LSHIFT){
                qDebug() << "VK_LSHIFT is down";
                w->show_quickcontrol();
                return 0;
            }
            if(pkbhs->vkCode == VK_ESCAPE){
                qDebug() << "VK_ESCAPE is down";
                w->qs.hide();
                return 0;
            }
        }
        return 0;//返回1表示截取消息不再传递,返回0表示不作处理,消息继续传递
    }
    return CallNextHookEx(keyHook, nCode, wParam, lParam);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    isQuickSelect=true;
    QFile file(QApplication::applicationDirPath()+"/resources/config");
    if(file.open(QIODevice::ReadOnly)){
        QString str=file.readAll();
        QStringList list=str.split("\n");
        if(list.at(0).toInt()==0) isQuickSelect=false;
    }
    file.close();

    int fontID=QFontDatabase::addApplicationFont(":/new/fonts/DroidSansChinese.ttf");
    //int fontID=QFontDatabase::addApplicationFont(":/new/fonts/PingFang Bold.ttf");
    font_string=QFontDatabase::applicationFontFamilies(fontID).at(0);
    QFont font(font_string);
    QApplication::setFont(font);

    w=new MainWindow;

    keyHook=SetWindowsHookEx(WH_KEYBOARD_LL,keyProc,nullptr,0);
    w->show();
    return a.exec();
}
