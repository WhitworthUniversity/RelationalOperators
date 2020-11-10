#pragma once

#pragma once

#include "relationalop.h"
#include <algorithm>

//The join operator
class SortMergeJoin : public RelationalOperator {
private:
	RelationalOperator* inputLeft; //Two inputs, left and right, which must be deleted by the destructor
	RelationalOperator* inputRight;
	Predicate pred;				//The join predicate
	DataItem::LessThanOnAttr leftcomp;
	DataItem::LessThanOnAttr rightcomp;
	vector<DataItem> cacheLeft, cacheRight;
	int icacheLeft, icacheRight;
	bool leof, reof;		//Have we hit the end of either input?
public:
	SortMergeJoin(Predicate p, RelationalOperator* l, RelationalOperator* r) : pred(p), inputLeft(l), inputRight(r), leof(false), reof(false) {}
	~SortMergeJoin() { delete inputLeft; delete inputRight; }
	//Initialize variables, and open child inputs
	virtual void open()
	{
		inputLeft->open();
		inputRight->open();

		readentireinput(inputLeft, cacheLeft, icacheLeft);
		readentireinput(inputRight, cacheRight, icacheRight);
		
		leftcomp = DataItem::LessThanOnAttr(pred.getleftattr());
		rightcomp = DataItem::LessThanOnAttr(pred.getrightattr());
		sort(cacheLeft.begin(), cacheLeft.end(), leftcomp);
		sort(cacheRight.begin(), cacheRight.end(), rightcomp);
	}

	virtual DataItem next() {
		DataItem diOut;
		bool eof = cacheLeft[icacheLeft].fEOF || cacheRight[icacheRight].fEOF;
		if (eof)	//We're already done
			diOut.fEOF = true;
		else {
			bool pass = false;
			do {
				pass = pred.evaluate(cacheLeft[icacheLeft], cacheRight[icacheRight]);
				if (!pass)
				{
					if (cacheLeft[icacheLeft].data[pred.getleftattr()] < cacheRight[icacheRight].data[pred.getrightattr()])
						icacheLeft++;
					else
						icacheRight++;
				}
				else
				{
					if (icacheLeft < cacheLeft.size() || pred.evaluate(cacheLeft[icacheLeft+1], cacheRight[icacheRight]))
						icacheLeft++;
					else
						icacheRight++;
				}
				eof = cacheLeft[icacheLeft].fEOF || cacheRight[icacheRight].fEOF;
			} while (!eof && !pass);

			diOut.fEOF = eof;
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

