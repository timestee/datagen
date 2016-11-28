#include "zexcelparser.h"
#include "qdebug.h"
#include "util.h"
#include "zdumperjson.h"
#include "zdumperlua.h"
#include "zdumperphp.h"
#include "qmessagebox.h"
#include "zconf.h"
#include "qdatetime.h"
#include "zutils.h"

void ZExcelParser::clear()
{
    _rawFileName .clear();
    _flags = 0;
    QMap<QString,ZSheetInfo*>::iterator it;
    for(it=_sheetInfo.begin();it!=_sheetInfo.end();++it)
    {
        delete it.value();
    }
    _sheetInfo.clear();
    _metaInfo.clear();
    _metaFound = false;
    _warns.clear();
}

bool ZExcelParser::proc(QString fileName,int flags)
{
    clear();
    _flags = flags;
    _rawFileName = fileName;

    QXlsx::Document xlsx(fileName);
    QXlsx::Workbook* workbook=xlsx.workbook();

    for(int i=0;i<workbook->sheetCount();++i)
    {
        QXlsx::Worksheet* sheet = static_cast<QXlsx::Worksheet*>(workbook->sheet(i));
        QString name = sheet->sheetName();
        ZSheetInfo* si = new ZSheetInfo(sheet);
        if(name.compare(META_SHEET_NAME)==0){
            _metaFound = true;
            if(!updateMetaInfo(si)){
                return false;
            }
        }
        _sheetInfo.insert(name,si);
    }
    if(!_metaFound){
        _warns.append(fileName+", no meta sheet found");
        return false;
    }

    QMap<QString,ZSheetInfo*>::iterator it;
    for(it=_sheetInfo.begin();it!=_sheetInfo.end();++it)
    {
        ZSheetInfo* si = (it.value());
        if(si->getSheetType() == ST_MASTER){
            this->parseSheet(si->_sheetName);
        }
    }
    return onPostProc();
}
bool ZExcelParser::dumpToFile(ZSheetInfo *si, int flag)
{
    QString key = getFileName(si->_sheetName,flag);
    QString filePath =getOutFileName(flag,_rawFileName,key);
    QString compact = getMetaVal("compact",si->_sheetName,flag);
    bool useCompact = compact.compare("1")==0;
    switch (flag) {
    case FLAG_FILE_LUA:
        return ZDumperLua(si,filePath,key).dumpToFile(useCompact);
    case FLAG_FILE_JSON:
        return ZDumperJson(si,filePath,key).dumpToFile(useCompact);
    case FLAG_FILE_PHP:
        return ZDumperPhp(si,filePath,key).dumpToFile(useCompact);
    }
    _warns.append("flag"+ getEnumStr(flag) + "not supported");
    return false;
}
bool ZExcelParser::onPostProc()
{
    QMap<QString,ZSheetInfo*>::iterator it;
    for(it=_sheetInfo.begin();it!=_sheetInfo.end();++it)
    {
        ZSheetInfo* si = it.value();
        if(si->getSheetType() == ST_MASTER){
            if(_flags & FLAG_FILE_JSON){
                dumpToFile(si,FLAG_FILE_JSON);
            }
            if(_flags & FLAG_FILE_LUA){
                dumpToFile(si,FLAG_FILE_LUA);
            }
            if(_flags & FLAG_FILE_PHP){
                dumpToFile(si,FLAG_FILE_PHP);
            }
        }
    }
    return true;
}

QJsonValue autoType(QString value,QString type=""){
    if(type=="s"){
        return QJsonValue(value);
    }

    //true,true
    if(type=="[]"){
        QJsonArray ret;
        QVector<QString> out;
        ZUtils::split(out,value,";,");
        for(int i=0;i<out.size();++i){
            ret.append(autoType(out.at(i)));
        }
        return ret;
    }
    //mode:percent;value:45
    if(type=="{}"){
        QJsonObject ret;
        QVector<QString> out;
        ZUtils::split(out,value,";,");
        for(int i=0;i<out.size();++i){
            QVector<QString> out2;
            ZUtils::split(out2,out.at(i),":");
            ret.insert(out2.at(0),autoType(out2.at(1)));
        }
        return ret;
    }
    //id:2;level:30,id:3;level:80
    if(type=="[{}]"){
        QJsonArray ret;
        QVector<QString> out;
        ZUtils::split(out,value,";");
        for(int i=0;i<out.size();++i){
            QVector<QString> out2;
            ZUtils::split(out2,out.at(i),",");
            QJsonObject o;
            for(int i=0;i<out2.size();++i){
                QVector<QString> out3;
                ZUtils::split(out3,out2.at(i),":");
                o.insert(out3.at(0),autoType(out3.at(1)));
            }
            ret.append(o);
        }
        return ret;
    }
    return QJsonValue(value);
}

