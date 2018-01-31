#ifndef __LB_DISCRETE__
#define __LB_DISCRETE__


#include <cassert>
#include <cstdlib>
#include <valarray>
#include <vector>
#include <random>
#include <iostream>
#include <cmath>


template <typename TCell, typename TRng=std::mt19937>
class LB {
public:
  LB() {
    urd = std::uniform_real_distribution<double>(std::nextafter(0.0, 1.0), std::nextafter(1.0, 0.0));
    std::random_device rd;
    rng.seed(rd());
    type_count = TCell::get_type_count();
    X.resize(type_count);
    a.resize(type_count * 3);
    T.resize(type_count * 3);
    P.resize(type_count * 3);
    r.resize(type_count * 3);
    dt.resize(type_count * 3);
    X = 0;
    // a has to be calculated, so no reason to waste time initializing
    T = 0;
    P = 0;
    for (size_t i = 0; i < type_count; ++i) {
      cells.push_back(TCell(i));
    }
  }

  void set_cell_count(size_t type, size_t count) {
    assert (type < type_count);
    X[type] = count;
  }

  void get_birth_rates() {
    for (size_t i = 0; i < type_count; ++i) {
      // a[i*3] = cells[i].get_birth_rate(std::begin(X), std::end(X));
      double rate = cells[i].get_birth_rate(std::begin(X), std::end(X));
      a[i*3] = X[i] * rate;
    }
  }
  void get_death_rates() {
    for (size_t i = 0; i < type_count; ++i) {
      // a[i*3 + 1] = cells[i].get_death_rate(std::begin(X), std::end(X));
      double rate = cells[i].get_death_rate(std::begin(X), std::end(X));
      a[i*3 + 1] = X[i] * rate;
    }
  }
  void get_interaction_death_rates() {
    for (size_t i = 0; i < type_count; ++i) {
      // a[i*3 + 2] = cells[i].get_interaction_death_rate(std::begin(X), std::end(X));
      double rate = cells[i].get_interaction_death_rate(std::begin(X), std::end(X));
      // int sizemult = X[i] * (X[i] - 1);
      int sizemult = X.sum() - 1;
      // if (sizemult < 0) sizemult = 0;
      a[i*3 + 2] = sizemult * X[i] * rate;
    }
  }
  void update_rates() {
    get_birth_rates();
    get_death_rates();
    get_interaction_death_rates();
    // a *= X;
  }

  void init() {
    // Force sequentiality not normally required by valarray.
    // If valarray were to parallellize its 'apply' calls then
    // the random number generation could cause race conditions.
    // Unless I use a threadsafa rng, this is probably the best way.
    // And since it's initialization, it's not really that speed critical.
    for (auto &rr: r) {
      rr = urd(rng);
    }
    P = log(1.0/r);
    // calculate all propensity values (a)
    // These functions should probably be optimized with sfinae
    update_rates();
  }


  void simulate(double interval) {
    double t_end = t + interval;
    init();
    // std::cout.precision(3);
    std::cout << "time\tsize1\tsize2\n";
    while (t < t_end) {
      dt = (P - T) / a;
      // double d = dt.min();

      std::cout << t;
      for (auto x: X) std::cout << '\t' << x;
      std::cout << '\n';
      // for (auto x: P) std::cout << x << '\t';
      // std::cout << std::endl;
      // for (auto x: T) std::cout << x << '\t';
      // std::cout << std::endl;
      // for (auto x: a) std::cout << x << '\t';
      // std::cout << std::endl;
      // for (auto x: dt) std::cout << x << '\t';
      // std::cout << std::endl;
      // std::cout << std::endl;

      size_t u = std::min_element(std::begin(dt), std::end(dt)) - std::begin(dt);
      double d = dt[u];
      t += d;
      int event_celltype = u/3;
      // std::cout << u << '\n';
      if (u%3 == 0) {
        auto new_cell_id = cells[event_celltype].mutate(rng);
        ++X[new_cell_id];
      } else {
        --X[event_celltype];
      }
      T = T + a*d;
      r[u] = urd(rng);
      P[u] += log(1.0/r[u]);
      update_rates();

    }
  }

private:
  double t = 0;
  size_t type_count;
  std::valarray<double> X;
  std::valarray<double> a;
  std::valarray<double> T;
  std::valarray<double> P;
  std::valarray<double> r;
  std::valarray<double> dt;

  std::vector<TCell> cells;

  TRng rng;
  std::uniform_real_distribution<double> urd;

};


#endif
