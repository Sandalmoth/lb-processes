#ifndef __CELL_DISCRETE__
#define __CELL_DISCRETE__


#include <cstdlib>
#include <random>


class Cell {
public:
  Cell(size_t id)
    : id(id) { }

  static size_t get_type_count() {
    return 2;
  }

  size_t get_type_id() {
    return id;
  }

  template <typename T>
  double get_birth_rate(T first, T last) {
    if (id) return 0.11;
    else return 0.09;
  }

  template <typename T>
  double get_death_rate(T first, T last) {
    return 0.01;
  }

  template <typename T>
  double get_interaction_death_rate(T first, T last) {
    return 0.000001;
  }

  template <typename T>
  size_t mutate(T rng) {
    return (id + (std::uniform_real_distribution<float>()(rng) < 0.00001)) % 2;
  }



private:
  size_t id = 1;

};


#endif
