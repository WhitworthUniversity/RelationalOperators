#include <iostream>
#include <vector>
#include "dataitem.h"
#include "relationalop.h"
#include "datagen.h"
#include "select.h"
#include "project.h"
#include "stupidsimplejoin.h"
#include "union.h"
using namespace std;

int DataItem::PAGESIZE = 100;

int main() {
	vector<int> attrs;
	attrs.push_back(0); attrs.push_back(0); attrs.push_back(1); attrs.push_back(3);
	Predicate predSel;
	predSel.lexpr = "$0"; predSel.op = GT; predSel.rexpr = "10";
	Predicate predJoin;
	predJoin.lexpr = "$0"; predJoin.op = EQ; predJoin.rexpr = "$0";

	RelationalOperator *ro1 = new Project(attrs, false, 
								new Select(predSel, 
									new StupidSimpleJoin(predJoin, new DataGenerator(1000), new DataGenerator(550))));
	RelationalOperator *ro2 = new Project(attrs, false, 
								new Select(predSel, new 
									StupidSimpleJoin(predJoin, new DataGenerator(1000), new DataGenerator(550))));
	RelationalOperator *ro = new Union(ro1, ro2);
	DataItem di;

	ro->open();
	do {
		di = ro->next();
		if (!di.fEOF) {
			cout << di.display() << "\n";
		}
	} while (!di.fEOF);
	ro->close();

	delete ro;

	cout << "press enter to quit";
	getchar();
}