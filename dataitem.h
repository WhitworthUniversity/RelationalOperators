#pragma once

#include <string>
#include <vector>
#include <sstream>
using namespace std;

enum DataType { INT, DOUBLE, STRING };
class DataValue {
public:
	enum DataType type;
	int intVal;
	double dblVal;
	string strVal;

	static DataValue cast(string& st, DataType& type) {
		DataValue ret;
		ret.type = type;
		switch (type) {
		case INT:
			ret.intVal = stoi(st);
			break;
		case DOUBLE:
			ret.dblVal = stod(st);
			break;
		case STRING:
			ret.strVal = st;
			break;
		}
		return ret;
	}
};

bool operator<(const DataValue& val1, const DataValue& val2) {
	if (val1.type == INT)
		return val1.intVal < val2.intVal;
	else if (val1.type == DOUBLE)
		return val1.dblVal < val2.dblVal;
	else if (val1.type == STRING)
		return val1.strVal < val2.strVal;

	return false;
}
bool operator>(const DataValue& val1, const DataValue& val2) {
	return val2 < val1;
}
bool operator==(const DataValue& val1, const DataValue& val2) {
	return !(val1 < val2 && val2 < val1);
}
bool operator<=(const DataValue& val1, const DataValue& val2) {
	return val1 < val2 || val1 == val2;
}
bool operator>=(const DataValue& val1, const DataValue& val2) {
	return val1 > val2 || val1 == val2;
}
bool operator!=(const DataValue& val1, const DataValue& val2) {
	return !(val1 == val1);
}
ostream& operator<<(ostream& out, const DataValue& val) {
	switch (val.type) {
	case INT:
		out << val.intVal;
		break;
	case DOUBLE:
		out << val.dblVal;
		break;
	case STRING:
		out << val.strVal;
		break;
	}
	return out;
}

//Our data item class is simply a vector of integers.
// Lame, maybe. Simple, certainly
class DataItem {
public:
	static int PAGESIZE;	//Assume we can fit 100 tuples per page

	vector<DataValue> data;	//The actual data for this data item
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
		enum DataType type = STRING;
		DataValue lval, rval;
		lval.type = STRING; lval.strVal = lexpr;
		rval.type = STRING; rval.strVal = rexpr;
		if (lexpr[0] == '$') {
			lval = diIn.data[(int)(lexpr[1] - '0')];
			rval = DataValue::cast(rval.strVal, lval.type);
		}
		else if (rexpr[0] == '$') {
			rval = diIn.data[(int)(rexpr[1] - '0')];
			lval = DataValue::cast(lval.strVal, rval.type);
		}

		return evalVal(lval, rval);
	}

	//Evaluate two data items (join)
	bool evaluate(DataItem &diInLeft, DataItem &diInRight) {
		return evalVal(diInLeft.data[(int)(lexpr[1] - '0')], diInRight.data[(int)(rexpr[1] - '0')]);
	}

	int getleftattr() { return lexpr[0] == '$' ? lexpr[1] - '0' : -1; }

	int getrightattr() { return rexpr[0] == '$' ? rexpr[1] - '0' : -1; }

private:
	bool evalVal(DataValue lval, DataValue rval) {
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

