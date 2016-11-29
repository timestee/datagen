#ifndef EXCELPARSER_H
#define EXCELPARSER_H

#include "xlsx/xlsxdocument.h"
#include "xlsx/xlsxworkbook.h"
#include "xlsx/xlsxworksheet.h"

#include "qjsonobject.h"
#include "qjsonarray.h"
#include "qjsonvalue.h"
#include "qvariant.h"
#include "ztypes.h"


class ZFieldInfo
{
public:
    QString _comment; // comment info
    QString _raw;     // raw field name with type info and sub sheet info
    QString _name;    // field name
    QString _type;    // field type
    QString _default; // default value
    QString _linkToSheet;    // column link to sub sheet
    bool    _isIdCol;  // is id column or not

    // title format: hero#id , skill#[{}], level@
    ZFieldInfo(QString title,QString currSheetName,QString comment){
        _raw = title.trimmed();
        _isIdCol = false;
        _comment = comment.trimmed();

        int pos_1 = _raw.indexOf('#');
        int pos_2 = _raw.indexOf('@');
        if(pos_1 == -1){
            pos_1 = pos_2;
        }
        // _type could be empty
        _type = _raw.mid(pos_1+1,pos_2-pos_1).trimmed();
        _name = _raw.mid(0,pos_1).trimmed();
        if(_name.isEmpty()){
            _name = _raw;
        }
        if(_type=="id"){
            _isIdCol = true;
        }
        if(pos_2 == -1){
            _linkToSheet = "";
        }else{
            // link to sub sheet : fieldname@parent_sheetname
            _linkToSheet = _name+"@"+currSheetName.trimmed();
        }
    }
    bool isIdCol(){
        return _isIdCol;
    }
};

class ZSheetInfo{
public:
    QString _sheetName;  // full sheet name, could be level@hero
    QString _pureSheetName; // pure sheet name without parent sheet info, shoud be level(level@hero)
    QXlsx::Worksheet* _sheet; // sheet object
    SHEET_TYPE _sheetType; // sheet type, meta master sub
    QString _parentSheetKey; // out key to parent sheet
    QString _parentSheetName; // parent sheet name
    QMap<int,ZFieldInfo*> _fieldInfosBasic; // field info
    QVector<QString> _fieldComments; //fiekd names
    bool _parsed;
    ZFieldInfo* _idColInfo;

    QJsonArray*_jsonArray;
    QJsonObject* _jsonObject;
public:
    ZSheetInfo(QXlsx::Worksheet* sheet){
        QString name = sheet->sheetName();
        this->_sheet = sheet;
        this->_sheetName = name.trimmed().toLower();
        this->_pureSheetName = this->_sheetName;

        if(this->_sheetName.compare("meta")==0){
            this->_sheetType = ST_META;
        }else{
            int indexSplit = this->_sheetName.indexOf('@');
            if(indexSplit==-1){
                this->_sheetType = ST_MASTER;
            }else{
                // property@parent_sheet_name
                this->_sheetType = ST_SUB;
                this->_parentSheetKey = this->_sheetName.mid(0,indexSplit);
                this->_pureSheetName = this->_parentSheetKey;
                this->_parentSheetName =this->_sheetName.mid(indexSplit+1);
            }
        }
        _parsed = false;
        _jsonArray = NULL;
        _jsonObject = NULL;
        _idColInfo = NULL;
    }

    ~ZSheetInfo(){
        QMap<int,ZFieldInfo*>::iterator it;
        for(it=_fieldInfosBasic.begin();it!=_fieldInfosBasic.end();++it)
        {
            delete it.value();
        }
        _fieldInfosBasic.clear();
        _sheet = NULL;
        _idColInfo = NULL;
        if(_jsonArray){
            delete _jsonArray;
        }
        if(_jsonObject)
            delete _jsonObject;
    }

    void updateFieldInfo(int col,QString title,QString comment){
        ZFieldInfo* info = new ZFieldInfo(title,_pureSheetName,comment);
        _fieldComments.append(comment);
        _fieldInfosBasic.insert(col,info);
        if(info->isIdCol()){
            _idColInfo = info;
        }
    }

    ZFieldInfo* getFiledInfo(int col){
        return _fieldInfosBasic.value(col);
    }

    SHEET_TYPE getSheetType(){
        return this->_sheetType;
    }
    QVector<QString> getComments(){
        return _fieldComments;
    }
};

class ZExcelParser
{
public:
    int     _flags;
    bool    _metaFound;
    QString _rawFileName;

    QVector<QString> _warns;

    void clear();

    QMap<QString,ZSheetInfo*> _sheetInfo;
    QMap<QString,QString> _metaInfo;

    bool proc(QString fileName,int flags);
    bool onPostProc();

    static QString getEnumStr(int flag);

    ZSheetInfo* parseSheet(QString name);
    QString getWarnInfo() const {
        QString warn="";
        for(int i=0; i<_warns.size(); i++) {
            warn += _warns.at(i)+ "\r\n";
        }
        return warn;
    }
    QString getFileName(QString msiName,int flag);

    QString getMetaVal(QString domain,QString msiName="",int flag=0);

    QString getOutFileName(int fileTypeFlag,QString filePath,QString saveFileName);
    QJsonArray* procDataSheet(ZSheetInfo* si);
    QJsonValue parseFieldValue(QString value,ZFieldInfo* item,QJsonObject* oRow);
    QJsonValue autoType(std::string value,std::string type="");
    bool updateMetaInfo(ZSheetInfo* si);
    bool dumpToFile(ZSheetInfo* si,int flag);
};

#endif // EXCELPARSER_H
