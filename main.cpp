/*
	Alex Vladimirov
	SQL Interpreter
	estimated hours: 38
*/

#include <iostream>
#include <conio.h>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

using namespace std;

struct student
{
	string studentName; //Column 1
	string studentID; //Column 2
	string studentAge; //Column 3
};

const int MAX_INP_LEN = 100;

void TEST();

void Input(vector<student> DB, char inp, bool &st); //Takes options input
void FillTestDB(vector<student> &DB); //Fills the initial DB
void SetErr(int code); //Sets an error code and interprets it
void OutputHelp(); //Outputs syntax rules and examples

void OutputDatabase(const vector<student> &DB, string col); //Outputs a specific column or all columns if col == "*"
void OutputDatabase(const vector<student> &DB, string col1, string col2); //Outputs two columns
void OutputDatabase(const vector<student> &DB, string col, const vector<string> &constraints); //Outputs a specific col or all cols with WHERE constraints
void OutputDatabase(const vector<student> &DB, string col1, string col2, const vector<string> &constraints); //Outputs two cols with WHERE constraints
void OutputStudent(const vector<student> &DB, int index, string col); //Outputs a specified member variable in a vector of structs

vector<string> ParseToVec(string inp); //Parses a string into a vector
void InterpretVec(vector<string> code, vector<student> &DB); //Interprets a vector of sql code
vector<bool> isStatement(vector<string> code); //Checks if string vector elements are SQL statements or clauses
bool checkState(vector<string> code, int pos); //Used by isStatement

void NewEntry(vector<student> &DB, string name, string id, string age); //Creates a new database entry
student NewStudent(string name, string id, string age); //Create a new strudent struct

void exec(vector<student> &dataBase); 

int main()
{
	vector<student> dataBase; //makes the initial database
	FillTestDB(dataBase); //fills the database
	bool start = false; //starts interpeter when set to true
	char input;
	while (!start)
	{
		cout << "Enter [1] to start SQL interpreter, [2] for help, or [3] to view the database:" << endl;
		input = _getch();
		Input(dataBase, input, start);
	}

	bool exit = false;
	char ans = 'y';

	while (!exit)
	{
		//TEST();
		exec(dataBase);

		cout << "again? [y/n]:";
		cin >> ans;
		cin.ignore(1000, '\n');
		if (ans == 'n')
			exit = true;
	}

	system("pause");
}

void exec(vector<student> &dataBase) 
{
	string inp;
	cout << ": ";
	getline(cin, inp);

	vector<string> sqlCode = ParseToVec(inp);

	if (sqlCode.size() < 4)
	{
		SetErr(1);
	}
	else if (sqlCode[sqlCode.size() - 1] != ";")
	{
		SetErr(2);
	}
	else
	{
		InterpretVec(sqlCode, dataBase);
	}
}

void TEST()
{
	vector<student> dataBase;
	FillTestDB(dataBase);

	vector<string> exCode = { "SELECT", "*", "FROM", "students", "WHERE", "studentName", "=", "Alex Vladimirov", ";", "\n"};
	vector<bool> isState;

	isState = isStatement(exCode);
	cout << "isState:";
	for (auto it : isState)
		cout << it << " ";

	InterpretVec(exCode, dataBase);

	OutputDatabase(dataBase, "*");
	//OutputDatabase(const vector<student> &DB, string col1, string col2)
	//OutputDatabase(const vector<student> &DB, string col, vector<string> constraints)
	//OutputDatabase(const vector<student> &DB, string col1, string col2, vector<string> constraints)
	return;
}

void Input(vector<student> DB, char inp, bool &st)
{
	if (inp == '1')
	{
		st = true;
		return;
	}
	else if (inp == '2')
	{
		OutputHelp();
		return;
	}
	else if (inp == '3')
	{
		OutputDatabase(DB, "*");
	}
	else
	{
		SetErr(1);
	}
}

