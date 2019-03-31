#ifndef LOGFILESYSTEMMODEL_H
#define LOGFILESYSTEMMODEL_H

#include <memory>
#include <QMap>
#include <QDebug>
#include <QString>
#include <QMutex>
#include <QMutexLocker>

#include "localfilemodel.h"

namespace fileinfomodel
{

typedef QMap<QString, QString> LogTimeCache;

template <typename ModelT>
class LogFileSystemModel : public FileModelTrait<ModelT>::ModelBaseType
{
    //Q_OBJECT
    typedef typename FileModelTrait<ModelT>::ModelBaseType BaseModel;
public:
    explicit LogFileSystemModel(FileModelPtr fileModel = nullptr, QObject *parent = nullptr):
        BaseModel(parent), fileModel_((IFileModel*)new ModelT(this, this))
    {
        //connect(fileModel_.get(), &ModelT::dataChanged, this, &LogFileSystemModel::dataTrigger);
        //setReadOnly(true);
    }
    virtual ~LogFileSystemModel()
    {
        //pool_->complete(-1);
    }
public:

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if(!index.isValid())
        {
            return FileModelTrait<ModelT>::ModelBaseType::data(index, role);
        }
        if(index.column() == columnCount() - 1)
        {
            switch(role)
            {
               case(Qt::DisplayRole):
                   return logStartTime(index);
               case(Qt::TextAlignmentRole):
                   return static_cast<int>(Qt::AlignLeft | Qt::AlignHCenter);
               default:
                   break;
            }
        }
        return FileModelTrait<ModelT>::ModelBaseType::data(index,role);
    }
    int columnCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return FileModelTrait<ModelT>::ModelBaseType::columnCount()+1;
    }
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        if (section == columnCount() && orientation == Qt::Horizontal && role == Qt::DisplayRole)
            return tr("LogStartTime");
        return FileModelTrait<ModelT>::ModelBaseType::headerData(section, orientation, role);
    }

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

public slots:
    void dataTrigger(const QModelIndex &index)
    {
        emit dataChanged(index, index);
    }

private:
    QString logStartTime(const QModelIndex &index) const
    {
        return fileModel_->getLogStartTimeStr(index);
    }

private:
    FileModelPtr fileModel_;
    mutable LogTimeCache logCache_;
    mutable QMutex cacheMutex_;
};

}
#endif // LOGFILESYSTEMMODEL_H
