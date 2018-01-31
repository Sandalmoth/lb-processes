#ifndef __NEXT_LB_H__
#define __NEXT_LB_H__


#include <vector>
#include <numeric>
#include <random>
#include <iostream>
#include <algorithm>
#include <type_traits>
#include <cassert>
#include <memory>
#include <queue>
#include <functional>
#include <cmath>


#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

template <typename TCell, typename TRng = std::mt19937>
class NextLB {
public:
  struct State {
    std::shared_ptr<TCell> type; // shared between all rates for this type
    std::shared_ptr<size_t> N; // cell count, shared with all rates for this type

    // This function get's more information than it ought to. System state is defined by the pairs of type and N
    // no other features of the State class are neccessary.
    double (TCell::*rate)();

    double T = 0; // internal time
    double P; // event probability
    double a; // propensity function
    double dt; // waiting time
    bool birth_type = false;

    // ensures finding the next event (i.e. lowest waiting time) for all possible options
    bool operator<(const State &other) {
      return dt > other.dt; // maybe somewhat obfuscated way of reversing std::priority_queue?
    }

    void update(TRng &rng) {
      double r = std::uniform_real_distribution<double>()(rng);
      P = log(1.0/r);
      dt = (P - T)/a;
    }

    void foo() {
      if (birth_type) {
        // implement
      } else {
        --(*N);
      }
    }

    void recalculate_propensity() {
      // I wish that invoke was implemented :(
      // a = std::invoke(*type, rate) * (*N);
      // a = ((type->(*rate))()) * (*N);
      a = (*N) * CALL_MEMBER_FN(*type, rate)();
    }


  };


  struct StateCompare {
    bool operator()(const State &a, const State &b) {
      return a.dt > b.dt;
    }
  };


  NextLB() {
    std::random_device rd;
    rng.seed(rd());
  }


  void add_cell(TCell c) {
    auto p = std::make_shared<TCell>(c);
    auto n = std::make_shared<size_t>(1);
    State state_b;
    state_b.type = p;
    state_b.N = n;
    state_b.rate = &TCell::get_birth_rate;
    state_b.birth_type = true;
    state_b.recalculate_propensity();
    state_b.update(rng);
    State state_d;
    state_d.type = p;
    state_d.N = n;
    state_d.rate = &TCell::get_death_rate;
    state_d.recalculate_propensity();
    state_d.update(rng);
    State state_id;
    state_id.type = p;
    state_id.N = n;
    state_id.rate = &TCell::get_interaction_death_rate;
    state_id.recalculate_propensity();
    state_id.update(rng);
    events.push_back(state_b);
    events.push_back(state_d);
    events.push_back(state_id);
  }


  void simulate(double duration) {
    double stop_time = time + duration;

    std::uniform_real_distribution<double> urd;

    // initial system setup
    for (auto it = events.begin(); it != events.end(); ++it) {
      it->recalculate_propensity();
      it->update(rng);
    }
    // induce heap ordering for faster finding of next event.
    std::make_heap(events.begin(), events.end(), StateCompare());

    // for (auto s: events) {
    //   std::cout << s.dt << std::endl;
    // }

    std::cout << "time\tsize\ttmin\ttmax\tcavg" << std::endl;

    while (time < stop_time) {
      // auto t = events.front().dt;
      // std::pop_heap(events.begin(), events.end(), StateCompare());
      // events.pop_back();
      // std::cout << t << std::endl;
      // time += t;
      auto &e = events.front();
      double d = e.dt;
      time += d;
      if (e.birth_type) {
        for (int i = 0; i < 2; ++i) {
          auto c = e.type->get_mutant(rng);
          add_cell(c);
        }
      }
      // Cell always dies
      *(e.N) -= 1;
      // Get rid of non-extant events
      // this is a terrible implementation btw
      for (auto it = events.begin(); it != events.end(); ++it) {
        if (it->N <= 0) {
          it = events.erase(it);
          --it;
        }
      }
      for (auto &e: events) {
        e.T += d*e.a;
      }
      for (auto &e: events) {
        e.recalculate_propensity();
      }
      // since we probably ruined it by removing and adding stuff
      std::make_heap(events.begin(), events.end(), StateCompare());

      // ### PRINTING ### //
      auto mm = std::minmax_element(events.begin(), events.end()
                                    , [&](auto a, auto b) {
                                      return a.type->get_discrete() < b.type->get_discrete();
                                    });
      auto avg_cont = std::accumulate(events.begin(), events.end(), 0.0
                                      , [&](auto a, auto b) {
                                        return a + b.type->get_continuous();
                                      }) / static_cast<double>(events.size());
      std::cout << time << '\t'
                << events.size() / 3 << '\t'
                << mm.first->type->get_discrete() << '\t'
                << (mm.second == events.end() ? events.begin()->type->get_discrete() : mm.second->type->get_discrete()) << '\t'
                << avg_cont << '\n';
    }
  }


private:
  TRng rng;
  double time = 0.0;
  std::vector<State> events;

};


