#ifndef __CELL_TWOFACTOR_H__
#define __CELL_TWOFACTOR_H__


#include <cstdlib>
#include <random>


class Cell {
public:
  Cell() { }

  Cell(size_t discrete, double continuous)
    : discrete(discrete)
    , continuous(continuous) { }


  static size_t get_discrete_options() {
    return 1;
  }


  size_t get_discrete_type() {
    return discrete;
  }
  double get_continuous_type() {
    return continuous;
  }


  // Note possibility of time dependence (double t)
  // and evolutionary game theory (first, last of list of all other cells)

  template <typename T>
  double get_birth_rate(double t, T first, T last) {
    return 1.0;
  }

  template <typename T>
  double get_death_rate(double t, T first, T last) {
    return 0.1;
  }

  template <typename T>
  double get_birth_interaction(double t, T first, T last) {
    return 0.0;
  }

  template <typename T>
  double get_death_interaction(double t, T first, T last) {
    return 0.0000010;
  }

  template <typename T>
  double get_mutation_rate(double t, T first, T last) {
    return 0.000001;
  }


  // Mutate this cell
  template <typename T>
  void mutate(T rng) { }



private:
  size_t discrete = 0;
  double continuous = 0.0;

};


#endif
