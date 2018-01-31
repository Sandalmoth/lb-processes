#ifndef __REACTCELL_H__
#define __REACTCELL_H__


#include <cstdlib>
#include <cmath>
#include <iostream>
#include <random>


class Reactcell {
public:
  Reactcell(double p, double d, double c)
    : birth_rate(p)
    , death_rate(d)
    , interaction_death_rate(c) { }


  double get_birth_rate() {
    return birth_rate * (discrete + 1) + continuous;
  }
  double get_death_rate() {
    return death_rate;
  }
  double get_interaction_death_rate() {
    return interaction_death_rate;
  }

  int get_discrete() const { return discrete; }
  double get_continuous() const { return continuous; }

  template <typename TRng>
  Reactcell get_mutant(TRng &rng) {
    Reactcell c(*this);
    c.change_params(
                    (std::uniform_real_distribution<double>()(rng) < 0.00001 ? 1 : 0)
                    , std::normal_distribution<double>(0.0, 0.01)(rng)
                    );
    return c;
  }

  void change_params(double c, int d) {
    continuous += c;
    discrete += d;
  }

private:
  double birth_rate;
  double death_rate;
  double interaction_death_rate;

  double continuous = 0.0;
  int discrete = 0;
};

// private:
//   double birth_rate;
//   double death_rate;
//   double interaction_death_rate;

// public:
//   enum class event_type {
//     BIRTH,
//     DEATH
//   };

//   // These methods cannot be right, because the question we want to ask is not whether the base rates are constant.
//   // It is whether the get_x functions always return the same value.
//   // Maybe it's better if the user simply tags the functions somehow?
//   // constexpr static auto const_birth_rate = std::is_const<decltype(birth_rate)>();
//   // constexpr static auto const_death_rate = std::is_const<decltype(death_rate)>();
//   // constexpr static auto const_interaction_death_rate = std::is_const<decltype(interaction_death_rate)>();
//   // typename std::is_const<decltype(birth_rate)> const_birth_rate;
//   // typename std::is_const<decltype(death_rate)> const_death_rate;
//   // typename std::is_const<decltype(interaction_death_rate)> const_interaction_death_rate;
//   // Maybe simply:
//   typedef std::false_type const_birth_rate;
//   typedef std::true_type const_death_rate;
//   typedef std::true_type const_interaction_death_rate;
//   // Since user implements this class anyway, this is probably simpler

//   Reactcell(double p, double d, double c)
//     : birth_rate(p)
//     , death_rate(d)
//     , interaction_death_rate(c) { }


//   void mutate(std::mt19937 & rng) {
//     discrete += (std::uniform_real_distribution<double>()(rng) < 0.00001 ? 1 : 0);
//     continuous += std::normal_distribution<double>(0.0, 0.01)(rng);
//   }
//   double get_event_rate(size_t n) {
//     return get_birth_rate() + get_death_rate() + (n - 1)*get_interaction_death_rate();
//   }
//   event_type select_event(std::mt19937 & rng, size_t n) {
//     return (std::uniform_real_distribution<double>()(rng) < get_birth_rate()/get_event_rate(n) ? event_type::BIRTH : event_type::DEATH);
//   }

//   int get_discrete() const { return discrete; }
//   double get_continuous() const { return continuous; }

//   double get_birth_rate() {
//     // std::cout << birth_rate * (discrete + 1) + continuous << std::endl;
//     return birth_rate * (discrete + 1) + continuous;
//   }
//   double get_death_rate() { return death_rate; }
//   double get_interaction_death_rate() { return interaction_death_rate; }

// private:
//   int discrete = 0;
//   double continuous = 0.0;



#endif
