#include <iostream>
using namespace std;
#include <vector>
#include<string>
#include <sstream>
#include<fstream>
#include<queue>
#include <memory.h>

//#define DEBUG

struct force
{
	int element[5];
	force(const force& rd)
	{
		for (int i = 0; i < 5; i++)
			element[i] = rd.element[i];
	}
	force()
	{
	}
	force operator-(force &rd)
	{
		force temp;
		for (int i = 0; i < 5; i++)
			temp.element[i] = element[i] - rd.element[i];
		return temp;
	}
	force operator+(force &rd)
	{
		force temp;
		for (int i = 0; i < 5; i++)
			temp.element[i] = element[i] + rd.element[i];
		return temp;
	}

	bool operator>=(const force &rd)
	{
		for (int i = 0; i < 5; i++)
		{
			if (element[i] < rd.element[i])
				return false;
		}
		return true;
	}
	force& operator=(const force &rd)
	{
		for (int i = 0; i < 5; i++)
			element[i] = rd.element[i];
		return *this;
	}
	friend ostream& operator<<(ostream &output,force rd)
	{
		cout << "(";
		for (int i = 0; i < 5; i++)
		{
			cout << rd.element[i];
			if (i != 4)
				cout << ", ";
		}
		cout << ")";
	}
};
struct request
{
	int index;
	force  Force;
	bool release;
	friend ostream& operator<<(ostream &output,request rd)
	{
		cout << "(" << rd.index<<" ";
		for (int i = 0; i < 5; i++)
			cout << ", " << rd.Force.element[i];
		cout << ")";
		return output;
	}
};

vector<request> Requests;
queue<request>waiting;

vector<force>Available, Allocation, Max, Need;
vector<int> safeSequence;

string fileName = "test.txt";

void Loaddata();
void calculateNeed();
bool safe(int index);
bool valid(int index);
int main()
{
	Loaddata();
	calculateNeed();

	if (safe(-1) == true)
	{
		cout << "Initial state: safe, safe sequence = ( ";
		for (int i = 0; i < 5; i++)
		{
			cout << safeSequence[i];
			if (i != 4)
				cout << ", ";
		}
		cout << ')' << endl;
		safeSequence.clear();
	}
	else
	{
		cout << "Initial state: unsafe";
		getchar();
		return 0;
	}

	for (int i = 0; i < Requests.size(); i++)
	{
		cout << Requests[i];
		if (Requests[i].release == true)
			cout << " r ";
		else
			cout << " a ";
		cout << ": AVAILABLE= " << Available[0] << endl;

		if (Requests[i].release == true)
		{
			Available[0] = Available[0] + Requests[i].Force;
			cout << Requests[i]<<": gid "<<Requests[i].index<<" finish,, AVAILABLE= "<<Available[0]<<endl;
		}
		else if (!valid(i))
		{
			cout << Requests[i] << " :invalid request, not granted" << endl;
		}
		else if (safe(i))
		{
			Available[0] = Available[0] + Allocation[Requests[i].index];
			cout << Requests[i] << "granted, safe sequence =( ";
			for (int i = 0; i < 5; i++)
			{
				cout << safeSequence[i];
				if (i != 4)
					cout << ", ";
			}
			cout << ')' << endl;

			cout << Requests[i] << ": gid " << Requests[i].index << " finish,, AVAILABLE= " << Available[0] << endl;
			safeSequence.clear();
		}
		else
		{
			waiting.push(Requests[i]);
			cout << Requests[i] << "not granted safe sequence, push into waiting queue"<<endl;
		}
	}

	getchar();
}

void Loaddata()
{
	//string fileName;
	//cout << "Input file name:";
	//cin >> fileName;
	ifstream file(fileName, ios::in);
	if (!file)
	{
		cout << "File " << fileName << " could not be opened" << endl;
		exit(0);
	}

	string line;
	int garbage;

	vector<force> *currentMatrix;
	string currentMatrixName;

	while (getline(file, line) && line.size() != 0)
	{
		if (line[0] != '/')
		{
			if (line[0] == '#')
			{
				if (line == "#AVAILABLE")
					currentMatrix = &Available;
				if (line == "#MAX")
					currentMatrix = &Max;
				if (line == "#ALLOCATION")
					currentMatrix = &Allocation;
				if (line == "#REQUEST")
					break;
				currentMatrixName = line;
			}
			else
			{
				force *temp = new force();

				stringstream lineStream;
				lineStream << line;
				if (currentMatrixName != "#AVAILABLE")
					lineStream >> garbage;

				for (int i = 0; i < 5; i++)
					lineStream >> temp->element[i];

				currentMatrix->push_back(temp[0]);
			}
		}
	}
	while (getline(file, line) && line.size() != 0)
	{
		if (line[0] != '/')
		{
			request temp;
			stringstream lineStream;
			lineStream << line;
			lineStream >> temp.index;
			for (int i = 0; i < 5; i++)
				lineStream >> temp.Force.element[i];
			char x;
			if (lineStream >> x and x == 'a')
				temp.release = false;
			else
				temp.release = true;
			Requests.push_back(temp);
		}
	}
}

bool safe(int index)
{
	force localAvailable = Available[0];

	bool satisfy[5];
	memset(satisfy, 0, sizeof(satisfy));
	if (index == -1)
	{
		for (int q = 0; q < 5; q++)
		{
			for (int i = 0; i < 5; i++)
			{
				if (satisfy[i] == false)
				{
					if (localAvailable >= Need[i])
					{
						localAvailable = localAvailable + Allocation[i];
						satisfy[i] = true;
						safeSequence.push_back(i);
					}
				}
			}
		}
	}
	else
	{
		localAvailable = localAvailable - Requests[index].Force;
		for (int q = 0; q < 5; q++)
		{
			for (int i = 0; i < 5; i++)
			{
				if (satisfy[i] == false)
				{
					bool localSafe = true;
					if (Need[i] >= localAvailable)
					{
						localSafe = false;
						break;
					}
					if (localSafe == true)
					{
						localAvailable = localAvailable + Allocation[i];
						satisfy[i] = true;
						safeSequence.push_back(i);
					}
				}
			}
		}
	}
	if (safeSequence.size() != 5)
		return false;
	return true;
}

void calculateNeed()
{
	for (int i = 0; i < Max.size(); i++)
	{
		force *temp = new force();
		*temp = Max[i] - Allocation[i];

		Need.push_back(*temp);
	}
}

bool valid(int index)
{
	force temp = Need[Requests[index].index] - Requests[index].Force;
	for (int i = 0; i < 5; i++)
	{
		if (temp.element[i] < 0)
			return false;
	}
	return true;
}
