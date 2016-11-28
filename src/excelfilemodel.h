#ifndef EXCELFILEMODEL_H
#define EXCELFILEMODEL_H

#include "QAbstractTableModel"

#pragma execution_character_set("utf-8")

typedef enum
{
    DEAL_STATE_NORMAL  = 0,
    DEAL_STATE_WAIT    = 1,
    DEAL_STATE_DEALING = 2,
    DEAL_STATE_SUCCESS = 3,
    DEAL_STATE_SUCCESS_WARN = 4,
    DEAL_STATE_ERROR   = 5
} EXCEL_DEAL_STATE;

struct ExcelFileInfo
{
    QString filePath;
    QString fileName;
    EXCEL_DEAL_STATE state;
    bool flagToDeal;
    QString info;
    QString getStateString() const
    {
        if(state==DEAL_STATE_NORMAL) return  QStringLiteral("Wait");
        if(state==DEAL_STATE_WAIT)     return QStringLiteral("Waiting");
        if(state==DEAL_STATE_DEALING)  return QStringLiteral("Dealing");
        if(state==DEAL_STATE_SUCCESS)  return QStringLiteral("Succ");
        if(state==DEAL_STATE_SUCCESS_WARN)  return QStringLiteral("Succ _ Warn");
        if(state==DEAL_STATE_ERROR)    return QStringLiteral("Fail");
        return "Invalid";
    }
};


class ExcelFileModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    ExcelFileModel(const int totalColumn, const int aColumnNumWithChechBox = 0);
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const
    {
        return m_excelInfos.size();
    }
    int columnCount(const QModelIndex &parent = QModelIndex()) const
    {
        return 3;
    }
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool appendData(ExcelFileInfo info);

    void getCheckedFiles(QVector<QString>& files);
    void selectAll(bool selected);
    void clear();

    void updateFileState(QString fileName,EXCEL_DEAL_STATE state,QString info="");
    QString getTipString(int row);

private:
    typedef QVector<ExcelFileInfo> ExcelInfos;
    ExcelInfos m_excelInfos;
    int totalColumn;
    int colNumberWithCheckBox;
};

#endif // EXCELFILEMODEL_H
