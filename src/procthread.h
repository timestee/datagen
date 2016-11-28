#ifndef PROCTHREAD_H
#define PROCTHREAD_H

#include "qthread.h"
#include "QVector"
#include "QString"
#include "excelfilemodel.h"

class ProcThread : public QThread
{
    Q_OBJECT
public:
    ProcThread(QObject *parent = 0);
    void startWithFiles(QVector<QString>& files,int flags,QObject* dilg);
    void term();

protected:
    void run();

protected:
    QVector<QString> m_files;
    QObject* m_dlg;
    int    m_flags;
    bool   m_bCancel;

signals:
    void excelDone(QString fileName,int state,QString info="");
    void procOK();
    void procFail();
};

#endif // PROCTHREAD_H
