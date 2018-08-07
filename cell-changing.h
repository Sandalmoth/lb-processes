#ifndef __CELL_CHANGING__
#define __CELL_CHANGING__


#include <cmath>
#include <cstdlib>
#include <random>


class Cell {
public:
  Cell(size_t id)
    : id(id) { }

  static size_t get_type_count() {
    return 1;
  }

  size_t get_type_id() {
    return id;
  }

  template <typename T>
  double get_birth_rate(double t, T first, T last) {
    return 1.0 + (static_cast<int>(t/5) % 2);
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
  size_t mutate(double t, T rng) {
    return id;
  }



private:
  size_t id = 1;

};


#endif
