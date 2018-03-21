#include "Matrix.h"


bool Do_double_Operator(char op, stack<string>& stk)
{
	string left, right;
	right = stk.top();
	stk.pop();
	left = stk.top();
	stk.pop();
	Matrix res, matL = find(left), matR = find(right);

	switch (op)
	{
	case '+':
		if (matL.colomn != matR.colomn || matL.line != matR.line)
		{
			cerr << left << " cannot be added with " << right << "!\n";
			return 0;
		}
		res = find(left) + find(right);			//用到重载 右值引用的 =
		break;
	case '-':
		if (matL.colomn != matR.colomn || matL.line != matR.line)
		{
			cerr << left << " cannot be subtracted by " << right << "!\n";
			return 0;
		}
		res = find(left) - find(right);
		break;
	case '*':
		if (matL.colomn != matR.line)
		{
			cerr << left << " cannot be multipled by " << right << "!\n";
			return 0;
		}
		res = find(left) * find(right);
		break;
	case '/':
		if (matL.colomn != matR.line)
		{
			cerr << left << " cannot be divided by " << right << "!\n";
			return 0;
		}
		res = find(left) / find(right);
		if (res.name == "Null")
			return 0;
		break;
	}

	matrixTable.push_back(res);
	stk.push(res.name);

	return 1;
}

bool Do_single_Operator(char op, stack<string>& stk)
{
	string x;
	x = stk.top();
	stk.pop();
	Matrix res, mat = find(x);

	switch (op)
	{
	case '|':
		if (mat.colomn != mat.line)
		{
			cerr << x << "cannot be inversed as it is not a square matrix!" << endl;
			return 0;
		}
		res = !mat;
		if (res.name == "Null")
			return 0;
		break;
	}

	matrixTable.push_back(res);
	stk.push(res.name);

	return 1;
}

bool calc(string postfix)
{
	stringstream s(postfix);
	stack<string> stk;
	char ch;
	string newOperand;

	while (s >> ch, ch != '#')		//流输入不会赋值出'\0'
	{
		switch (ch)
		{
		case '+':
		case '-':
		case '*':
		case '/':
			if (!Do_double_Operator(ch, stk))
				return 0;
			break;
		case '|':
			if (!Do_single_Operator(ch, stk))
				return 0;
			break;
		default:			//Operand
			s.putback(ch);
			s >> newOperand;
			stk.push(newOperand);
		}
	}

	string result = stk.top();
	stk.pop();
	if (!stk.empty())	//其实遇不到
		return 0;

	find(result).output();
	return 1;
}

void welcome()
{
	cout << "Welcome to MATRIX CALCULATOR! You may want to\n\
1.input new matrix\n\
2.check existed matrix\n\
3.use the calculator with operation () + - * |(matrix inversion) /(define A/B as A*(B|) )	\n\
or input any other value to exit.\n";
	int op = 0;
	cin >> op;	//乱输会读不进去
	switch (op)
	{
	case 1:	input_matrix(); break;
	case 2:	display_matrix(); break;
	case 3:	calculator();	break;
	default:
		break;
	}

}

void calculator()
{
	cout << "Please input a legal expression, or input Q to quit.\n";
	string nifix, postfix;

	do {
		do { getline(cin, nifix); } while (nifix.find_first_not_of(' ') == -1);	//即 全是空格 就重读一行

		if (nifix == "Q")
			break;

		postfix.clear();
		if (process(nifix, postfix) == 0 || calc(postfix) == 0)
		{
			cout << "Please input a LEGAL expression!\n";
			continue;
		}

	} while (1);

	welcome();
}
//C+A*B*(B-C)
//a+b*a/(a+b)
//a+a*(a-a)

