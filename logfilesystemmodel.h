#ifndef LOGFILESYSTEMMODEL_H
#define LOGFILESYSTEMMODEL_H

#include <QFileSystemModel>

class LogFileSystemModel : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit LogFileSystemModel(QObject *parent = nullptr):
        QFileSystemModel(parent) {}

public:
    enum Roles  {
        LogStartTimeRole = Qt::UserRole + 4,
    };
    Q_ENUM(Roles)

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return QFileSystemModel::columnCount()+1;
    }
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        if (section == 4 && orientation == Qt::Horizontal && role == Qt::DisplayRole)
            return tr("Notifications");
        return QFileSystemModel::headerData(section, orientation, role);
    }
private:
    QString logStartTime(const QFileInfo &fi) const;

};

#endif // LOGFILESYSTEMMODEL_H
