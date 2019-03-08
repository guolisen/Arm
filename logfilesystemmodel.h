#ifndef LOGFILESYSTEMMODEL_H
#define LOGFILESYSTEMMODEL_H

#include <QMap>
#include <QString>
#include <QFileSystemModel>
#include <QMutex>
#include <QMutexLocker>
#include <FileIdentifier/ifileidentifier.h>
#include <Core/IThreadPool.h>

typedef QMap<QString, QString> LogTimeCache;
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
    int columnCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return QFileSystemModel::columnCount()+1;
    }
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        if (section == 4 && orientation == Qt::Horizontal && role == Qt::DisplayRole)
            return tr("LogStartTime");
        return QFileSystemModel::headerData(section, orientation, role);
    }
    void init();

    void setCache(QString key, QString value) const
    {
        QMutexLocker locker(&cacheMutex_);
        logCache_.insert(key, value);
    }

    QString findCache(const QString& key) const
    {
        QMutexLocker locker(&cacheMutex_);
        auto cacheTime = logCache_.find(key);
        if (cacheTime != logCache_.end() && !cacheTime.value().isEmpty())
            return cacheTime.value();
        return "";
    }

private:
    QString logStartTime(const QModelIndex &index, const QFileInfo &fi) const;
    mutable LogTimeCache logCache_;
    fileIdentifier::FileIdentifierPtr fileIdentifier_;
    core::ThreadPoolPtr pool_;
    mutable QMutex cacheMutex_;
};

#endif // LOGFILESYSTEMMODEL_H
