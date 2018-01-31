#include <iostream>


#include "lb-process.h"
#include "cell.h"


using namespace std;


int main() {
  cout << Cell::const_birth_rate::value << endl;
  cout << Cell::const_death_rate::value << endl;
  cout << Cell::const_interaction_death_rate::value << endl;
  LBProcess<Cell> lbp;
  for (int i = 0; i < 1000; ++i) {
    lbp.add_cell(Cell(1.0, 0.5, 0.01));
  }
  lbp.simulate(2701);
}
