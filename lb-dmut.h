#ifndef __LB_DMUT__
#define __LB_DMUT__


#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <random>
#include <valarray>
#include <vector>

#include "deep_stack.h"
#include "record.h"


template <typename TCell, typename TRng=std::mt19937>
class LB {
public:
  LB(std::shared_ptr<Record> record)
    : record(record) {
    urd = std::uniform_real_distribution<double>(std::nextafter(0.0, 1.0), 1.0);
    std::random_device rd;
    rng.seed(rd());
    type_count = TCell::get_type_count();
    X.resize(type_count);
    a.resize(type_count * 2);
    T.resize(type_count * 2);
    P.resize(type_count * 2);
    r.resize(type_count * 2);
    dt.resize(type_count * 2);
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
    for (size_t i = 0; i < count; ++i) {
      size_t id = ids.pop();
      extant_ids[type].push_back(id);
      record->insert(id, type);
    }
    X[type] = count;
  }

  void get_birth_rates() {
    for (size_t i = 0; i < type_count; ++i) {
      double rate = cells[i].get_birth_rate();
      double interaction = cells[i].get_birth_interaction();
      int sizemult = X.sum() - 1;
      double ai = X[i] * rate - sizemult * X[i] * interaction;
      if (ai >= 0.0)
        a[i*2] = ai;
      else {
        a[i*2] = 0;
        a[i*2 + 1] -= ai;
      }
    }
  }
  void get_death_rates() {
    for (size_t i = 0; i < type_count; ++i) {
      double rate = cells[i].get_death_rate();
      double interaction = cells[i].get_death_interaction();
      int sizemult = X.sum() - 1;
      a[i*2 + 1] = X[i] * rate + sizemult * X[i] * interaction;
    }
  }

  void update_rates() {
    get_death_rates(); // order is important!
    get_birth_rates();
  }

  void init() {
    for (auto &rr: r) {
      rr = urd(rng);
    }
    P = log(1.0/r);
    // calculate all propensity values (a)
    update_rates();
  }


  void simulate(double interval) {
    double t_end = t + interval;
    init();

    while (t < t_end) {
      dt = (P - T) / a;

      size_t u = std::min_element(std::begin(dt), std::end(dt)) - std::begin(dt);
      double d = dt[u];
      t += d;
      int event_celltype = u/2;
      if (u%2 == 0) {
        --X[event_celltype];
        auto new_cell_type1 = cells[event_celltype].mutate(rng);
        auto new_cell_type2 = cells[event_celltype].mutate(rng);
        ++X[new_cell_type1];
        ++X[new_cell_type2];

        int id_index = std::uniform_int_distribution<int>(0, extant_ids[event_celltype].size())(rng);
        size_t id = extant_ids[event_celltype][id_index];
        size_t new_ids[2] = {ids.pop(), ids.pop()};
        size_t new_types[2] = {new_cell_type1, new_cell_type2};
        record->insert(t, new_ids, new_types, id);
        ids.push(id);
        extant_ids[event_celltype].erase(extant_ids[event_celltype].begin() + id_index);

      } else {
        --X[event_celltype];

        int id_index = std::uniform_int_distribution<int>(0, extant_ids[event_celltype].size())(rng);
        size_t id = extant_ids[event_celltype][id_index];
        ids.push(id);
        extant_ids[event_celltype].erase(extant_ids[event_celltype].begin() + id_index);
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

  DeepStack ids;

  std::shared_ptr<Record> record;

  std::vector<size_t> extant_ids[2];

};


#endif
