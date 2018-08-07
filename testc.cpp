#include <iostream>


#include "lb-changing.h"
#include "cell-changing.h"


using namespace std;


int main() {
  LB<Cell> lb;
  lb.set_cell_count(0, 100);
  lb.simulate(100);
}
