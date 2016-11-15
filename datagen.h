#pragma once

#include "relationalop.h"

class DataGenerator : public RelationalOperator {
private:
	int iRow;	//Which row was last output?
	int cRow;	//How many rows max?
public:
	DataGenerator(int rows) { cRow = rows; }
	//Initialize iRow
	virtual void open() { iRow = 0; }
	//Generate a new data item as the pair <x, x/7>
	virtual DataItem next() {
		DataItem di;
		
		di.fEOF = (iRow >= cRow);
		if (!di.fEOF) {
			di.data.push_back(iRow);
			di.data.push_back(iRow/7);
			iRow++;
		}
		return di;
	}
	virtual void close() {}
};

