#include "zconf.h"
#include "qsettings.h"

ZConf::ZConf()
{
    _setting = new QSettings("data_gen_setting",QSettings::IniFormat);
}

ZConf::~ZConf()
{
    _setting->sync();
}

ZConf &ZConf::instance()
{
    static ZConf s_conf;
    return s_conf;
}

void ZConf::set(QString key, QVariant value)
{
    _setting->setValue(key,value);
}

QVariant ZConf::get(QString key,QVariant def)
{
    return _setting->value(key,def);
}

QString ZConf::getFilePath()
{
    return _setting->fileName();
}


