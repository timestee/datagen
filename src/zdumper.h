#ifndef DUMPER_H
#define DUMPER_H

#include "zdumper.h"
#include "qstring.h"
#include "qjsonvalue.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qjsonarray.h"
#include "qjsonobject.h"
#include "QJsonValueRef"
#include "qvariant.h"
#include "zexcelparser.h"
#include "qdatetime.h"
#include "zutils.h"

class ZDumper
{
public:
    ZSheetInfo* _si;
    QString _fp;
    QString _key;
    bool _bCompact;
    ZDumper(ZSheetInfo* mastersi,QString filepath,QString key):_si(mastersi),_fp(filepath),_key(key){
        _bCompact = false;
    }
public:
    virtual QString commentStart(){
        return "/* ";
    }
    virtual QString commentEnd(){
        return " */";
    }
    virtual QString comments(){
        QDateTime dateTime = dateTime.currentDateTime();
        QString dateTimeString = dateTime.toString("yyyy-MM-dd hh:mm:ss");
        QString s = commentStart();
        s += "This file is generated by datagen; do not edit. "+dateTimeString+"\r\n";
        QVector<QString> comment = _si->getComments();
        for(int i=0;i<comment.size();++i){
            if(i==0){
                s+="== ";
            }
            s +="\t"+comment.at(i);
        }
        s+=commentEnd();
        return s;
    }
    virtual bool dumpToFile(bool compact=false){
        _bCompact = compact;
        QString script = begin();
        if(_si->_jsonObject!=NULL){
            script += toString(*_si->_jsonObject,-1);
        }else{
            script += toString(*_si->_jsonArray,-1);
        }
        script = end(script);
        return ZUtils::saveToFile(_fp,script);
    }

    virtual QString begin(){
        return "";
    }
    virtual QString end(QString curr){
        return curr;
    }
    virtual QString toString(QJsonValue jv,int nested=0){
        QString ret="";
        if(jv.isObject()){
            nested++;
            QJsonObject jo =jv.toObject();
            ret = objectBegin(ret);
            ret =dealNested(ret,nested);
            QJsonObject::iterator it;
            for(it=jo.begin();it!=jo.end();++it){
                QString s = toString(it.value().operator QJsonValue(),nested);
                ret = object(ret,it.key(),s);
                if(it== jo.end()-1){
                    nested-=1;
                }
                ret =dealNested(ret,nested);
            }
             ret = objectEnd(ret);
            return ret;
        }else if(jv.isArray()){
            nested++;
            QJsonArray jo =jv.toArray();
            ret = arrayBegin(ret);
            ret = dealNested(ret,nested);
            int josize = jo.size();
            for(int i=0;i<josize;++i){
                QString s = toString(jo.at(i),nested);
                ret = array(ret,s);
                if(i== josize-1){
                    nested-=1;
                }
                ret = dealNested(ret,nested);
            }
            ret = arrayEnd(ret);
            return ret;
        }
        QString vs = jv.toVariant().toString();
        if(ZUtils::isInt(vs) || ZUtils::isDouble(vs) || ZUtils::isBool(vs)){
            return vs;
        }
        return str(ret,vs);
    }
    virtual QString dealNested(QString curr,int nested)
    {
        if(_bCompact){
            return curr;
        }
        QString ret="\r\n";
        for(int i=0;i<=nested;++i){
            ret += "\t";
        }
        return curr + ret;
    }
    virtual QString objectBegin(QString curr){
        return curr;
    }

    virtual QString object(QString curr,QString name, QString v)
    {
        return curr +name+v;
    }
    virtual QString objectEnd(QString curr){
        return curr;
    }
    virtual QString arrayBegin(QString curr){
        return curr;
    }

    virtual QString array(QString curr,QString v)
    {
        return curr +v;
    }
    virtual QString arrayEnd(QString curr){
        return curr;
    }

    virtual QString str(QString curr,QString v)
    {
        return curr + QString::asprintf("\"%s\"",v.toStdString().c_str());
    }
};




#endif // DUMPER_H
