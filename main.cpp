#include <iostream>
#include <tclap/CmdLine.h>
#include <memory>
#include <regex>


#include "record.h"
#include "lb-dmut.h"
#include "cell-dmut.h"


using namespace std;


const string VERSION = "1.0.0";


struct Arguments {
  int starting_population[2];
  double t_end;
};


// Split a string like "0.3 17.4" into two doubles
template<typename T>
void split_pair_input(string pair, T *v) {
  cerr << "called split_pair_input for unspecilized type. Unlikely to work as expected" << endl;
  regex re("[^\\s]+");
  smatch m;
  int i = 0;
  while (regex_search(pair, m, re)) {
    for (auto x: m) {
      v[i++] = static_cast<T>(x);
    }
    pair = m.suffix().str();
  }
}
template<>
void split_pair_input(string pair, double *v) {
  regex re("\\d+\\.?\\d*");
  smatch m;
  int i = 0;
  while (regex_search(pair, m, re)) {
    for (auto x: m) {
      v[i++] = stod(x);
    }
    pair = m.suffix().str();
  }
}
template<>
void split_pair_input(string pair, int *v) {
  regex re("\\d+");
  smatch m;
  int i = 0;
  while (regex_search(pair, m, re)) {
    for (auto x: m) {
      v[i++] = stoi(x);
    }
    pair = m.suffix().str();
  }
}



int main(int argc, char **argv) {

  Arguments a;
  try {
    TCLAP::CmdLine cmd("General treatment simulator", ' ', VERSION);

    TCLAP::ValueArg<string> a_n0("n", "n0", "Starting cell count", false, "2 0", "\'integer integer\'", cmd);
    TCLAP::ValueArg<string> a_birth_rate("b", "birth-rate", "Birth rate", false, "1.0 1.0", "\'double double\'", cmd);
    TCLAP::ValueArg<string> a_death_rate("d", "death-rate", "Death rate", false, "0.1 0.1", "\'double double\'", cmd);
    TCLAP::ValueArg<string> a_birth_interaction("p", "birth-interaction", "Birth interaction", false, "0.0, 0.0", "\'double double\'", cmd);
    TCLAP::ValueArg<string> a_death_interaction("q", "death-interaction", "Death interaction", false, "0.001, 0.001", "\'double double\'", cmd);
    TCLAP::ValueArg<double> a_t("t", "t-max", "Simulation time", false, 10, "double", cmd);
    TCLAP::ValueArg<double> a_mutation_rate("u", "mutation-rate", "Mutation rate", false, 1e-3, "double", cmd);

    cmd.parse(argc, argv);

    split_pair_input(a_n0.getValue(), a.starting_population);
    split_pair_input(a_birth_rate.getValue(), birth_rates);
    split_pair_input(a_death_rate.getValue(), death_rates);
    split_pair_input(a_birth_interaction.getValue(), birth_interactions);
    split_pair_input(a_death_interaction.getValue(), death_interactions);
    a.t_end = a_t.getValue();
    mutation_rate = a_mutation_rate.getValue();

    // Sanity checks
    for (int i = 0; i < 2; ++i) {
      assert(a.starting_population[i] >= 0);
      assert(birth_rates[i] > 0); // Technically simulating a process without birth could be interesting. But, as implemented, the simulation would never end.
      assert(death_rates[i] >= 0);
      assert(birth_interactions[i] >= 0);
      assert(death_interactions[i] >= 0);
    }
    assert(a.starting_population[0] + a.starting_population[1] > 0);
    assert(a.t_end > 0);
    assert(mutation_rate <= 1.0);

  } catch (TCLAP::ArgException &e) {
    cerr << "TCLAP Error: " << e.error() << endl << "\targ: " << e.argId() << endl;
    return 1;
  }

  auto record = make_shared<RecordForest>();
  LB<Cell> lb(record);
  lb.set_cell_count(0, a.starting_population[0]);
  lb.set_cell_count(1, a.starting_population[1]);
  lb.simulate(a.t_end);
  cout << *record << endl;
}
