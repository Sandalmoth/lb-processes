#include <iostream>


#include "next-lb.h"
#include "reactcell.h"


using namespace std;


int main() {
  NextLB<Reactcell> lbp;
  for (int i = 0; i < 100; ++i) {
    lbp.add_cell(Reactcell(1.0, 0.5, 0.01));
  }
  lbp.simulate(10);
}
