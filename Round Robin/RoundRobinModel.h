#pragma once

#include <QAbstractListModel>
#include <QStringListModel>
#include <QThread>
#include <QTimer>

struct MinMaxRange : QPair<int, int>
{
    Q_GADGET
    Q_PROPERTY(int min MEMBER first)
    Q_PROPERTY(int max MEMBER second)

public:
    MinMaxRange() = default;
    MinMaxRange(int min, int max)
        : QPair<int, int>(min, max)
    {
    }
};

struct Settings : QObject
{
    Q_OBJECT

    Q_PROPERTY(int timeout MEMBER timeout)
    Q_PROPERTY(MinMaxRange executor MEMBER executorMinMax)
    Q_PROPERTY(MinMaxRange performance MEMBER performanceMinMax)
    Q_PROPERTY(MinMaxRange tasks MEMBER tasksMinMax)
    Q_PROPERTY(MinMaxRange complexity MEMBER complexityMinMax)
    Q_PROPERTY(bool Amethod MEMBER isAMethod)
    Q_PROPERTY(bool Bmethod MEMBER isBMethod)

public:
    Settings() = default;

    int timeout{1};
    MinMaxRange executorMinMax{1, 3};
    MinMaxRange performanceMinMax{1, 10};
    MinMaxRange tasksMinMax{1, 10};
    MinMaxRange complexityMinMax{1, 10};
    bool isAMethod{true};
    bool isBMethod{false};
};

struct ExecutorData
{
    QString executorName;
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
    explicit ExecutorsModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role) const override;

    void push(const ExecutorData& entry);

    void remove(int index, int count);

    void clear();

    QList<ExecutorData> executors() const;

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    QList<ExecutorData> executors_;
};

struct TaskData
{
    QString name;
    int complexity;
};

struct InfoData
{
    QString executorName;
    QString taskName;
    int round{0};
};

class InfoModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles
    {
        ExecutorNameRole = Qt::UserRole + 1,
        TaskNameRole,
        RoundRole
    };
    explicit InfoModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role) const override;

    void push(const InfoData& entry);

protected:
    QHash<int, QByteArray> roleNames() const override;
private:
    std::vector<InfoData> info_;
};

class RoundRobinModel : public QObject
{
    Q_OBJECT

    // Q_PROPERTY(QStringListModel* tasks READ tasks CONSTANT)
    Q_PROPERTY(ExecutorsModel* executorsModel READ executorsModel CONSTANT)
    Q_PROPERTY(InfoModel* infoModel READ infoModel CONSTANT)
    Q_PROPERTY(Settings* settings READ getSettings CONSTANT)

signals:
    void paused(bool paused);
    void started();
    void startedAMethod();

public:
    enum Roles
    {
        NameRole = Qt::UserRole + 1,
        PerformanceRole
    };
    explicit RoundRobinModel(QObject* parent = nullptr);

    ExecutorsModel* executorsModel() const;

    Q_INVOKABLE QStringListModel* tasks(QString executorName);

    InfoModel* infoModel() const;

    Settings* getSettings() const;

    Q_INVOKABLE void AMethod();
    Q_INVOKABLE void BMethod();
    Q_INVOKABLE void restart();
    Q_INVOKABLE void pause();

private:
    void init();

    std::unique_ptr<ExecutorsModel> executorsModel_;
    std::unique_ptr<QStringListModel> tasksModel_;
    std::unique_ptr<InfoModel> infoModel_;
    std::unique_ptr<Settings> settings_;
    std::vector<TaskData> tasks_;
    bool stopModeling_{false};
    QTimer timer_;

    std::unordered_map</*executor*/ QString, /*attached tasks*/ std::vector<TaskData>>
        currentTasks_;
};
