#ifndef LOGFILESYSTEMMODEL_H
#define LOGFILESYSTEMMODEL_H

#include <memory>
#include <QMap>
#include <QDebug>
#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <QTreeView>
#include <Core/appcontext.h>
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
    explicit LogFileSystemModel(core::ContextPtr context, FileModelPtr fileModel = nullptr, QObject *parent = nullptr):
        BaseModel(parent), fileModel_((IFileModel*)new ModelT(context, this, this)), context_(context)
    {
        //setReadOnly(true);
    }
    virtual ~LogFileSystemModel()
    {
        //pool_->complete(-1);
    }
public:
    mutable int columnSize_;
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
        return FileModelTrait<ModelT>::ModelBaseType::columnCount() + 1;
    }
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        if (section == (columnCount() - 1) && orientation == Qt::Horizontal && role == Qt::DisplayRole)
            return tr("LogStartTime");
        return FileModelTrait<ModelT>::ModelBaseType::headerData(section, orientation, role);
    }

    void setCache(QString key, QString value) const
    {
        QMutexLocker locker(&cacheMutex_);
        //beginResetModel();
        logCache_.insert(key, value);
        //endResetModel();
    }

    QString findCache(const QString& key) const
    {
        QMutexLocker locker(&cacheMutex_);
        auto cacheTime = logCache_.find(key);
        if (cacheTime != logCache_.end() && !cacheTime.value().isEmpty())
            return cacheTime.value();
        return "";
    }

    void setCurrentDir(const QString& path, QTreeView* tree)
    {
        fileModel_->setCurrentDir(path, tree);
    }

    IFileModel* getModel()
    {
        return fileModel_;
    }
private:
    QString logStartTime(const QModelIndex &index) const
    {
        return fileModel_->getLogStartTimeStr(index);
    }

private:
    IFileModel* fileModel_;
    mutable LogTimeCache logCache_;
    mutable QMutex cacheMutex_;
    core::ContextPtr context_;
};

}
#endif // LOGFILESYSTEMMODEL_H
