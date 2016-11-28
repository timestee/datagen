#include "dialog.h"
#include "ui_dialog.h"
#include "QtDebug"

#include "qstandarditemmodel.h"
#include "src/excelfilemodel.h"
#include <qfiledialog.h>
#include "qdir.h"
#include "src/procthread.h"
#include "qtooltip.h"
#include <qmessagebox.h>
#include <qerrormessage.h>
#include "qmessagebox.h"
#include "zconf.h"
#include "ztypes.h"
#include "qdesktopservices.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    m_procThread = NULL;
    ui->setupUi(this);
    setFixedSize(421,570);

    m_excelFileModel = ui->m_listCtrl->getExcelModel();
    connect(ui->m_listCtrl,SIGNAL(entered(const QModelIndex&)),this,SLOT(showToolTip(const QModelIndex&)));

    ui->m_outPath->setText(ZConf::instance().get("dir_last_out").toString());

    ui->btnStopCurrentTask->setEnabled(false);

    QDir dir(ZConf::instance().get("dir_last_in").toString());
    if(dir.exists())
    {
        QFileInfoList infoList = dir.entryInfoList();
        for(int i = 0; i < infoList.size(); i ++)
        {
            QFileInfo &fileInfo = infoList[i];
            if(fileInfo.suffix() != "xls" && fileInfo.suffix() != "xlsx") continue;
            ExcelFileInfo excelInfo;
            excelInfo.filePath = fileInfo.absoluteFilePath();
            excelInfo.fileName = fileInfo.fileName();
            excelInfo.state    = DEAL_STATE_NORMAL;
            excelInfo.flagToDeal = true;
            m_excelFileModel->appendData(excelInfo);
        }
    }
    ui->ck_json->setChecked(ZConf::instance().get("ck_json",false).toBool());
    ui->ck_lua->setChecked(ZConf::instance().get("ck_lua",false).toBool());
    ui->ck_php->setChecked(ZConf::instance().get("ck_php",false).toBool());
}

void Dialog::showToolTip(const QModelIndex &index)
{
    if(!index.isValid())
            return;
    QString info = m_excelFileModel->getTipString(index.row());

    QToolTip::showText(QCursor::pos(), info);
}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::on_btnAllSelect_clicked()
{
    m_excelFileModel->selectAll(true);
}

void Dialog::on_btnAllUnSelect_clicked()
{
    m_excelFileModel->selectAll(false);
}

void Dialog::on_btnClearAll_clicked()
{
    m_excelFileModel->clear();
}

void Dialog::on_btnOpenFile_clicked()
{
    QFileDialog::Options options;
    QString selectedFilter;
    QStringList files = QFileDialog::getOpenFileNames(
                                    this, tr("QFileDialog::getOpenFileNames()"),
                                    ZConf::instance().get("dir_last_in").toString(),
                                    QStringLiteral("Excel (*.xls *.xlsx)"),
                                    &selectedFilter,
                                    options);

    for(int i=0;i<files.count();++i)
    {
        ExcelFileInfo info;
        info.filePath = files[i];
        info.fileName = QFileInfo(files[i]).fileName();
        info.state    = DEAL_STATE_NORMAL;
        info.flagToDeal = true;
        m_excelFileModel->appendData(info);
    }
    if(files.size()>0)
    {
        ZConf::instance().set("dir_last_in",QFileInfo(files[files.count()-1]).absolutePath());
    }
}

void Dialog::on_btnOutPath_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, QStringLiteral("Out Dir"),
    ZConf::instance().get("dir_last_out").toString(),
    QFileDialog::ShowDirsOnly
    | QFileDialog::DontResolveSymlinks);
    if(!dir.isEmpty())
    {
        ui->m_outPath->setText(dir);
        ZConf::instance().set("dir_last_out",dir);
    }
}

void Dialog::doMakeTask(int flags)
{
    ZConf::instance().set("dir_last_out",ui->m_outPath->text());

    QVector<QString> files;
    m_excelFileModel->getCheckedFiles(files);
    m_procThread = new ProcThread();
    m_procThread->startWithFiles(files,flags,this);

    couldProc(false);
}

void Dialog::excelFileDone(QString fileName,int state,QString info)
{
    m_excelFileModel->updateFileState(fileName,(EXCEL_DEAL_STATE)state,info);
}

void Dialog::on_btnStopCurrentTask_clicked()
{
    if(m_procThread) {
        m_procThread->term();
        delete m_procThread;
        m_procThread = NULL;
    }
}


void Dialog::on_btnCloseDlg_clicked()
{
    this->close();
}

void Dialog::procOK()
{
    if(!m_warning.isEmpty())
    {
        QErrorMessage *msg = new QErrorMessage(this);
        msg->showMessage(m_warning);
    }
    couldProc(true);
}
void Dialog::procFail()
{
    procOK();
}

 void Dialog::couldProc(bool could)
 {
     ui->btnAllSelect->setEnabled(could);
     ui->btnAllUnSelect->setEnabled(could);
     ui->btnClearAll->setEnabled(could);
     ui->btnOutPath->setEnabled(could);
     ui->btnOpenFile->setEnabled(could);
     ui->btnStopCurrentTask->setEnabled(!could);
 }

void Dialog::on_btnAboutVersion_clicked()
{
    QMessageBox::information(NULL,"About" ,"DataGen" ,QMessageBox::Ok,QMessageBox::Ok);
}

void Dialog::on_btnStart_clicked()
{
    ZConf::instance().set("ck_json",ui->ck_json->isChecked());
    ZConf::instance().set("ck_lua",ui->ck_lua->isChecked());
    ZConf::instance().set("ck_php",ui->ck_php->isChecked());

    int flag= 1;
    if(ui->ck_json->isChecked()){
        flag |= FLAG_FILE_JSON;
    }
    if(ui->ck_lua->isChecked()){
        flag |= FLAG_FILE_LUA;
    }

    if(ui->ck_php->isChecked()){
        flag |= FLAG_FILE_PHP;
    }
    doMakeTask(flag);
}

void Dialog::on_btn_config_clicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(ZConf::instance().getFilePath()));
}
