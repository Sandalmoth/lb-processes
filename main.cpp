#include <iostream>
#include <tclap/CmdLine.h>
#include <memory>


#include "record.h"
#include "lb-dmut.h"
#include "cell-dmut.h"


using namespace std;


int main(int argc, char **argv) {
  auto record = make_shared<RecordForest>();
  LB<Cell> lb(record);
  lb.set_cell_count(0, 10);
  lb.simulate(100);
  cout << *record << endl;
}
