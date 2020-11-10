#pragma once

#include "relationalop.h"
#include <algorithm>
using std::sort;

//The project operator
class Project : public RelationalOperator {
private:
	RelationalOperator* input;	//The input operator (it must be deleted in the destructor)
	vector<int> attributes;		//The attributes (0-based) to keep in the output
	bool dupelim;
	vector<DataItem> cached;
	int icached;

	DataItem project(DataItem diIn)
	{
		DataItem diOut;
		diOut.fEOF = diIn.fEOF;
		for (unsigned int i = 0; !diIn.fEOF && i<attributes.size(); i++)
			diOut.data.push_back(diIn.data[attributes[i]]);

		return diOut;
	}

public:
	Project(vector<int>& as, bool dupelim, RelationalOperator* in) : input(in), dupelim(dupelim), icached(0) {
		for (unsigned int i=0; i<as.size(); i++)
			attributes.push_back(as[i]);
	}
	~Project() {
		delete input;
	}

	//Open the child input
	virtual void open()
	{
		input->open();
		if (dupelim)
		{
			readentireinput(input, cached, icached);
			sort(cached.begin(), cached.end(), DataItem::lessthan);
			unique(cached.begin(), cached.end(), DataItem::equals);
		}
	}
	//Read each data item and output it's projection, based on the desired attributes
	virtual DataItem next() {
		return (dupelim) ? cached[icached++] : project(input->next());
	}
	//Free up the resources
	virtual void close() {input->close(); }
};

