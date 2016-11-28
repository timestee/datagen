#ifndef EXCELTABLEVIEW_H
#define EXCELTABLEVIEW_H

#include "qtableview.h"
#include "excelfilemodel.h"

class ExcelTableView : public QTableView
{
public:
    explicit ExcelTableView(QWidget *parent=0);

    ExcelFileModel* getExcelModel()  {return  m_excelFileModel;}

    bool appendData(ExcelFileInfo& info);
    void getCheckedFiles(QVector<QString>& files);
    void updateFileState(QString fileName,EXCEL_DEAL_STATE state,QString info="");

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

    void addFile(const QString &file);
    void addText(const QString &text);

private:
    ExcelFileModel* m_excelFileModel;
};

#endif // EXCELTABLEVIEW_H
