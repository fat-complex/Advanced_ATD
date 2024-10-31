#pragma once

#include <QAbstractListModel>
#include <QStringListModel>

struct Entry
{
    QString executor_name;
    int performance;
};

class ExecutorsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles
    {
        NameRole = Qt::UserRole + 1,
        PerformanceRole
    };
    ExecutorsModel(QObject* parent = nullptr)
        : QAbstractListModel(parent)
    {
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return executors_.count();
    }

    QVariant data(const QModelIndex& index, int role) const override
    {
        if (index.row() < 0 || index.row() >= executors_.count())
            return {};

        const Entry& entry = executors_[index.row()];
        switch (role)
        {
            case NameRole:
                return entry.executor_name;
            case PerformanceRole:
                return entry.performance;
            default:
                return {};
        }
    }

    void push(const Entry& entry)
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        executors_.append(entry);
        endInsertRows();
    }

    void remove(int index, int count)
    {
        beginRemoveRows(QModelIndex(), index, index + count - 1);
        for (int row = 0; row < count; ++row)
        {
            executors_.removeAt(index);
        }
        endRemoveRows();
    }

protected:
    QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> roles;
        roles[NameRole] = "name";
        roles[PerformanceRole] = "performance";
        return roles;
    }

private:
    QList<Entry> executors_;
};

class RoundRobinModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringListModel* tasks READ tasks CONSTANT)
    Q_PROPERTY(ExecutorsModel* executorsModel READ executorsModel CONSTANT)
public:
    enum Roles
    {
        NameRole = Qt::UserRole + 1,
        PerformanceRole
    };
    explicit RoundRobinModel(QObject* parent = nullptr)
        : QObject(parent)
        , executors_model_{std::make_unique<ExecutorsModel>(this)}
        , tasks_model_{std::make_unique<QStringListModel>(this)}
    {
    }

    ExecutorsModel* executorsModel()
    {
        return executors_model_.get();
    }

    QStringListModel* tasks()
    {
        QStringList list;
        list << "A"
             << "B"
             << "C";
        tasks_model_->setStringList(list);

        return tasks_model_.get();
    }

private:
    std::unique_ptr<ExecutorsModel> executors_model_;
    std::unique_ptr<QStringListModel> tasks_model_;
};