void FillTestDB(vector<student> &DB)
{
	for (int i = 0; i < 5; i++)
	{
		DB.push_back(student());
	}
	DB[0].studentName = "Ezequiel Arpin";
	DB[0].studentID = "11110000";
	DB[0].studentAge = "19";

	DB[1].studentName = "Ronnie Harpster";
	DB[1].studentID = "00110011";
	DB[1].studentAge = "18";

	DB[2].studentName = "Maude Lumpkins";
	DB[2].studentID = "10101111";
	DB[2].studentAge = "21";

	DB[3].studentName = "Maryetta Fusco";
	DB[3].studentID = "00001111";
	DB[3].studentAge = "19";

	DB[4].studentName = "Irene Strikland";
	DB[4].studentID = "01011100";
	DB[4].studentAge = "20";

	return;
}

void SetErr(int code)
{
	switch (code)
	{
	case 1:
		cout << "ERROR: incorrect input" << endl;
		break;
	case 2:
		cout << "ERROR: missing ';'" << endl;
		break;
	default:
		break;
	}
	return;
}

void OutputHelp()
{
	cout << "Syntax for most inputs(combinations can be made):" << endl;
	cout << "SELECT colum_name FROM table_name WHERE column_name operator value;" << endl;
	cout << "SELECT column_name1 column_name2 FROM Students WHERE column_name operator value AND col op val;" << endl;
	cout << "SELECT column_name FROM Students WHERE column_name operator value OR col op val;" << endl;
	cout << "INSERT INTO Students VALUES firstName lastName idNumber age;" << endl << endl;
	cout << "Example:" << endl;
	cout << "SELECT * FROM Students WHERE studentName = 'Alex Vladimirov';" << endl;
	cout << "SELECT studentName studentAge FROM Students WHERE studentAge != 18;" << endl;
	cout << "INSERT INTO Students VALUES Alex Vladimirov 11111111 19;" << endl;
	cout << "Press any key" << endl;
	_getch();
	return;
}

vector<string> ParseToVec(string inp)
{
	vector<string> tokens;
	int currPos = 0;
	string temp;
	
	for (int i = 0; i < inp.length(); i++)
	{
		if (inp[i] == ' ')
		{
			string substr = inp.substr(currPos, i - currPos);
			tokens.push_back(substr);
			currPos = i + 1;
		}
		else if (inp[i] == '\'') //To make sure that something like 'hello world' is put into a single vector element
		{
			currPos++;
			int apostIndex = inp.find('\'', currPos);
			string substr = inp.substr(currPos, (apostIndex)-currPos);
			tokens.push_back(substr);
			i = apostIndex + 1;
		}
		else if (inp[i] == ';')
		{
			string substr = inp.substr(currPos, (i - currPos));
			tokens.push_back(substr);
			tokens.push_back(";");
			return tokens;
		}
	}
	return tokens;
}

