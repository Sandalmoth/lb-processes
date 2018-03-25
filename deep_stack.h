#ifndef __DEEP_STACK__
#define __DEEP_STACK__


#include <stack>


// Infinite stack for getting new id's or similar in a convenient way.
// Will provide id's starting from 0 in an increasing fashion with pop
// but if some value is returned with push, that will be returned with pop
// before it generates new larger numbers.


class DeepStack {
public:
  size_t pop() {
    if (returned_values.size() == 0) {
      // No values have been returned, so generate a new one
      return max_popped++;
    } else {
      // Return an unused old value
      size_t a = returned_values.top();
      returned_values.pop();
      return a;
    }
  }

  void push(size_t a) {
    returned_values.push(a);
  }


private:
  std::stack<size_t> returned_values;
  size_t max_popped = 0;

};


#endif