// template <typename TCell>
// class LBProcess {
// public:
//   LBProcess() {
//     std::random_device rd;
//     rng.seed(rd());
//   }


//   void add_cell(TCell c) {
//     cells.push_back(c);
//   }


//   // template <typename T = TCell, typename std::enable_if_t<T::const_birth_rate::value>::value>
//   void init_birth_rate() {
//     assert(cells.size() > 0);
//     birth_rate = cells.front().get_birth_rate();
//   }
//   template <typename T = TCell, typename std::enable_if_t<T::const_birth_rate::value>::value>
//   double total_birth_rate() {
//     return birth_rate * cells.size();
//   }
//   template <typename T = TCell, typename std::enable_if_t<!T::const_birth_rate::value>::value>
//   double total_birth_rate() {
//     return std::accumulate(cells.begin(), cells.end(), 0.0
//                            , [&](auto a, auto b) {
//                              return a + b.get_birth_rate();
//                            });
//   }


//   // template <typename T = TCell, typename std::enable_if_t<T::const_death_rate::value>::value>
//   void init_death_rate() {
//     assert(cells.size() > 0);
//     death_rate = cells.front().get_death_rate();
//   }
//   template <typename T = TCell, typename std::enable_if_t<T::const_death_rate::value>::value>
//   double total_death_rate() {
//     return death_rate * cells.size();
//   }
//   template <typename T = TCell, typename std::enable_if_t<!T::const_death_rate::value>::value>
//   double total_death_rate() {
//     return std::accumulate(cells.begin(), cells.end(), 0.0
//                            , [&](auto a, auto b) {
//                              return a + b.get_death_rate();
//                            });
//   }

//   // template <typename T = TCell, typename std::enable_if_t<T::const_interaction_death_rate::value>::value>
//   void init_interaction_death_rate() {
//     assert(cells.size() > 0);
//     interaction_death_rate = cells.front().get_interaction_death_rate();
//   }
//   template <typename T = TCell, typename std::enable_if_t<T::const_interaction_death_rate::value>::value>
//   double total_interaction_death_rate() {
//     return interaction_death_rate * cells.size();
//   }
//   template <typename T = TCell, typename std::enable_if_t<!T::const_interaction_death_rate::value>::value>
//   double total_interaction_death_rate() {
//     return std::accumulate(cells.begin(), cells.end(), 0.0
//                            , [&](auto a, auto b) {
//                              return a + b.get_interaction_death_rate();
//                            });
//   }




//   void simulate(double dt) {
//     init_birth_rate();
//     init_death_rate();
//     init_interaction_death_rate();

//     std::cout << "time\tsize\ttmin\ttmax\tcavg" << std::endl;
//     double end_time = time + dt;
//     while (time < end_time) {
//       std::vector<double> event_rates;
//       event_rates.reserve(cells.size());
//       for (auto &c: cells) event_rates.push_back(c.get_event_rate(cells.size()));
//       double total_event_rate = std::accumulate(event_rates.begin(), event_rates.end(), 0.0);
//       time += std::exponential_distribution<double>(total_event_rate)(rng);
//       int cell_id = std::discrete_distribution<int>(event_rates.begin(), event_rates.end())(rng);
//       typename TCell::event_type event = cells[cell_id].select_event(rng, cells.size());
//       if (event == TCell::event_type::BIRTH) {
//         TCell new_cell(cells[cell_id]);
//         new_cell.mutate(rng);
//         cells.push_back(new_cell);
//       } else if (event == TCell::event_type::DEATH) {
//         cells.erase(cells.begin() + cell_id);
//       }
//       auto mm = std::minmax_element(cells.begin(), cells.end()
//                             , [&](auto a, auto b) {
//                               return a.get_discrete() < b.get_discrete();
//                             });
//       auto avg_cont = std::accumulate(cells.begin(), cells.end(), 0.0
//                                       , [&](auto a, auto b) {
//                                         return a + b.get_continuous();
//                                       }) / static_cast<double>(cells.size());
//       std::cout << time << '\t'
//                 << cells.size() << '\t'
//                 << mm.first->get_discrete() << '\t'
//                 << (mm.second == cells.end() ? cells.begin()->get_discrete() : mm.second->get_discrete()) << '\t'
//                 << avg_cont << '\n';
//     }
//   }

// private:
//   double time = 0.0;
//   std::vector<TCell> cells;

//   std::mt19937 rng;

//   // used if cells are known to have constant rates
//   double birth_rate;
//   double death_rate;
//   double interaction_death_rate;

// };


#endif
