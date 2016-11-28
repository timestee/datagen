#ifndef ZDUMPERLUA_H
#define ZDUMPERLUA_H

#include "zdumper.h"
#include "qstring.h"
#include "qjsonvalue.h"

class ZSheetInfo;

class ZDumperLua : public ZDumper
{
public:
    ZDumperLua(ZSheetInfo* si,QString fp,QString key):ZDumper(si,fp,key){}
    virtual QString commentStart(){
        return "--[[ ";
    }
    virtual QString commentEnd(){
        return " ]]--";
    }
    virtual QString begin(){
        return comments()+QString::asprintf("\r\n\r\nlocal %s = ",_key.toStdString().c_str());
    }
    virtual QString end(QString curr){
        return curr+"\r\nreturn "+_key+"\r\n";
    }

     virtual QString objectBegin(QString curr){
         return curr+"{";
     }
     virtual QString object(QString curr,QString name,QString v){
        QString equal = " = ";
         if(_bCompact){
             equal="=";
         }
         return curr + QString::asprintf("[\"%s\"]%s%s,",name.toStdString().c_str(),equal.toStdString().c_str(),v.toStdString().c_str());
     }
     virtual QString objectEnd(QString curr){
         return curr+"}";
     }

     virtual QString arrayBegin(QString curr){
         return curr+"{";
     }
     virtual  QString array(QString curr,QString v){
         return curr + QString::asprintf("%s,",v.toStdString().c_str());
     }
     virtual QString arrayEnd(QString curr){
         return curr+"}";
     }

    virtual QString str(QString curr,QString v){
        if(v.indexOf(QRegExp("[\r\n\"]"))!=-1){
            return curr + QString::asprintf("[==[%s]==]",v.toStdString().c_str());
        }
        return curr + QString::asprintf("\"%s\"",v.toStdString().c_str());
    }
};

#endif // ZDUMPERLUA_H
