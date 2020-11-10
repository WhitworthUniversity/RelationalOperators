#pragma once

#include "relationalop.h"

//The union operator
class Union : public RelationalOperator {
private:
	RelationalOperator* linput;		//left child input
	RelationalOperator* rinput;		//right child input
	bool leof, reof;				//is either input at the end?
	bool lread;						//which input do we read from next

public:
	Union(RelationalOperator* l, RelationalOperator* r) { linput = l; rinput = r; leof = false; reof = false; lread = true; }
	~Union() { delete linput; delete rinput; }

	//Open the child inputs
	virtual void open() { linput->open(); rinput->open(); }
	//Get a data item from one input and output it
	virtual DataItem next() {
		DataItem diIn, diOut;
		if (leof && reof)
			//We're at the end of both inputs. Get out
			diOut.fEOF = true;
		else if (lread || reof) {
			//Output data from the left input
			diOut = linput->next();
			leof = diOut.fEOF;
		}
		else if (!lread || leof) {
			//Output data from the right input
			diOut = rinput->next();
			reof = diOut.fEOF;
		}
		lread = !lread;

		return diOut;
	}
	virtual void close() { linput->close(); rinput->close(); }
};

