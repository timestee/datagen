#ifndef ZDUMPERPHP_H
#define ZDUMPERPHP_H

#include "zdumper.h"
#include "qstring.h"
#include "qjsonvalue.h"

class ZSheetInfo;

class ZDumperPhp : public ZDumper
{
public:
    ZDumperPhp(ZSheetInfo* si,QString fp,QString key):ZDumper(si,fp,key){}
    virtual QString begin(){
        return "<?php\r\n"+comments()+"\r\n\r\nreturn ";
    }
    virtual QString end(QString curr){
        return curr+";\r\n";
    }

    virtual QString objectBegin(QString curr){
        return curr +"array(";
    }
    virtual QString object(QString curr,QString name,QString v){
        QString equal = " => ";
         if(_bCompact){
             equal="=>";
         }
         return curr + QString::asprintf("\"%s\"%s%s,",name.toStdString().c_str(),equal.toStdString().c_str(),v.toStdString().c_str());
    }
    virtual QString objectEnd(QString curr){
        return curr +")";
    }

    virtual QString arrayBegin(QString curr){
        return curr +"array(";
    }
    virtual QString array(QString curr,QString v){
        return curr + QString::asprintf("%s,",v.toStdString().c_str());
    }
    virtual QString arrayEnd(QString curr){
        return curr +")";
    }
    virtual QString str(QString curr,QString v){
        if(v.indexOf(QRegExp("[\r\n\"]"))!=-1){
            return curr + QString::asprintf("'%s'",v.toStdString().c_str());
        }
        return curr + QString::asprintf("\"%s\"",v.toStdString().c_str());
    }
};

#endif // ZDUMPERPHP_H