/*
The following function Interprets these sql statements from a string vector:
SELECT - extracts data from a database
INSERT INTO - inserts new data into a database

and these clauses/statements:
WHERE, FROM, AND, OR
*/
void InterpretVec(vector<string> code, vector<student> &DB)
{
	vector<string> statements = { "DELETE", "SELECT", "INSERT" }; 
	vector<string> clauses = { "DISTINCT", "FROM", "WHERE", "AND", "OR", "VALUES" }; 

	int initState = 0;

	for (size_t i = 0; i < statements.size(); i++)
	{
		if (code[0] == statements[i])
		{
			initState = i;
		}
	}

	vector<bool> isState = isStatement(code);

	int pos = 1; //current position in the vector
	switch (initState)
	{
	case 0: //DELETE
		break;
	case 1: //SELECT
		{
			//sample: 
			//SELECT * FROM Students;           WHERE studentName = 'A V' OR studentID = '1111000';
			//  0    1   2     3                  4  <-- pos
			//two col sample:
			//SELECT studentName studentAge FROM Students WHERE
			//  0          1            2       3      4      5  <-- pos
			
			string col1 = code[pos]; //pos = 1
			pos++; //pos = 2
			string col2;
			bool twoCol = false; //if there are two columns
			bool areConst = false; //if there are constraints
			string andOr = "none"; //can be changed to AND or OR
			vector<string> constraints;
			string prevWord;
			prevWord = "selCol";

			while (code[pos] != ";")
			{
				if (prevWord == "selCol" && code[pos] != "FROM")
				{
					col2 = code[2];
				}
				else if (code[pos] == "FROM")
				{
					prevWord = "FROM";
				}
				else if (prevWord == "FROM")
				{
					prevWord = "Students";
				}
				else if (code[pos] == "WHERE")
				{
					areConst = true; //There are constraints for output
					for (int i = pos + 1; i < code.size(); i++) //checks if there is an AND or OR
					{
						if (isState[i] == true)
						{
							andOr = code[i];
						}
					}
					if (andOr != "none")
					{

						constraints.push_back(code[pos + 1]);
						constraints.push_back(code[pos + 2]);
						constraints.push_back(code[pos + 3]);
						//skips the AND or OR
						constraints.push_back(code[pos + 5]);
						constraints.push_back(code[pos + 6]);
						constraints.push_back(code[pos + 7]);
						constraints.push_back(andOr); //appends AND or OR to the end
					}
					else
					{
						constraints.push_back(code[pos +1]);
						constraints.push_back(code[pos + 2]);
						constraints.push_back(code[pos + 3]);
					}
					prevWord = "null"; //causes the loop to run to the end
					break;
				}
				pos++;
			}

			if (!twoCol && !areConst)
			{
				OutputDatabase(DB, col1);
			}
			else if (!twoCol && areConst)
			{
				OutputDatabase(DB, col1, constraints);
			}
			else if (twoCol && !areConst)
			{
				OutputDatabase(DB, col1, col2);
			}
			else if (twoCol && areConst)
			{
				OutputDatabase(DB, col1, col2, constraints);
			}

		}
		break;
	case 2: //INSERT INTO
		//sample:
		//INSERT INTO Students VALUES Alex Vladimirov 11110000 19
		//  0      1      2      3      4      5         6      7 <-- pos

		string firstName = code[4];
		string lastName = code[5];
		string fullName = firstName + " " + lastName;
		string id = code[6];
		string age = code[7];
		NewEntry(DB, fullName, id, age); //DB is always Students
		break;
	}

	//if (code[0] == "DELETE")
		
}

void OutputDatabase(const vector<student> &DB, string col)
{
	for (size_t i = 0; i < DB.size(); i++)
	{
		OutputStudent(DB, i, col); 
		cout << endl;
	}
	return;
}

void OutputDatabase(const vector<student> &DB, string col1, string col2)
{	
		for (size_t i = 0; i < DB.size(); i++)
		{
			OutputStudent(DB, i, col1);
			cout << " ";
			OutputStudent(DB, i, col2);
			cout << endl;
		}
}

void OutputDatabase(const vector<student> &DB, string col, const vector<string> &constraints)
{
	vector<string> possCols;
	possCols = { "studentName", "studentID", "studentAge" };

	vector<string> possOpers;
	possOpers = { "=", "!=" };

	int colIndex;
	int operIndex;

	for (size_t i = 0; i < possCols.size(); i++)
	{
		if (possCols[i] == constraints[0])
			colIndex = i;
	}

	for (size_t i = 0; i < possOpers.size(); i++)
	{
		if (possOpers[i] == constraints[1])
			operIndex = i;
	}

	string val = constraints[2];

	switch (colIndex)
	{
	case 0: //studentName
			for (size_t i = 0; i < DB.size(); i++)
			{
				if (operIndex == 0) //=
				{
					if (DB[i].studentName == val) 
					{
						OutputStudent(DB, i, col);
						cout << endl;
					}
				}
				else if (operIndex == 1) //!=
				{
					if (DB[i].studentName != val)
					{
						OutputStudent(DB, i, col);
						cout << endl;
					}
				}
			}
		break;
	case 1: //studentID
		for (size_t i = 0; i < DB.size(); i++)
		{
			if (operIndex == 0) //=
			{
				if (DB[i].studentID == val) 
				{
					OutputStudent(DB, i, col);
					cout << endl;
				}
			}
			else if (operIndex == 1) //!=
			{
				if (DB[i].studentID != val) 
				{
					OutputStudent(DB, i, col);
					cout << endl;
				}
			}
		}
		break;
	case 2: //studentAge
		for (size_t i = 0; i < DB.size(); i++)
		{
			if (operIndex == 0) //=
			{
				if (DB[i].studentAge == val) 
				{
					OutputStudent(DB, i, col);
					cout << endl;
				}
			}
			else if (operIndex == 1) 
			{
				if (DB[i].studentAge != val) //!=
				{
					OutputStudent(DB, i, col);
					cout << endl;
				}
			}
		}
		break;
	}
	return;
}

