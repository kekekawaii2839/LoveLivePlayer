#include "lrc.h"

LRC::LRC()
{
    text="";
    time=-1;
    color_num=0;
    for(int i=0;i<9;i++){
        color[i]=false;
    }
}
