#include <QAbstractListModel>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QObject>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct proccess
{
    std::string PID;
    int arrival_Time;
    int burst_Time;

    proccess()
    {
        PID = "";
        arrival_Time = 0;
        burst_Time = 0;
    }
};

class RoundRobinModel;

void to_model(RoundRobinModel& model)
{

    std::cout << "\n\nEnter the length of the quantum [1-100]: ";

    int quantum = 0;

    std::ifstream fin("Assign-1.ip");

    if (fin.is_open())
    {
        std::string buffer;
        std::string temp;

        getline(fin, buffer);

        std::vector<proccess> jobs;

        // Getting the processes from the file
        while (!fin.eof())
        {
            getline(fin, buffer);

            if (!buffer.empty())
            {
                proccess P;

                int i = 0;

                for (i = 0; buffer[i] != ','; i++)
                {
                    P.PID.push_back(buffer[i]);
                }

                i++;

                temp = "";

                for (; buffer[i] != ','; i++)
                {
                    temp.push_back(buffer[i]);
                }

                P.arrival_Time = atoi(temp.c_str());

                i++;

                temp = "";

                for (; buffer[i] != '\0'; i++)
                {
                    temp.push_back(buffer[i]);
                }

                P.burst_Time = atoi(temp.c_str());

                jobs.push_back(P);
            }
        }

        fin.close();

        // Number of Processes
        int total_Processes = jobs.size();

        if (total_Processes > 0)
        {
            std::ofstream fout("Assign-1.op");

            if (fout.is_open())
            {
                fout << "Quantum Number each " << quantum << " Cycles, Executing Process' Name\n";

                int running_Time = 1;

                int quantum_Number = 1;

                bool isIdle = true;

                int has_burst_zero = 0;

                while (has_burst_zero != total_Processes)
                {
                    isIdle = true;

                    for (int i = 0; i < total_Processes; i++)
                    {
                        if (jobs[i].arrival_Time < running_Time && jobs[i].burst_Time > 0)
                        {
                            fout << quantum_Number << ',' << jobs[i].PID << std::endl;

                            jobs[i].burst_Time = jobs[i].burst_Time - quantum;

                            running_Time = running_Time + quantum;
                            quantum_Number++;

                            isIdle = false;

                            if (jobs[i].burst_Time <= 0)
                                has_burst_zero++; // Increment size by one
                        }
                    }

                    if (isIdle)
                    {
                        fout << quantum_Number << ',' << "CPU is Idle" << std::endl;
                        quantum_Number++;
                        running_Time = running_Time + quantum;
                    }
                }

                std::cout << "\nOutput file generated successfully.\n";
            }

            else
                std::cout << "\nThe output file could not be created.\n";
        }

        else
            std::cout << "\nThe file is empty. There are no proccesses to schedule.\n";
    }

    else
        std::cout << "\nThe input file could not be opened.\n";
}

struct Entry
{
    QString executor_name;
    int performance;
};

class RoundRobinModel : public QAbstractListModel
{
    QOBJECT_H

    Q_PROPERTY(QStringList tasks READ tasks)
public:
    enum Roles
    {
        NameRole = Qt::UserRole + 1,
        PerformanceRole
    };
    explicit RoundRobinModel(QObject* parent = nullptr): QAbstractListModel(parent) {}

    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return executors_.count();
    }

    QVariant data(const QModelIndex& index, int role) const override
    {
        if (index.row() < 0 || index.row() >= executors_.count())
            return {};

        const Entry &entry = executors_[index.row()];
        switch (role) {
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
        for (int row = 0; row < count; ++row) {
            executors_.removeAt(index);
        }
        endRemoveRows();
    }

    QStringList tasks() const
    {
        return QStringList() << "A" << "B" << "C";
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

int main(int argc, char* argv[])
{
    QGuiApplication guiApp(argc, argv);

    QQmlApplicationEngine engine;
    QQmlContext* context = engine.rootContext();

    RoundRobinModel model{&guiApp};
    model.push({"GG", 1});
    model.push({"WP", 2});

    context->setContextProperty("roundRobin", QVariant::fromValue(&model));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &guiApp, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);

    const QUrl url(QString(QStringLiteral("qrc:main.qml")));
    engine.load(url);
    if (engine.rootObjects().isEmpty())
    {
        return -1;
    }

    to_model(model);

    return guiApp.exec();
}