void OutputDatabase(const vector<student> &DB, string col1, string col2, const vector<string> &constraints)
{
	vector<string> possCols;
	possCols = { "studentName", "studentID", "studentAge" };

	vector<string> possOpers;
	possOpers = { "=", "!=" };

	int colIndex;
	int operIndex;

	for (size_t i = 0; i < possCols.size(); i++)
	{
		if (possCols[i] == constraints[0])
			colIndex = i;
	}

	for (size_t i = 0; i < possOpers.size(); i++)
	{
		if (possOpers[i] == constraints[1])
			operIndex = i;
	}

	string val = constraints[2];

	switch (colIndex)
	{
	case 0: //studentName
		for (size_t i = 0; i < DB.size(); i++)
		{
			if (operIndex == 0) //=
			{
				if (DB[i].studentName == val)
				{
					OutputStudent(DB, i, col1);
					cout << " ";
					OutputStudent(DB, i, col2);
					cout << endl;
				}
			}
			else if (operIndex == 1) //!=
			{
				if (DB[i].studentName != val)
				{
					OutputStudent(DB, i, col1);
					cout << " ";
					OutputStudent(DB, i, col2);
					cout << endl;
				}
			}
		}
		break;
	case 1: //studentID
		for (size_t i = 0; i < DB.size(); i++)
		{
			if (operIndex == 0) //=
			{
				if (DB[i].studentID == val)
				{
					OutputStudent(DB, i, col1);
					cout << " ";
					OutputStudent(DB, i, col2);
					cout << endl;
				}
			}
			else if (operIndex == 1) //!=
			{
				if (DB[i].studentID != val)
				{
					OutputStudent(DB, i, col1);
					cout << " ";
					OutputStudent(DB, i, col2);
					cout << endl;
				}
			}
		}
		break;
	case 2: //studentAge
		for (size_t i = 0; i < DB.size(); i++)
		{
			if (operIndex == 0) //=
			{
				if (DB[i].studentAge == val)
				{
					OutputStudent(DB, i, col1);
					cout << " ";
					OutputStudent(DB, i, col2);
					cout << endl;
				}
			}
			else if (operIndex == 1)
			{
				if (DB[i].studentAge != val) //!=
				{
					OutputStudent(DB, i, col1);
					cout << " ";
					OutputStudent(DB, i, col2);
					cout << endl;
				}
			}
		}
		break;
	}
}

void OutputStudent(const vector<student> &DB, int index, string col)
{
	if (col == "*" || col == "studentName")
		cout << DB[index].studentName; if (col == "*") cout << " ";
	if(col == "*" || col == "studentID")
		cout << DB[index].studentID; if (col == "*") cout << " ";
	if(col == "*" || col == "studentAge")
		cout << DB[index].studentAge;
	return;
}

void NewEntry(vector<student> &DB, string name, string id, string age)
{
	student stu = NewStudent(name, id, age);
	DB.push_back(stu);
	return;
}

student NewStudent(string name, string id, string age)
{
	student stu;
	stu.studentName = name;
	stu.studentAge = age;
	stu.studentID = id;
	return stu;
}

vector<bool> isStatement(vector<string> code)
{
	vector<bool> isState;
	for (int i = 0; i < code.size(); i++)
	{
		isState.push_back(checkState(code, i));
	}
	
	return isState;
}

bool checkState(vector<string> code, int pos)
{
	vector<string> statements = { "DELETE", "SELECT", "INSERT INTO",
		"DISTINCT", "FROM", "WHERE", "AND", "OR", "VALUES" };

	int sSize = statements.size();

	for (int k = 0; k < sSize; k++)
	{
		if (code[pos] == statements[k])
		{
			return true;
		}
	}
	return false;
}
