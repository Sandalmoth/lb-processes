#ifndef __LB_GILLESPIE_H__
#define __LB_GILLESPIE_H__


#include <iostream>
#include <numeric>
#include <random>


template <typename TCell, typename TRng=std::mt19937>
class LB {
public:
  LB() {
    std::random_device rd;
    rng.seed(rd());
  }


  void add_cell(const TCell &cell) {
    cells.emplace_back(cell);
  }


  void simulate(double interval) {
    double t_end = t + interval;


    std::cout.precision(3);
    std::cout << "time\tsize\n";
    std::cout << t << '\t' << cells.size() << '\n';
    double record_interval = 0.1;
    double next_record = t + record_interval;


    std::vector<double> rates; // No need to keep reallocating

    while (t < t_end) {
      // Fetch birth, mutation, and death rates for all cells
      rates.resize(cells.size() * 3);
      for (size_t i = 0; i < cells.size(); ++i) {
        // Store rates in order (birth without mutation, birth with mutation, death)
        // With birth interaction overflowing into death rate if it is bigger than the birth rate
        rates[3*i]      = cells[i].get_birth_rate(t, cells.begin(), cells.end())
                        - (cells.size() - 1) * cells[i].get_birth_interaction(t, cells.begin(), cells.end());
        rates[3*i + 2]  = -std::min(rates[3*i], 0.0);
        rates[3*i]      = std::max(rates[3*i], 0.0);
        rates[3*i + 1]  = rates[3*i] * cells[i].get_mutation_rate(t, cells.begin(), cells.end());
        rates[3*i]     -= rates[3*i + 1];
        rates[3*i + 2] += cells[i].get_death_rate(t, cells.begin(), cells.end())
                        + (cells.size() - 1) * cells[i].get_death_interaction(t, cells.begin(), cells.end());
      }

      // Take timestep dependent on rate of all events
      // double event_rate = std::reduce(rates.begin(), rates.end(), 0.0);
      double event_rate = std::accumulate(rates.begin(), rates.end(), 0.0);
      double dt = std::exponential_distribution<double>(event_rate)(rng);
      t += dt;

      // Select an event to perform based on their rates
      int event = std::discrete_distribution<int>(rates.begin(), rates.end())(rng);
      int event_type = event % 3;
      int event_cell = event / 3;
      switch (event_type) {
      case 0: // birth without mutation
        cells.emplace_back(cells[event_cell]);
        break;
      case 1: // birth with mutation
        cells.emplace_back(cells[event_cell]);
        cells.back().mutate(rng);
        break;
      case 2: //death
        std::swap(cells[event_cell], cells.back());
        cells.pop_back();
        break;
      }

      if (t > next_record) {
        std::cout << t << '\t' << cells.size() << '\n';
        do {
          next_record += record_interval;
        } while (next_record < t);
      }
    }
  }


private:
  TRng rng;
  std::vector<TCell> cells;

  double t = 0;

};


#endif
