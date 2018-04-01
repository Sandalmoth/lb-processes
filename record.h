#ifndef __RECORD_H__
#define __RECORD_H__


#include <memory>
#include <utility>
#include <vector>


// Pure virtual base class
class Record {
public:
  virtual void insert(double t, size_t id, size_t type) = 0;
};


// Forest based record
struct Node {
  double life;
  std::shared_ptr<Node> left = nullptr;
  std::shared_ptr<Node> right = nullptr;
};


class RecordForest : public Record {
public:
  virtual void insert(double t, size_t id, size_t type) {
  }


private:
  std::vector<std::shared_ptr<Node>> roots;
};


#endif
