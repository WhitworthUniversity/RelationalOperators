#pragma once

#include <string>
#include <vector>
#include <sstream>
using namespace std;

//Our data item class is simply a vector of integers.
// Lame, maybe. Simple, certainly
class DataItem {
public:
	static int PAGESIZE;	//Assume we can fit 100 tuples per page

	vector<int> data;	//The actual data for this data item
	bool fEOF;			//Whether this marks the end of the data from the input

	string display() {
		stringstream out;
		out << "<";
		for (unsigned int i=0; i<data.size(); i++) {
			out << data[i];
			if (i != data.size()-1) out << ", ";
		}
		out << ">";
		return out.str();
	}

	class LessThanOnAttr
	{
	private:
		int attr;
	public:
		LessThanOnAttr(int a) : attr(a) {}
		bool operator()(DataItem& d1, DataItem& d2) const
		{
			return d1.data[attr] < d2.data[attr];
		}
	};

	static bool equals(DataItem& d1, DataItem& d2)
	{
		return !lessthan(d1, d2) && !lessthan(d2, d1);
	}

	static bool lessthan(DataItem& d1, DataItem d2)
	{
		bool lt = true;
		int s = d1.data.size() < d2.data.size() ? d1.data.size() : d2.data.size();
		for (int i = 0; lt && i<s; i++)
		{
			lt = d1.data[i] < d2.data[i];
		}
		return lt;
	}
};

//A predicate is two operands (left and right) and an operator
//   integer constant operands are simply written as the constant
//   data item attribute values are referenced with '$i', where i is the ith attribute (0-based)
//  e.g. "$1 > 10" wants all data items where the 1st attribute is larger than 10
enum Operator { EQ, NEQ, LT, GT, LTEQ, GTEQ };
class Predicate {
public:
	string lexpr;	//The left-hand operand
	Operator op;	//The operator
	string rexpr;	//The right-hand operand

	//Evaluate a single data item with a constant (select operator)
	bool evaluate(DataItem &diIn) {
		bool pass = false;
		int lval = ((lexpr[0] == '$') ? diIn.data[(int) (lexpr[1] - '0')] : atoi(rexpr.c_str()));
		int rval = ((rexpr[0] == '$') ? diIn.data[(int) (rexpr[1] - '0')] : atoi(rexpr.c_str()));

		return evalVal(lval, rval);
	}

	//Evaluate two data items (join)
	bool evaluate(DataItem &diInLeft, DataItem &diInRight) {
		int lval = ((lexpr[0] == '$') ? diInLeft.data[(int) (lexpr[1] - '0')] : atoi(rexpr.c_str()));
		int rval = ((rexpr[0] == '$') ? diInRight.data[(int) (rexpr[1] - '0')] : atoi(rexpr.c_str()));

		return evalVal(lval, rval);
	}

	int getleftattr() { return lexpr[0] == '$' ? lexpr[1] - '0' : -1; }

	int getrightattr() { return rexpr[0] == '$' ? rexpr[1] - '0' : -1; }

private:
	bool evalVal(int lval, int rval) {
		bool pass = false;
		switch(op) {
			case EQ:
				pass = (lval == rval);
				break;
			case NEQ:
				pass = (lval != rval);
				break;
			case LT:
				pass = (lval < rval);
				break;
			case GT:
				pass = (lval > rval);
				break;
			case GTEQ:
				pass = (lval >= rval);
				break;
			case LTEQ:
				pass = (lval <= rval);
				break;
		}
		return pass;
	}
};

