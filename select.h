#pragma once

#include "relationalop.h"

//The select operator
class Select : public RelationalOperator {
private:
	RelationalOperator* input;	//The input operator (note, this is deleted in the destructor)
	Predicate pred;				//The select predicate
	vector<DataItem> cache;
	int icache;

public:
	Select(Predicate p, RelationalOperator* in) : pred(p), input(in), icache(0) {}
	~Select() {
		delete input;
	}

	//Simply open the child input
	virtual void open()
	{
		input->open();

		//Read a page
		readpage(input, cache, icache);
	}
	//Evaluate each data item until one is found that passes the predicate or the end of input is found
	virtual DataItem next() {
		DataItem diIn;
		bool pass = false;
		do {
			if (icache >= cache.size())
				readpage(input, cache, icache);
			diIn.fEOF = cache.size() == 0 || cache[icache].fEOF;
			if (!diIn.fEOF) {
				diIn = cache[icache++];
				pass = pred.evaluate(diIn);	//If not end of input, do we have a data item that passes the predicate?
			}

		} while (!diIn.fEOF && pass == false);

		return diIn;
	}
	//Close the child input operator
	virtual void close() { input->close(); }
};

