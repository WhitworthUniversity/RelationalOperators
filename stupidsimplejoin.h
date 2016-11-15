#pragma once

#include "relationalop.h"

//The join operator
class StupidSimpleJoin : public RelationalOperator {
private:
	RelationalOperator* linput; //Two inputs, left and right, which must be deleted by the destructor
	RelationalOperator* rinput;
	Predicate pred;				//The join predicate
	DataItem diInLeft;			//Need to hold onto the left (outer) input data item we can find an item from the right to match it
	bool leof;		//Have we hit the ned of the left (outer) input?
public:
	StupidSimpleJoin(Predicate p, RelationalOperator* l, RelationalOperator* r) {
		pred = p; linput = l; rinput = r;
	}
	~StupidSimpleJoin() { delete linput; delete rinput; }
	//Initialize variables, and open child inputs
	virtual void open()
	{
		linput->open(); 
		rinput->open(); 
		leof = false; 
		
		diInLeft = linput->next();
	}

	virtual DataItem next() {
		DataItem diInRight, diOut;
		diInRight.fEOF = true;
		if (leof)	//We're already done
			diOut.fEOF = true;
		else {
			bool pass = false;
			do {
				//If we're not at the end of the left input, read a new item from the right
				if (!diInLeft.fEOF) {
					diInRight = rinput->next();
					//If we hit the end of the right input, it's time to start the right input over and advance the left
					if (diInRight.fEOF)
					{
						rinput->close();
						rinput->open();
						diInLeft = linput->next();
					}

					//If we have two good inputs, see if they join
					if (!diInRight.fEOF && !diInLeft.fEOF)
						pass = pred.evaluate(diInLeft, diInRight);
				}
			} while (!diInLeft.fEOF && !pass);

			diOut.fEOF = diInLeft.fEOF;
			if (pass) {
				//If we have two data items that matched on the join predicate, build a new data item
				// that is the pairing of the two input items
				for (unsigned int i=0; i<diInLeft.data.size(); i++)
					diOut.data.push_back(diInLeft.data[i]);
				for (unsigned int i=0; i<diInRight.data.size(); i++)
					diOut.data.push_back(diInRight.data[i]);
			}

		}

		return diOut;
	}

	//Clean up child inputs.
	virtual void close() { linput->close(); rinput->close(); }
};

