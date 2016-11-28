#ifndef ZDUMPERJSON_H
#define ZDUMPERJSON_H

#include "zdumper.h"
#include "qstring.h"
#include "qjsondocument.h"
#include "zutils.h"

class ZSheetInfo;

class ZDumperJson : public ZDumper
{
public:
    ZDumperJson(ZSheetInfo* si,QString fp,QString key):ZDumper(si,fp,key){}
    virtual bool dumpToFile(bool compact=false){
        QJsonDocument doc(*_si->_jsonArray);
        if(_si->_jsonObject !=NULL){
            doc = QJsonDocument(*_si->_jsonObject);
        }
        QByteArray bytes = doc.toJson(compact?QJsonDocument::Compact:QJsonDocument::Indented);
        return ZUtils::saveToFile(_fp,QString::fromStdString((bytes.toStdString())));
    }
};

#endif // ZDUMPERJSON_H
