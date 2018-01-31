#ifndef __LB_PROCESS_H__
#define __LB_PROCESS_H__


#include <vector>
#include <numeric>
#include <random>
#include <iostream>
#include <algorithm>
#include <type_traits>
#include <cassert>


template <typename TCell>
class LBProcess {
public:
  LBProcess() {
    std::random_device rd;
    rng.seed(rd());
  }


  void add_cell(TCell c) {
    cells.push_back(c);
  }


  // template <typename T = TCell, typename std::enable_if_t<T::const_birth_rate::value>::value>
  void init_birth_rate() {
    assert(cells.size() > 0);
    birth_rate = cells.front().get_birth_rate();
  }
  template <typename T = TCell, typename std::enable_if_t<T::const_birth_rate::value>::value>
  double total_birth_rate() {
    return birth_rate * cells.size();
  }
  template <typename T = TCell, typename std::enable_if_t<!T::const_birth_rate::value>::value>
  double total_birth_rate() {
    return std::accumulate(cells.begin(), cells.end(), 0.0
                           , [&](auto a, auto b) {
                             return a + b.get_birth_rate();
                           });
  }


  // template <typename T = TCell, typename std::enable_if_t<T::const_death_rate::value>::value>
  void init_death_rate() {
    assert(cells.size() > 0);
    death_rate = cells.front().get_death_rate();
  }
  template <typename T = TCell, typename std::enable_if_t<T::const_death_rate::value>::value>
  double total_death_rate() {
    return death_rate * cells.size();
  }
  template <typename T = TCell, typename std::enable_if_t<!T::const_death_rate::value>::value>
  double total_death_rate() {
    return std::accumulate(cells.begin(), cells.end(), 0.0
                           , [&](auto a, auto b) {
                             return a + b.get_death_rate();
                           });
  }

  // template <typename T = TCell, typename std::enable_if_t<T::const_interaction_death_rate::value>::value>
  void init_interaction_death_rate() {
    assert(cells.size() > 0);
    interaction_death_rate = cells.front().get_interaction_death_rate();
  }
  template <typename T = TCell, typename std::enable_if_t<T::const_interaction_death_rate::value>::value>
  double total_interaction_death_rate() {
    return interaction_death_rate * cells.size();
  }
  template <typename T = TCell, typename std::enable_if_t<!T::const_interaction_death_rate::value>::value>
  double total_interaction_death_rate() {
    return std::accumulate(cells.begin(), cells.end(), 0.0
                           , [&](auto a, auto b) {
                             return a + b.get_interaction_death_rate();
                           });
  }




  void simulate(double dt) {
    init_birth_rate();
    init_death_rate();
    init_interaction_death_rate();

    std::cout << "time\tsize\ttmin\ttmax\tcavg" << std::endl;
    double end_time = time + dt;
    while (time < end_time) {
      std::vector<double> event_rates;
      event_rates.reserve(cells.size());
      for (auto &c: cells) event_rates.push_back(c.get_event_rate(cells.size()));
      double total_event_rate = std::accumulate(event_rates.begin(), event_rates.end(), 0.0);
      time += std::exponential_distribution<double>(total_event_rate)(rng);
      int cell_id = std::discrete_distribution<int>(event_rates.begin(), event_rates.end())(rng);
      typename TCell::event_type event = cells[cell_id].select_event(rng, cells.size());
      if (event == TCell::event_type::BIRTH) {
        TCell new_cell(cells[cell_id]);
        new_cell.mutate(rng);
        cells.push_back(new_cell);
      } else if (event == TCell::event_type::DEATH) {
        cells.erase(cells.begin() + cell_id);
      }
      auto mm = std::minmax_element(cells.begin(), cells.end()
                            , [&](auto a, auto b) {
                              return a.get_discrete() < b.get_discrete();
                            });
      auto avg_cont = std::accumulate(cells.begin(), cells.end(), 0.0
                                      , [&](auto a, auto b) {
                                        return a + b.get_continuous();
                                      }) / static_cast<double>(cells.size());
      std::cout << time << '\t'
                << cells.size() << '\t'
                << mm.first->get_discrete() << '\t'
                << (mm.second == cells.end() ? cells.begin()->get_discrete() : mm.second->get_discrete()) << '\t'
                << avg_cont << '\n';
    }
  }

private:
  double time = 0.0;
  std::vector<TCell> cells;

  std::mt19937 rng;

  // used if cells are known to have constant rates
  double birth_rate;
  double death_rate;
  double interaction_death_rate;

};


#endif
