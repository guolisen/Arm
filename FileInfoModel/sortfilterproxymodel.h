#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include <QtCore/qsortfilterproxymodel.h>

class SortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SortFilterProxyModel(QObject *parent);
    void setFilterKeyColumns(qint32 colNum);
    void addFilterFixedString(const QString &pattern);

    void clearFilter();
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    QMap<qint32, QString> columnPatterns_;
};

#endif // SORTFILTERPROXYMODEL_H
