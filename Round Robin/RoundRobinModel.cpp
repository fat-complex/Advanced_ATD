#include "RoundRobinModel.h"

#include <QTimer>

#include <random>

using dst = std::uniform_int_distribution<std::mt19937_64::result_type>;

namespace
{
std::random_device rnd_device;
std::mt19937_64 rng{rnd_device()};

std::string uniqueName()
{
    auto randChar = []() -> char
    {
        constexpr char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        constexpr size_t max_index = (sizeof(charset) - 1);
        auto distribution = dst(0, max_index - 1);
        return charset[distribution(rng)];
    };
    std::string str(5, 0);
    std::generate_n(str.begin(), 4, randChar);
    return str;
}

std::vector<ExecutorData> makeExecutorsData(const Settings& settings)
{
    const auto& executors = settings.executorMinMax;
    const auto& performance = settings.performanceMinMax;
    auto distribution1 = dst(executors.first, executors.second);
    auto distribution2 = dst(performance.first, performance.second);

    std::vector<ExecutorData> executorsData(distribution1(rng));
    for (auto& executorData : executorsData)
    {
        executorData.executorName = QString::fromStdString(uniqueName());
        executorData.performance = distribution2(rng);
    }
    return executorsData;
}

std::vector<TaskData> makeTasksData(const Settings& settings)
{
    const auto& tasks = settings.tasksMinMax;
    const auto& complexity = settings.complexityMinMax;
    auto distribution1 = dst(tasks.first, tasks.second);
    auto distribution2 = dst(complexity.first, complexity.second);

    std::vector<TaskData> tasksData(distribution1(rng));
    for (auto& taskData : tasksData)
    {
        taskData.name = QString::fromStdString(uniqueName());
        taskData.complexity = distribution2(rng);
    }
    return tasksData;
}

} // namespace

ExecutorsModel::ExecutorsModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int ExecutorsModel::rowCount(const QModelIndex& parent) const
{
    return executors_.count();
}

QVariant ExecutorsModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= executors_.count())
        return {};

    const ExecutorData& entry = executors_[index.row()];
    switch (role)
    {
        case NameRole:
            return entry.executorName;
        case PerformanceRole:
            return entry.performance;
        default:
            return {};
    }
}

void ExecutorsModel::push(const ExecutorData& entry)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    executors_.append(entry);
    endInsertRows();
}

void ExecutorsModel::remove(int index, int count)
{
    beginRemoveRows(QModelIndex(), index, index + count - 1);
    for (int row = 0; row < count; ++row)
    {
        executors_.removeAt(index);
    }
    endRemoveRows();
}

void ExecutorsModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, executors_.count() - 1);
    executors_.clear();
    endRemoveRows();
}
QList<ExecutorData> ExecutorsModel::executors() const
{
    return executors_;
}

QHash<int, QByteArray> ExecutorsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[PerformanceRole] = "performance";
    return roles;
}

InfoModel::InfoModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int InfoModel::rowCount(const QModelIndex& parent) const
{
    return info_.size();
}
QVariant InfoModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= info_.size())
        return {};

    const InfoData& entry = info_[index.row()];
    switch (role)
    {
        case ExecutorNameRole:
            return entry.executorName;
        case TaskNameRole:
            return entry.taskName;
        case RoundRole:
            return entry.round;
        default:
            return {};
    }
}

void InfoModel::push(const InfoData& entry)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    info_.push_back(entry);
    endInsertRows();
}

QHash<int, QByteArray> InfoModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ExecutorNameRole] = "executor";
    roles[TaskNameRole] = "taskName";
    roles[RoundRole] = "round";
    return roles;
}

RoundRobinModel::RoundRobinModel(QObject* parent)
    : QObject(parent)
    , executorsModel_{std::make_unique<ExecutorsModel>(this)}
    , tasksModel_{std::make_unique<QStringListModel>(this)}
    , infoModel_{std::make_unique<InfoModel>(this)}
    , settings_{std::make_unique<Settings>()}
    , tasks_{makeTasksData(*settings_)}
    , timer_{this}
{
    connect(&timer_,
            &QTimer::timeout,
            this,
            [this]()
            {
                static int round = 1;
                stopModeling_ = true;
                timer_.stop();
                for (const auto& executor: executorsModel_->executors())
                {
                    auto& tasks = currentTasks_[executor.executorName];
                    std::remove_if(tasks.begin(), tasks.end(), [&executor, this] (const TaskData& task)
                    {
                        bool to_be_deleted = task.complexity - executor.performance <= 0;
                        if (to_be_deleted)
                        {
                            infoModel_->push({executor.executorName, task.name, round++});
                        }
                        return to_be_deleted;
                    });
                }
            });
    connect(
        this,
        &RoundRobinModel::startedAMethod,
        this,
        [this]
        {
            if (stopModeling_)
                return;
            AMethod();
        },
        Qt::QueuedConnection);
}

ExecutorsModel* RoundRobinModel::executorsModel() const
{
    return executorsModel_.get();
}
QStringListModel* RoundRobinModel::tasks(QString executorName)
{
    qDebug() << executorName;
    QStringList list;
    for (const auto& t : currentTasks_[executorName])
    {
        qDebug() << t.name;
        list << t.name;
    }
    tasksModel_->clearItemData({});
    tasksModel_->setStringList(list);

    return tasksModel_.get();
}
InfoModel* RoundRobinModel::infoModel() const
{
    return infoModel_.get();
}
Settings* RoundRobinModel::getSettings() const
{
    return settings_.get();
}

void RoundRobinModel::AMethod()
{
    static int nextExecIndex = 0;
    static int nextTaskIndex = 0;

    const auto& executors = executorsModel_->executors();
    if (!stopModeling_)
    {
        const auto& executor = executors[nextExecIndex % executors.size()];
        currentTasks_[executor.executorName].push_back(tasks_[nextTaskIndex % tasks_.size()]);
        nextExecIndex++;
        nextTaskIndex++;
        emit startedAMethod();
    }
    else
        nextExecIndex = 0;
}

void RoundRobinModel::BMethod()
{
    auto executors = executorsModel_->executors();
    auto first = *executors.begin();
    for (int i = executors.size() - 1; i >= 1; --i)
    {
        const auto& tasks = currentTasks_[executors[i].executorName];
        currentTasks_[executors[i - 1].executorName].assign(tasks.begin(), tasks.end());
    }
    executors[executors.size() - 1] = first;
}

void RoundRobinModel::restart()
{
    init();
    stopModeling_ = false;

    timer_.start(settings_->timeout * 1000);
    if (settings_->isAMethod)
    {
        AMethod();
        return;
    }
    BMethod();
}

void RoundRobinModel::pause()
{
    emit paused(true);
}

void RoundRobinModel::init()
{
    executorsModel_->clear();
    for (const auto& d : makeExecutorsData(*settings_))
    {
        executorsModel_->push(d);
    }
    tasks_.clear();
    tasks_ = makeTasksData(*settings_);
}