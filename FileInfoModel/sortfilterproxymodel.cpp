#include "sortfilterproxymodel.h"
#include <ssh/sftpfilesystemmodel.h>
#include <QDebug>

SortFilterProxyModel::SortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{

}

void SortFilterProxyModel::setFilterKeyColumns(qint32 colNum)
{
    columnPatterns_.clear();

    for (qint32 i= 0; i < colNum; ++i)
        columnPatterns_.insert(i, QString());
}

void SortFilterProxyModel::addFilterFixedString(const QString &pattern)
{
    //if(!columnPatterns_.contains(column))
    //    return;
    qint32 count = columnPatterns_.size();
    for (qint32 i= 0; i < count; ++i)
        columnPatterns_[i] = pattern;
}

bool SortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(columnPatterns_.isEmpty())
        return true;

    for(QMap<qint32, QString>::const_iterator iter = columnPatterns_.constBegin();
        iter != columnPatterns_.constEnd();
        ++iter)
    {
        if (iter.value().isEmpty())
            continue;
        QModelIndex index = sourceModel()->index(sourceRow, iter.key(), sourceParent);
        QRegExp rx(iter.value());
        rx.setCaseSensitivity(Qt::CaseInsensitive);
        rx.setPatternSyntax(QRegExp::Wildcard);
        const QSsh::SftpFileNode* fileNode = static_cast<QSsh::SftpFileNode *>(index.internalPointer());
        qDebug() << "Filter: " << iter.value() << "" << fileNode->fileInfo.name;
        if(!rx.exactMatch(fileNode->fileInfo.name))
        {
            //qDebug() << fileNode->fileInfo.name << " Reject!";
            return false;
        }
    }
    //qDebug() << fileNode->fileInfo.name << " Accept!";
    return true;
}

bool SortFilterProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    const QSsh::SftpFileNode* leftFileNode = static_cast<QSsh::SftpFileNode *>(source_left.internalPointer());
    const QSsh::SftpFileNode* rightFileNode = static_cast<QSsh::SftpFileNode *>(source_left.internalPointer());

    if (leftFileNode->fileInfo.name > rightFileNode->fileInfo.name)
        return true;
    return false;
}

#if 0
bool SortFilterProxyModel::filterAcceptsRowRecursion(const QString& filter, const QSsh::SftpFileNode* fileNode)
{
    QRegExp rx(filter);
    rx.setCaseSensitivity(Qt::CaseInsensitive);
    rx.setPatternSyntax(QRegExp::Wildcard);
    if(!rx.exactMatch(fileNode->fileInfo.name))
        return false;


    return true;
}
#endif
void SortFilterProxyModel::clearFilter()
{
    addFilterFixedString("");
}
