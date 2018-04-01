#include <iostream>
#include <tclap/CmdLine.h>
#include <memory>


#include "record.h"
#include "lb-dmut.h"
#include "cell-dmut.h"


using namespace std;


int main(int argc, char **argv) {

  mutation_rate = 1e-3;
  birth_rates[0] = birth_rates[1] = 1.0;
  death_rates[0] = death_rates[1] = 0.1;
  birth_interactions[0] = birth_interactions[1] = 0.0;
  death_interactions[0] = death_interactions[1] = 0.00001;

  auto record = make_shared<RecordForest>();
  LB<Cell> lb(record);
  lb.set_cell_count(0, 10);
  lb.simulate(100);
  cout << *record << endl;
}
