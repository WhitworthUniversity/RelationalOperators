#pragma once

#include "dataitem.h"

//A relational operator follows Goetz's iterator model with open, next, and close
class RelationalOperator {
protected:
	static void readpage(RelationalOperator* input, vector<DataItem>& cache, int& icache)
	{
		icache = 0;
		cache.clear();
		for (DataItem di = input->next(); cache.size() < DataItem::PAGESIZE && !di.fEOF; di = input->next())
		{
			cache.push_back(di);
		}
	}

	static void readentireinput(RelationalOperator* input, vector<DataItem>& cache, int& icache)
	{
		icache = 0;
		cache.clear();
		for (DataItem di = input->next(); !di.fEOF; di = input->next())
		{
			cache.push_back(di);
		}
	}


public:
	RelationalOperator() {};
	virtual ~RelationalOperator() {};

	virtual void open() = 0;		//Prepare the input for reading
	virtual DataItem next() = 0;	//Read the next data item (one at a time)
	virtual void close() = 0;		//Close the input (and free any resources used)
};

