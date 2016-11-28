#include "excelfilemodel.h"
#include "qdebug.h"
#include "qcolor.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

ExcelFileModel::ExcelFileModel(const int totalColumn, const int aColumnNumWithChechBox)
    : totalColumn(totalColumn)
    , colNumberWithCheckBox(aColumnNumWithChechBox)
{
}

QVariant ExcelFileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if(section==0) return QStringLiteral("File Name");
    if(section==1) return QStringLiteral("State");
    if(section==2) return QStringLiteral("Selected");

    return QString("Column %1").arg(section);
}

QVariant ExcelFileModel::data( const QModelIndex &index, int role ) const
{
    if (role == Qt::DisplayRole)
    {
        if(index.column()==0)
        {
            return m_excelInfos[index.row()].fileName;
        }
        if(index.column()==1)
        {
            return m_excelInfos[index.row()].getStateString();
        }
    }
    if (role == Qt::CheckStateRole)
    {
        if (index.column() == colNumberWithCheckBox)
        {
            return (m_excelInfos.at(index.row()).flagToDeal)? Qt::Checked : Qt::Unchecked; return Qt::Unchecked;
        }
    }
    if(role == Qt::TextColorRole)
    {
        if(m_excelInfos.at(index.row()).state==DEAL_STATE_ERROR)
            return QColor(Qt::red);
    }

    return QVariant();
}


Qt::ItemFlags ExcelFileModel::flags( const QModelIndex &index ) const
{
    if(!index.isValid()){
    return 0;
    }

    if (index.column() == colNumberWithCheckBox){
                return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
    }
    return  Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool ExcelFileModel::appendData(ExcelFileInfo info)
{
    for(int i=0;i<m_excelInfos.size();++i)
    {
        if(m_excelInfos.at(i).filePath.compare(info.filePath,Qt::CaseInsensitive)==0)
        {
            return false;
        }
    }
    beginResetModel();
    m_excelInfos.append(info);
    endResetModel();
    return true;
}

bool ExcelFileModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if(!index.isValid())
        return false;

    if (role == Qt::CheckStateRole && index.column() == colNumberWithCheckBox)
    {
        if (value == Qt::Checked)
        {
            m_excelInfos[index.row()].flagToDeal = true;
        }
        else
        {
            m_excelInfos[index.row()].flagToDeal = false;
        }
    }
    return true;
}

void ExcelFileModel::clear()
{
    beginResetModel();
    m_excelInfos.clear();
    endResetModel();
}
void ExcelFileModel::selectAll(bool selected)
{
    beginResetModel();
    for(int i=0;i<m_excelInfos.size();++i)
    {
        m_excelInfos[i].flagToDeal=selected;
    }
    endResetModel();
}

void ExcelFileModel::getCheckedFiles(QVector<QString>& files)
{
    beginResetModel();
    for(int i=0;i<m_excelInfos.size();++i)
    {
        if(m_excelInfos.at(i).flagToDeal)
        {
            files.push_back(m_excelInfos.at(i).filePath);
            m_excelInfos[i].state = DEAL_STATE_WAIT;
        }
    }
    endResetModel();
}

void ExcelFileModel::updateFileState(QString fileName,EXCEL_DEAL_STATE state,QString info)
{
    for(int i=0;i<m_excelInfos.size();++i)
    {
        if(m_excelInfos.at(i).filePath.compare(fileName,Qt::CaseInsensitive)==0)
        {
            beginResetModel();
            m_excelInfos[i].state = state;
            m_excelInfos[i].info=info;
            endResetModel();
            break;
        }
    }
}

QString ExcelFileModel::getTipString(int row)
{
    QString info;
    info+="Path : "+m_excelInfos.at(row).filePath;
    info+="\r\n";
    info+="State : "+m_excelInfos.at(row).getStateString();
    info+="\r\n";
    info+="Warn:\r\n"+m_excelInfos.at(row).info;
    return info;
}
