#include <iostream>
#include <tclap/CmdLine.h>


#include "lb-dmut.h"
#include "cell-dmut.h"


using namespace std;


int main(int argc, char **argv) {
  LB<Cell> lb;
  lb.set_cell_count(0, 100);
  lb.simulate(100);
}
