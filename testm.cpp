#include <iostream>


#include "lb-dmut.h"
#include "cell-dmut.h"


using namespace std;


int main() {
  LB<Cell> lb;
  lb.set_cell_count(0, 100);
  lb.simulate(100);
}
