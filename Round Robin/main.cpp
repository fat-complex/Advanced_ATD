#include <string>
#include <iostream>

#include <QGuiApplication>
#include <fstream>
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

void to_model()
{

	std::cout << "\n\nEnter the length of the quantum [1-100]: ";

	int quantum = 0;

	std::cin >> quantum;

	while (quantum < 1 || quantum > 100)
	{
		std::cout << "\nInvalid Length. Enter a valid quantum length in [1-100]: ";
		std::cin >> quantum;
	}


	std::ifstream fin ("Assign-1.ip");

	if (fin.is_open())
	{
		std::string buffer;
		std::string temp;

		getline(fin,buffer);

		std::vector<proccess> jobs;


		//Getting the processes from the file
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

		//Number of Processes
		int total_Processes = jobs.size();


		if (total_Processes > 0)
		{
			std::ofstream fout ("Assign-1.op");

			if(fout.is_open())
			{
				fout << "Quantum Number each " << quantum <<" Cycles, Executing Process' Name\n";

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
								has_burst_zero++;	//Increment size by one
						}
					}

					if (isIdle)
					{
						fout << quantum_Number << ',' << "CPU is Idle" << std::endl;
						quantum_Number++;
						running_Time = running_Time + quantum;
					}
				}

				std::cout <<"\nOutput file generated successfully.\n";
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

int main(int argc, char* argv[])
{
    QGuiApplication guiApp(argc, argv);

    // to_model();

    return guiApp.exec();
}