#ifndef UTILS_H
#define UTILS_H

#include "qvector.h"
#include "qstring.h"

class ZUtils
{
public:
    static QString getAppPath();
    static bool isInt(QString value);
    static bool isDouble(QString value);
    static bool isBool(QString value);
    static bool saveToFile(QString filename, QString xstr);
    static void split(QVector<QString>& svec, const QString& ssrc, const QString& seps);
};

#endif // UTILS_H
