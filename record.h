#ifndef __RECORD_H__
#define __RECORD_H__


#include <algorithm>
#include <iostream>
#include <list>
#include <memory>
#include <utility>
#include <vector>


// Pure virtual base class
class Record {
public:
  virtual void insert(size_t id, size_t type) = 0;
  virtual void insert(double t, size_t id[2], size_t type[2], size_t parent_id) = 0;
};


// Forest based record
struct Node {
  Node(size_t id, size_t type, double birthtime=0.0)
    : id(id)
    , type(type)
    , birthtime(birthtime) { }

  size_t id;
  size_t type;
  double life;
  double birthtime;
  std::shared_ptr<Node> left = nullptr;
  std::shared_ptr<Node> right = nullptr;
};


class RecordForest : public Record {
private:
  void add_leaf(std::shared_ptr<Node> n) {
    leaves.push_back(n);
    leaves.sort([](auto a, auto b) {
        return a->id < b->id;
      });
  }

public:
  virtual void insert(size_t id, size_t type) {
    auto r = std::make_shared<Node>(id, type);
    r->life = 0.0;
    roots.push_back(r);
    add_leaf(r);
  }


  virtual void insert(double t, size_t id[2], size_t type[2], size_t parent_id) {
    auto it_parent = std::lower_bound(leaves.begin(), leaves.end(), parent_id, [](auto n, size_t parent_id) {
        return parent_id == n->id;
      });
    auto child1 = std::make_shared<Node>(id[0], type[0], t);
    auto child2 = std::make_shared<Node>(id[1], type[1], t);
    auto parent = *it_parent;
    parent->left = child1;
    parent->right = child2;
    parent->life = t - parent->birthtime;
    leaves.erase(it_parent);
    add_leaf(child1);
    add_leaf(child2);
  }


  friend std::ostream &operator<<(std::ostream &out, const RecordForest &record);


private:
  std::vector<std::shared_ptr<Node>> roots;
  // Maintain sorted list of leaves for more quickly adding new events
  std::list<std::shared_ptr<Node>> leaves;
};

std::ostream &operator<<(std::ostream &out, const std::shared_ptr<Node> n) {
  if (n->left == nullptr && n->right == nullptr) {
    out << n->id << ':' << n->life;
  } else {
    out << '(' << n->left << ',' << n->right << ')' << n->id << ':' << n->life;
  }
  return out;
}
std::ostream &operator<<(std::ostream &out, const RecordForest &record) {
  for (auto r: record.roots) {
    out << r << ';';
  }
  return out;
}


#endif
