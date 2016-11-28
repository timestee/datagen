#ifndef ZTYPES_H
#define ZTYPES_H


enum
{
    FLAG_FILE_LUA   = 0x01,
    FLAG_FILE_JSON  = 0x02,
    FLAG_FILE_PHP   = 0x04,
};

enum SHEET_TYPE{
    ST_MASTER = 1,
    ST_SUB = 2,
    ST_META = 3
};

#define META_SHEET_NAME "meta"

#endif
