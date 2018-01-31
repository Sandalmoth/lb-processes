#include <iostream>


#include "lb-discrete.h"
#include "cell-discrete.h"


using namespace std;


int main() {
  LB<Cell> lb;
  lb.set_cell_count(0, 100);
  lb.simulate(1000);
}
