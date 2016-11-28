#include "dialog.h"
#include <QApplication>
#include <QTextCodec.h>

#include <qdebug.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec* codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
    Dialog w;
    w.show();

    return a.exec();
}
