#include "qsmarttextwidget.h"

QSmartTextLabel::QSmartTextLabel(QWidget* parent){
    setParent(parent);
}

QString QSmartTextLabel::adjust_text_overlength(QString text,int mode){//自动换行 但不修改含html标签的字符串
    if(mode==0){//自动换行模式
        QString result=text.toUtf8();
        QString final;
        QStringList list=result.split("\n");
        result=text;
        int buttonwidth=width();
        for(int u=0;u<list.count();++u){
            int textwidth=fontMetrics().width(list.at(u));
            if(textwidth>=buttonwidth){
                QStringList temp=text.split(" ");
                if(temp.count()>1){//有空格
                    int sum=text.length()-temp.count()+1;
                    int l=0,pos=0;
                    for(int i=0;i<temp.count()-1;++i){
                        l+=temp.at(i).length();
                        if(l*2<=sum&&(l+temp.at(i+1).length())*2>sum){
                            pos=i;
                            break;
                        }
                        else if(i==0&&l*2>sum) break;
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
                    if(fontMetrics().width(ooo.at(p))>=buttonwidth){
                        copy.append(adjust_text_overlength(ooo.at(p),0));
                    }
                    else copy.append(ooo.at(p));
                }
                for(int p=0;p<copy.count();++p){
                    final+=copy.at(p)+"\n";
                }
            }
            else final+=result+"\n";
        }
        QString res=final.left(final.length()-1);
        return res;
    }
    else if(mode==1){//省略模式(替换为...)
        int textwidth=fontMetrics().width(text);
        int buttonwidth=width();
        if(textwidth<=buttonwidth) return text;
        else{
            QString temp;
            int tempwidth;
            for(int i=0;i<text.length();++i){
                temp.append(text.at(i));
                tempwidth=fontMetrics().width(temp);
                int ooowidth=fontMetrics().width("...");
                if(tempwidth+ooowidth>buttonwidth){
                    QString res=temp.left(temp.length()-1)+"...";
                    return res;
                }
            }
        }
    }
}

void QSmartTextLabel::setText(QString text, int mode){
    ori_text=text;
    Mode=mode;
    QLabel::setText(adjust_text_overlength(text,mode));
}

void QSmartTextLabel::resizeEvent(QResizeEvent* e){
    setText(ori_text,Mode);
}
