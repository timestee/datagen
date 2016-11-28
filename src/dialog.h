#ifndef DIALOG_H
#define DIALOG_H

#define VERSION  "V 1.1.0"
#define APP_NAME "DataGen"

#include <QDialog>
#include "src/excelfilemodel.h"
#include "src/procthread.h"
#include "exceltableview.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    QString m_warning;

private slots:
    void on_btnAllSelect_clicked();
    void on_btnAllUnSelect_clicked();
    void on_btnClearAll_clicked();
    void on_btnOpenFile_clicked();
    void on_btnOutPath_clicked();
    void on_btnStopCurrentTask_clicked();
    void on_btnCloseDlg_clicked();
    void on_btnStart_clicked();
    void on_btn_config_clicked();
    void on_btnAboutVersion_clicked();

    void doMakeTask(int flags);
    void couldProc(bool could);
private:
    Ui::Dialog *ui;
    ExcelFileModel* m_excelFileModel;
    ProcThread* m_procThread;

public slots:
    void excelFileDone(QString fileName,int state,QString info="");
    void procOK();
    void procFail();
    void showToolTip(const QModelIndex &index);

};

#endif // DIALOG_H