bool process(string expr, string& pf)
{
	expr += '#';

	stack<char> stk;
	char ch, ch1, op;
	stk.push('#');

	int pos = 0;
	ch = expr.at(pos++);

	bool tag = 0;
	string temp;
	while (!stk.empty())
	{
		if (ch == ' ')			//空格直接跳过
			ch = expr.at(pos++);
		else if (isalpha(ch))
		{
			if (tag == 0)
			{					//开始遇到矩阵名了
				tag = 1;
				temp.clear();
			}
			temp += ch;
			pf += ch;
			ch = expr.at(pos++);
		}
		else					//非字母 非空格 是符号 先处理一下
		{
			if (!temp.empty() && !is_in(temp))		//如果空的肯定是最开头没读入 属于missing operand的错误 下面会报的
			{
				cerr << "illegal input! Matrix " << temp << " has not been defined!\n";
				pf.clear();
				return 0;
			}

			if (isp(ch) == -1)		//其他错误符号
			{
				cerr << "illegal input! Operator " << ch << " is not included in (+ - * / |)\n";
				pf.clear();
				return 0;
			}

			//以下应该就是正确符号了			

			pf += ' ';			//遇到一个符号说明前面的矩阵名都已处理完 前面都加一个空格 防止变成后缀之后 操作数都连到一起去

			ch1 = stk.top();
			if (ch == '|')		//次）高的优先级直接进入
			{
				if (tag == 0)	//前面没有操作数
				{
					cerr << "illegal input! Missing operand!\n";
					pf.clear();
					return 0;
				}
				pf += ch;
				ch = expr.at(pos++);
			}
			else if (isp(ch1)<icp(ch))
			{
				if (ch != '(')
				{
					if (tag == 0)	//连续读进两个新二元运算符  或者第一个就是	即前面缺少操作数
					{
						cerr << "illegal input! Missing operand!\n";
						pf.clear();
						return 0;
					}
					tag = 0;		//只有在读进新二元运算符的时候才置一次 0		从栈里面pop出的时候不动  | ( )也不动  
				}
				stk.push(ch);
				ch = expr.at(pos++);
			}
			else if (isp(ch1)>icp(ch))
			{
				op = stk.top();
				stk.pop();
				pf += op;
			}
			else		//#  )
			{
				if (stk.top() == '(')	//弹掉之外还要继续往后读一个
				{
					if (ch != ')')
					{
						cerr << "illegal input! Missing ')'!\n";
						pf.clear();
						return 0;
					}
					ch = expr.at(pos++);
				}
				stk.pop();
			}


		}
	}

	pf += '#';
	//cout << pf << endl;
	return 1;
}

const Matrix& find(string name)
{
	for (list<Matrix>::iterator i = matrixTable.begin(); i != matrixTable.end(); ++i)
		if (i->name == name)
			return *i;							//之前已经确保 is_in 了			不碍事
	cerr << name << " is not in the existing matrix list!\n";
	exit(-1);
}

int isp(char c)
{
	switch (c)
	{
	case '|':return 7;
	case '*':
	case '/':return 6;
	case '+':
	case '-':return 4;

	case '(':return 1;
	case ')':return 8;
	case '#':return 0;

	default:
		return -1;
	}

}

int icp(char c)
{
	switch (c)
	{
	case '|':return 2;
	case '*':
	case '/':return 5;
	case '+':
	case '-':return 3;
	case '(':return 8;
	case ')':return 1;
	case '#':return 0;

	default:
		return -1;
	}

}

void display_matrix()
{
	for (list<Matrix>::iterator i = matrixTable.begin(); i != matrixTable.end(); ++i)
		i->output();

	welcome();
}

void input_matrix()
{
	cout << "You want to input matrix data\n\
1.from console\n\
2.from file(\"matrix_data.txt\") in the same directory of this project	\n\
or any other value to cancer.\n";
	int op = 0;
	cin >> op;
	switch (op)
	{
	case 1:	input_from_console(); break;
	case 2:	input_from_file();	break;

	default:
		break;
	}

	welcome();
}

bool is_in(string name)
{
	for (list<Matrix>::iterator i = matrixTable.begin(); i != matrixTable.end(); ++i)
		if (i->name == name)
			return 1;
	return 0;
}

bool is_legal(string name)
{
	for (string::iterator i = name.begin(); i != name.end(); ++i)
		if (!isalpha(*i))
			return 0;
	return 1;
}

void input_from_console()
{
	cout << "Please input the name of the new matrix, which should be composed only by letter word.\n";
	string name;
	cin >> name;
	while (!is_legal(name) || is_in(name))
	{
		if (!is_legal(name))
			cout << "Illegal input! Remember the name should be composed only by letter word. Please re-input.\n";
		else
			cout << "This name has existed! Please re-input.\n";

		cin >> name;
	}

	cout << "Now you can input the line and the colomn of the matrix, followed by the data.\n";
	int line, col;
	cin >> line >> col;

	Matrix temp(name, line, col, cin);
	matrixTable.push_back(temp);
}

void input_from_file()
{
	cout << "Please input the name of the new matrix, which should be composed only by letter word.\n";
	string name;
	cin >> name;
	while (!is_legal(name) || is_in(name))
	{
		if (!is_legal(name))
			cout << "Illegal input! Remember the name should be composed only by letter word. Please re-input.\n";
		else
			cout << "This name has existed! Please re-input.\n";

		cin >> name;
	}

	static ifstream in_f("matrix_data.txt");	//只要确保 文件中没有多余的空行
	if (!in_f)
	{
		cerr << "Fail to open the file!\n";
		exit(-1);
	}

	int line = 0, col = 0;					//加个初始值 方便调试
	in_f >> line >> col;

	if (line <= 0 || col <= 0)
	{
		cerr << "invalid input!\n";
		exit(-1);
	}

	Matrix temp(name, line, col, in_f);
	matrixTable.push_back(temp);

	cout << "Matrix " << name << " has been successfully input from file!\n\n";
}

int main()
{
	welcome();

	return 0;
}


//