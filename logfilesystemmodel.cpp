#include "logfilesystemmodel.h"

QVariant LogFileSystemModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
    {
        return QFileSystemModel::data(index, role);
    }
    if(index.column() == columnCount() - 1)
    {
        switch(role)
        {
           case(Qt::DisplayRole):
               return QString("YourText");
           case(Qt::TextAlignmentRole):
               return Qt::AlignLeft;
           default:
               break;
        }
    }
    return QFileSystemModel::data(index,role);
}

QHash<int, QByteArray> LogFileSystemModel::roleNames() const
{
     QHash<int, QByteArray> result = QFileSystemModel::roleNames();
     result.insert(LogStartTimeRole, QByteArrayLiteral("LogStartTime"));
     return result;
}

QString LogFileSystemModel::logStartTime(const QFileInfo &fi) const
{
    if (!fi.isFile())
        return QString();
    const qint64 size = fi.size();
    if (size > 1024 * 1024 * 10)
        return QString::number(size / (1024 * 1024)) + QLatin1Char('M');
    if (size > 1024 * 10)
        return QString::number(size / 1024) + QLatin1Char('K');
    return QString::number(size);
}
