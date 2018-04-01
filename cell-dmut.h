#ifndef __CELL_DMUT__
#define __CELL_DMUT__


#include <cstdlib>
#include <random>


static double mutation_rate;
static double birth_rates[2];
static double death_rates[2];
static double birth_interactions[2];
static double death_interactions[2];


class Cell {
public:
  Cell(size_t type)
    : type(type) { }

  static size_t get_type_count() {
    return 2;
  }

  double get_birth_rate() {
    return birth_rates[type];
  }

  double get_death_rate() {
    return death_rates[type];
  }

  double get_birth_interaction() {
    return birth_interactions[type];
  }

  double get_death_interaction() {
    return death_interactions[type];
  }

  template <typename T>
  size_t mutate(T rng) {
    return (type + (std::uniform_real_distribution<float>()(rng) < mutation_rate)) % 2;
  }


private:
  size_t type;

};


#endif