QJsonValue ZExcelParser::parseFieldValue(QString value, ZFieldInfo *item,QJsonObject* oRow)
{
    if(!item->_linkToSheet.isEmpty()){
        ZSheetInfo* si = this->parseSheet(item->_linkToSheet);
        if(!si){
            return QJsonValue::Null;
        }
        // using linked cell's value as key first, if empty, use _idColInfo._name as key
        QString keyVal = value;
        if(keyVal.isEmpty()){
            QJsonValue v_id = oRow->operator [](si->_idColInfo->_name);
            keyVal = v_id.toVariant().toString();
        }
        return si->_jsonObject->operator [](keyVal);
    }
    return autoType(value,item->_type);
}

bool ZExcelParser::updateMetaInfo(ZSheetInfo *si)
{
    QXlsx::Worksheet* sheet = si->_sheet;
    QXlsx::CellRange cells = sheet->dimension();
    int lastRow = cells.lastRow();
    long colCount = cells.columnCount();
    for(long i = 1; i <= lastRow; ++i){
        QXlsx::Cell* keyCell = sheet->cellAt(i,1);
        QXlsx::Cell* valCell = sheet->cellAt(i,2);
        if(keyCell==NULL || valCell==NULL){
            continue;
        }
        _metaInfo.insert(keyCell->value().toString().trimmed().toLower(),valCell->value().toString().trimmed().toLower());
    }
    return true;
}

QJsonArray* ZExcelParser::procDataSheet(ZSheetInfo* si)
{
    QXlsx::Worksheet* sheet = si->_sheet;
    QXlsx::CellRange cells = sheet->dimension();
    long colCount = cells.columnCount();

    // config
    int iNameRow = ZConf::instance().get("row_name",1).toInt();
    int iFieldRow = ZConf::instance().get("row_field",2).toInt();
    int iDdataRow = ZConf::instance().get("row_data",3).toInt();

    for(int col = 1; col <= colCount; col ++)
    {
        QXlsx::Cell *cell = sheet->cellAt(iFieldRow, col);
        QXlsx::Cell *nameCell = sheet->cellAt(iNameRow, col);
        if(cell == NULL)
            continue;
        QString name = cell->value().toString();
        if(!name.isEmpty())
        {
            QString comment =name;
            if(nameCell){
                comment = nameCell->value().toString();
            }
            si->updateFieldInfo(col-1,name,comment);
        }else{
            QString info  =QString::asprintf("%s title invalid at col:%d",si->_sheetName.toStdString().c_str(),col);
            if(col == colCount){
                 colCount -= 1;
                 info += ", is the last col,skip it.";
            }else{
                info += ", not the last col.";
            }
            qDebug()<<info<<endl;
            _warns.append(info);
        }
    }    
    //read data from dataRow to lastRow
    int lastRow = cells.lastRow();
    QJsonArray* jsonArray = new QJsonArray;
    for(long i = iDdataRow; i <= lastRow; ++i)
    {
        QJsonObject oRowJson;
        for(long j = 1; j <= colCount; ++j)
        {
            QXlsx::Cell* dataCell = sheet->cellAt(i,j);
            ZFieldInfo* info = si->getFiledInfo(j-1);
            if(!info){
                _warns.append(QString::asprintf("%d field info invalid at %s",j-1,si->_sheetName.toStdString().c_str()));
                continue;
            }
            QString valueStr = "";
            if(dataCell)
            {
                valueStr=dataCell->value().toString();
            }
            oRowJson.insert(info->_name,parseFieldValue(valueStr,info,&oRowJson));
        }
        jsonArray->append(oRowJson);
    }
    return jsonArray;
}

