#ifndef ZCONF_H
#define ZCONF_H

#include "qsettings.h"
#include "qvariant.h"

class ZConf
{
private:
    ZConf();
    ~ZConf();
public:
    static ZConf &instance();
    void set(QString key,QVariant value);
    QVariant get(QString key,QVariant def="");
    QString getFilePath();

    QSettings* _setting;
};
#endif // ZCONF_H
