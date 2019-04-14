#ifndef IFILEMODEL_H
#define IFILEMODEL_H

#include <memory>
#include <QObject>
#include <QTreeView>

namespace fileinfomodel
{
class IFileModel: public QObject
{
    Q_OBJECT
public:
    IFileModel(QObject* parent = nullptr): QObject(parent) {}
    virtual ~IFileModel() = default;

    virtual QString getLogStartTimeStr(const QModelIndex &index) = 0;
    virtual void setCurrentDir(const QString& path, QTreeView* tree) = 0;
    virtual int getCurrentJobNum() = 0;

Q_SIGNALS:
    void dataChanged(const QModelIndex& index);

};

typedef std::shared_ptr<IFileModel> FileModelPtr;

template<typename T>
struct FileModelTrait
{
    typedef int NoDefineBaseType;
    typedef NoDefineBaseType ModelBaseType;
};

}
#endif // IFILEMODEL_H
