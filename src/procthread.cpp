#include "procthread.h"
#include "qdebug.h"
#include <qmessagebox.h>
#include <qerrormessage.h>
#include "dialog.h"
#include "zexcelparser.h"

ProcThread::ProcThread(QObject *parent) : QThread(parent)
{
}

void ProcThread::run()
{
    QString &warning = static_cast<Dialog*>(m_dlg)->m_warning;
    warning.clear();

    bool bOK = true;
    ZExcelParser parser;

    connect(this,SIGNAL(excelDone(QString,int,QString)),m_dlg,SLOT(excelFileDone(QString,int,QString)));
    connect(this,SIGNAL(procOK()),m_dlg,SLOT(procOK()));
    connect(this,SIGNAL(procFail()),m_dlg,SLOT(procFail()));

    QVector<QString>::iterator ptr;
    for(ptr = m_files.begin(); ptr != m_files.end(); ++ptr) {
        emit excelDone(*ptr,DEAL_STATE_DEALING);
        if(m_bCancel) {
            bOK = false;
            break;
        }
        if(!parser.proc(*ptr,m_flags)) {
            emit excelDone(*ptr,DEAL_STATE_ERROR,parser.getWarnInfo());
        }else{
            QString warn = parser.getWarnInfo();
            if(warn.isEmpty()){
                emit excelDone(*ptr,DEAL_STATE_SUCCESS,warn);
            }else{
                emit excelDone(*ptr,DEAL_STATE_SUCCESS_WARN,warn);
            }
        }
    }

    if(bOK)
    {
        emit procOK();
    }else{
        emit procFail();
    }

}

void ProcThread::startWithFiles(QVector<QString>& files,int flags,QObject* dlg)
{
    m_flags = flags;
    m_files  = files;
    m_bCancel = false;
    m_dlg = dlg;
    this->start();
}


void ProcThread::term()
{
    m_bCancel = true;
    exit();
}
