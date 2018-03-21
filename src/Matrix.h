#pragma once

#include<iostream>
#include<fstream>
#include<sstream>
#include<iomanip>

#include<cstdlib>
#include<cassert>
#include<cstring>

#include<vector>
#include<list>
#include<string>
#include<stack>


using namespace std;


class Matrix
{
public:
	string name;
	int line;
	int colomn;

	vector<double>* data;

	Matrix() {}
	Matrix(string nm, int ln, int col, istream& in)
	{
		name = nm;
		line = ln;
		colomn = col;
		data = new vector<double>[line];
		for (int i = 0; i<line; ++i)
			for (int j = 0; j < col; ++j)
			{
				double temp;
				in >> temp;
				data[i].push_back(temp);
			}
	}

	Matrix(string nm, int ln, int col)
	{
		name = nm;
		line = ln;
		colomn = col;
		data = new vector<double>[line];
		for (int i = 0; i < line; ++i)
			data[i].resize(colomn, 0);
	}

	void output(ostream& out = cout)	const
	{
		out << "Matrix " << name << ":\n";

		for (int i = 0; i < line; ++i)
		{
			for (int j = 0; j < colomn; ++j)
				out << setiosflags(ios::fixed) << setprecision(2) << setw(8) << data[i][j];
			out << endl;
		}
	}


	Matrix(const Matrix &x)
	{
		name = x.name;
		colomn = x.colomn;
		line = x.line;

		data = new vector<double>[line];
		for (int i = 0; i < line; ++i)
			for (int j = 0; j < colomn; ++j)
				data[i].push_back(x.data[i][j]);
	}


	Matrix(Matrix &&temp)	//转移构造函数
	{
		data = temp.data;
		temp.data = 0;

		line = temp.line;
		colomn = temp.colomn;
		name = temp.name;
	}

	~Matrix()
	{
		if (data == 0)		//为配合转移构造函数 必须要有的
			return;
		delete[] data;
		data = 0;
	}

	Matrix& operator=(const Matrix& source)
	{
		if (this == &source)
			return *this;

		this->~Matrix(); //直接写~Matrix();会无法识别

		colomn = source.colomn;
		line = source.line;
		name = source.name;
		data = new vector<double>[line];
		for (int i = 0; i < line; ++i)
			for (int j = 0; j < colomn; ++j)
				data[i].push_back(source.data[i][j]);

		return *this;
	}

	void operator=(Matrix&& temp)		//为了增加效率 赋值操作也重载一个右值引用
	{
		if (this == &temp)
			return;

		this->~Matrix(); //直接写~Matrix();会无法识别

		data = temp.data;
		temp.data = 0;

		line = temp.line;
		colomn = temp.colomn;
		name = temp.name;

	}

	Matrix operator+(const Matrix& source)	const
	{
		if (line != source.line || colomn != source.colomn)				//debug 用
		{
			cerr << name << " cannot be added with " << source.name << "!\n";
			exit(0);
		}
		string res = '(' + name + '+' + source.name + ')';
		Matrix result(res, line, colomn);
		for (int i = 0; i < line; ++i)
			for (int j = 0; j < colomn; ++j)
				result.data[i][j] = data[i][j] + source.data[i][j];

		return result;						//像这些地方都用到了转移构造函数！
	}

	Matrix operator-(const Matrix& source)	const
	{
		if (line != source.line || colomn != source.colomn)				//debug 用
		{
			cerr << name << " cannot be subtracted by " << source.name << "!\n";
			exit(0);
		}
		string res = '(' + name + '-' + source.name + ')';
		Matrix result(res, line, colomn);
		for (int i = 0; i < line; ++i)
			for (int j = 0; j < colomn; ++j)
				result.data[i][j] = data[i][j] - source.data[i][j];

		return result;
	}

	Matrix operator*(const Matrix& source) const
	{
		if (colomn != source.line)
		{
			cerr << "Wrong multiple!" << endl;
			exit(0);
		}
		string res = '(' + name + '*' + source.name + ')';
		Matrix result(res, line, source.colomn);
		for (int i = 0; i < line; ++i)
			for (int j = 0; j < source.colomn; ++j)
			{
				double temp = 0;
				for (int k = 0; k < source.line; ++k)
					temp += data[i][k] * source.data[k][j];
				result.data[i][j] = temp;
			}

		return result;
	}

	Matrix operator/(const Matrix& source) const
	{
		if (colomn != source.line)
		{
			cerr << "Wrong division!" << endl;
			exit(0);
		}
		if ((!source).name == "Null")
			return Matrix("Null", 0, 0);

		string res = '(' + name + '/' + source.name + ')';
		Matrix result(res, line, source.colomn);
		result = *this * !source;

		return result;
	}


	int find_first_row_not_of_0(int ii)	const
	{
		for (int i = ii; i < line; ++i)
			if (fabs(data[i][ii])>1e-6)
				return i;
		return -1;
	}

	Matrix operator ! () const		//| 是不可以的 必须在已有操作符基础上进行重载
	{
		if (colomn != line)
		{
			cerr << "Wrong inverse!" << endl;
			exit(0);
		}
		string res = '(' + name + '|' + ')';
		Matrix mat_inv(res, line, colomn), mat(*this);
		for (int i = 0; i < line; ++i)
			mat_inv.data[i][i] = 1;

		for (int ii = 0; ii < line; ++ii)
		{
			int k = mat.find_first_row_not_of_0(ii);
			if (k == -1)
			{
				cerr << name << " cannot be inversed as it is a singular matrix!\n";
				return Matrix("Null", 0, 0);
			}
			if (k != ii)
			{
				vector<double> temp = mat.data[ii];
				mat.data[ii] = mat.data[k];
				mat.data[k] = temp;

				temp = mat_inv.data[ii];
				mat_inv.data[ii] = mat_inv.data[k];
				mat_inv.data[k] = temp;
			}

			double temp = mat.data[ii][ii];
			for (int i = 0; i < line; ++i)
				mat.data[ii][i] /= temp;
			for (int i = 0; i < line; ++i)
				mat_inv.data[ii][i] /= temp;

			for (int i = 0; i < line; ++i)
			{
				if (i == ii)
					continue;
				double temp = mat.data[i][ii];
				for (int j = 0; j < line; ++j)
					mat.data[i][j] -= mat.data[ii][j] * temp;
				for (int j = 0; j < line; ++j)
					mat_inv.data[i][j] -= mat_inv.data[ii][j] * temp;
			}
		}

		return mat_inv;
	}

};



void welcome();//欢迎界面 主界面

void input_matrix();//功能1
void input_from_console();//功能1-1
void input_from_file();//功能1-2
bool is_legal(string name);//功能1子功能a 判断输入的矩阵名是否合法
bool is_in(string name);//功能1子功能b 判断输入的矩阵名是否已经存在

void display_matrix();//功能2

void calculator();//功能3
bool process(string expr, string& pf);//功能3子功能a 将中缀表达式（利用栈）转化为后缀表达式
int icp(char c);//功能3子功能a子功能 栈外优先级
int isp(char c);//功能3子功能a子功能 栈内优先级
bool calc(string postfix);//功能3子功能b 对后缀表达式（利用栈）进行操作符和操作数的运算
bool Do_double_Operator(char op, stack<string>& stk);//功能3子功能b子功能 二目运算
bool Do_single_Operator(char op, stack<string>& stk);//功能3子功能b子功能 单目运算
const Matrix& find(string name);//功能3子功能a子功能子功能 根据字符串名字查找矩阵操作数 （事先已确认有这个名字的矩阵）


list<Matrix> matrixTable;//全局变量 矩阵表

