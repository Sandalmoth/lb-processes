#ifndef __CELL_DMUT__
#define __CELL_DMUT__


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
    if (id) return 1.0;
    else return 0.1;
  }

  template <typename T>
  double get_death_rate(T first, T last) {
    return 0.1;
  }

  template <typename T>
  double get_birth_interaction(T first, T last) {
    // return 0.0000010;
    // return 0.0000005;
    return 0.0000001;
    // return 0.0;
  }

  template <typename T>
  double get_death_interaction(T first, T last) {
    // return 0.0;
    // return 0.0000005;
    return 0.0000009;
    // return 0.0000010;
  }

  template <typename T>
  size_t mutate(T rng) {
    return (id + (std::uniform_real_distribution<float>()(rng) < 0.00001)) % 2;
  }



private:
  size_t id = 1;

};


#endif
