#pragma once

#include "relationalop.h"

//The join operator
class PageNestedJoin : public RelationalOperator {
private:
	RelationalOperator* inputLeft; //Two inputs, left and right, which must be deleted by the destructor
	RelationalOperator* inputRight;
	Predicate pred;				//The join predicate
	vector<DataItem> cacheLeft, cacheRight;
	int icacheLeft, icacheRight;
	bool leof;		//Have we hit the ned of the left (outer) input?
public:
	PageNestedJoin(Predicate p, RelationalOperator* l, RelationalOperator* r) {
		pred = p; inputLeft = l; inputRight = r;
	}
	~PageNestedJoin() { delete inputLeft; delete inputRight; }
	//Initialize variables, and open child inputs
	virtual void open()
	{
		inputLeft->open();
		inputRight->open();
		leof = false;

		readpage(inputLeft, cacheLeft, icacheLeft);
		readpage(inputRight, cacheRight, icacheRight);
	}

	virtual DataItem next() {
		DataItem diOut;
		if (leof)	//We're already done
			diOut.fEOF = true;
		else {
			bool pass = false;
			do {
				//If we're not at the end of the left input, read a new item from the right
				if (!leof) {
					if (icacheRight >= cacheRight.size())
						readpage(inputRight, cacheRight, icacheRight);
					//If we hit the end of the right input, it's time to start the right input over and advance the left
					if (cacheRight[icacheRight].fEOF)
					{
						inputRight->close();
						inputRight->open();
						readpage(inputRight, cacheRight, icacheRight);
						icacheLeft++;
						if (icacheLeft >= cacheLeft.size())
							readpage(inputLeft, cacheLeft, icacheLeft);
					}

					//If we have two good inputs, see if they join
					if (!cacheRight[icacheRight].fEOF && !cacheLeft[icacheLeft].fEOF)
						pass = pred.evaluate(cacheLeft[icacheLeft], cacheRight[icacheRight++]);
				}
			} while (!cacheLeft[icacheLeft].fEOF && !pass);

			diOut.fEOF = cacheLeft[icacheLeft].fEOF;
			if (pass) {
				//If we have two data items that matched on the join predicate, build a new data item
				// that is the pairing of the two input items
				for (unsigned int i = 0; i<cacheLeft[icacheLeft].data.size(); i++)
					diOut.data.push_back(cacheLeft[icacheLeft].data[i]);
				for (unsigned int i = 0; i<cacheRight[icacheRight].data.size(); i++)
					diOut.data.push_back(cacheRight[icacheRight].data[i]);
			}

		}

		return diOut;
	}

	//Clean up child inputs.
	virtual void close() { inputLeft->close(); inputRight->close(); }
};