ZSheetInfo* ZExcelParser::parseSheet(QString name){
    ZSheetInfo* si = _sheetInfo.value(name);
    if(!si){
        _warns.append("sheet not found:"+name);
        return NULL;
    }
    if(si->_parsed){
        return si;
    }

    QJsonArray* ja = procDataSheet(si);
    if(!ja){
        return NULL;
    }
    si->_jsonArray = ja;

    // master
    if(si->_sheetType == ST_MASTER){ // as object or array
        if(si->_idColInfo!=NULL){
            QJsonObject* root = new QJsonObject;
            for(int i=0;i<ja->size();++i){
                QJsonObject row = ja->at(i).toObject();
                QString key = (row.operator [](si->_idColInfo->_name)).toVariant().toString();
                QJsonValue existVal = root->operator [](key);
                if(!existVal.isNull()){
                    _warns.append(si->_sheetName + "id conflict with id: "+key);
                }
                root->insert(key,row);
            }
            si->_jsonObject =root;
        }else{
            //single item in array, output as object
            if(getMetaVal("single_obj")=="1" && ja->size()==1){
                si->_jsonObject = new QJsonObject(ja->at(0).toObject());
            }
        }
    }
    // sub
    if(si->_sheetType == ST_SUB){
        QString keyName = si->_parentSheetKey;
        ZFieldInfo* idCol = si->_idColInfo;
        QString idColName =idCol->_name;

        QJsonObject* root = new QJsonObject;
        bool asObject = true;
        if(ja->size()>0){
            QJsonValue key = ja->at(0).toObject().value(keyName);
            if(key.toVariant().toString().isEmpty()){
                asObject = false;
            }
        }

        for(int i=0;i<ja->size();++i){
            QJsonObject row = ja->at(i).toObject();
            QString idValue  = (row.operator [](idColName)).toVariant().toString();
            QString keyValue = (row.operator [](keyName)).toVariant().toString();
            row.remove(idColName);
            row.remove(keyName);

            if(asObject){
                QJsonObject o_id;
                QJsonValue v_id = root->operator [](idValue);
                if(!v_id.isNull()){
                    o_id = v_id.toObject();
                }
                o_id.insert(keyValue,row);
                root->insert(idValue,o_id);
            }else{
                QJsonArray a_id;
                QJsonValue v_id = root->operator [](idValue);
                if(!v_id.isNull()){
                    a_id = v_id.toArray();
                }
                a_id.append(row);
                root->insert(idValue,a_id);
            }
        }

        si->_jsonObject =root;
    }
    si->_parsed = true;
    return si;
}

QString ZExcelParser::getFileName(QString msiName,int flag)
{
    QString v = getMetaVal("file",msiName,flag);
    if(v.isEmpty()){
        return msiName;
    }
    return v;
}

QString ZExcelParser::getMetaVal(QString domain,QString msiName, int flag)
{
    QString doMainkey=domain;
    QString doMainMsiKey =domain;
    QString doMainMsiFlagKey =domain;

    if(!msiName.isEmpty()){
        doMainMsiKey = doMainkey+"-"+msiName;
    }
    if(flag!=0){
        doMainMsiFlagKey = doMainMsiKey+"-"+getEnumStr(flag);
    }

    if(_metaInfo.find(doMainMsiKey)!=_metaInfo.end()){
        return _metaInfo.value(doMainMsiKey);
    }
    if(_metaInfo.find(doMainMsiFlagKey)!=_metaInfo.end()){
        return _metaInfo.value(doMainMsiFlagKey);
    }
    if(_metaInfo.find(doMainkey)!=_metaInfo.end()){
        return _metaInfo.value(doMainkey);
    }
    return "";
}

QString ZExcelParser::getEnumStr(int flag)
{
    switch (flag) {
    case FLAG_FILE_LUA:
        return "lua";
    case FLAG_FILE_JSON:
        return "json";
    case FLAG_FILE_PHP:
        return "php";
    }
    return QString(flag);
}

QString ZExcelParser::getOutFileName(int fileTypeFlag, QString filePath, QString saveFileName)
{
    QString retFilePath;
    QString suffix = getEnumStr(fileTypeFlag);
    QString out = ZConf::instance().get("dir_last_out").toString();
    if(out.isEmpty())
    {
        int pos = filePath.lastIndexOf('/');
        retFilePath = filePath.left(pos+1);
        retFilePath += saveFileName;
        retFilePath+="."+suffix;
    }else{
        retFilePath = out+"/";
        retFilePath += saveFileName;
        retFilePath+="."+suffix;
    }
    qDebug()<<" retFilePath : "<<retFilePath<<endl;
    return retFilePath;
}
