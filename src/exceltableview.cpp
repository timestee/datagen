#include "exceltableview.h"
#include "QDragEnterEvent"
#include "qmimedata.h"
#include "qdebug.h"
#include "qlist.h"
#include "qheaderview.h"
#include "qtooltip.h"

ExcelTableView::ExcelTableView(QWidget *parent)
        : QTableView(parent)
{
    m_excelFileModel = new ExcelFileModel(3,2);
    setModel(m_excelFileModel);
    horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    setColumnWidth(0,270);
    setColumnWidth(1,86);
    setColumnWidth(2,30);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    verticalHeader()->hide();
    verticalHeader()->setDefaultSectionSize(20);

    setMouseTracking(true);

    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDragEnabled(true);
    setDropIndicatorShown(true);
}

void ExcelTableView::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}
void ExcelTableView::dropEvent(QDropEvent *event)
{
        if (event->mimeData()->hasUrls()){
            for (const QUrl &url : event->mimeData()->urls()){
                this->addFile(url.toLocalFile());
            }
        }
}
void ExcelTableView::dragMoveEvent(QDragMoveEvent *event)
{
}

void ExcelTableView::addFile(const QString &file)
{
    int pos = file.lastIndexOf('/');
    QString filename = file.mid(pos+1);
    ExcelFileInfo excelInfo;
    excelInfo.filePath = file;
    excelInfo.fileName = filename;
    excelInfo.state    = DEAL_STATE_NORMAL;
    excelInfo.flagToDeal = true;
    m_excelFileModel->appendData(excelInfo);
}

void ExcelTableView::addText(const QString &text)
{
}

bool ExcelTableView::appendData(ExcelFileInfo& info)
{
    if(m_excelFileModel)
    {
        return m_excelFileModel->appendData(info);
    }
    return false;
}

void ExcelTableView::getCheckedFiles(QVector<QString>& files)
{
    if(m_excelFileModel)
    {
        return m_excelFileModel->getCheckedFiles(files);
    }
}

void ExcelTableView::updateFileState(QString fileName,EXCEL_DEAL_STATE state,QString info)
{
    if(m_excelFileModel)
    {
        return m_excelFileModel->updateFileState(fileName,state,info);
    }
}
