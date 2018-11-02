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
  virtual void terminate(double t, size_t id) = 0;
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
    auto it_parent = std::lower_bound(leaves.begin(), leaves.end(), parent_id
                                      , [](auto n, size_t parent_id) {
                                        return n->id < parent_id;
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


  virtual void terminate(double t, size_t id) {
    auto it_parent = std::lower_bound(leaves.begin(), leaves.end(), id
                                      , [](auto n, size_t parent_id) {
                                        return n->id < parent_id;
                                      });
    auto parent = *it_parent;
    parent->life = t - parent->birthtime;
    leaves.erase(it_parent);
  }


  friend std::ostream &operator<<(std::ostream &out, const RecordForest &record);


private:
  std::vector<std::shared_ptr<Node>> roots;
  // Maintain sorted list of leaves for more quickly adding new events
  std::list<std::shared_ptr<Node>> leaves;
};

std::ostream &operator<<(std::ostream &out, const std::shared_ptr<Node> n) {
  if (n->left == nullptr && n->right == nullptr) {
    out << static_cast<char>('A' + n->type) << ':' << n->life;
  } else {
    out << '(' << n->left << ',' << n->right << ')' << static_cast<char>('A' + n->type) << ':' << n->life;
  }
  return out;
}
std::ostream &operator<<(std::ostream &out, const RecordForest &record) {
  for (auto r: record.roots) {
    out << r << ';';
  }
  return out;
}


// List based record
struct Element {
  double t;
  size_t counts[2];
};

class RecordList : public Record {
public:
  RecordList(double resolution)
    : resolution(resolution) {
    time = 0.0;
    counts[0] = counts[1] = 0;
    types.resize(1);
    next_record = 0.0;
  }


  virtual void insert(size_t id, size_t type) {
    ++counts[type];
    if (types.size() < id)
      types.resize(types.size() * 2);
    types[id] = type;
  }


  virtual void insert(double t, size_t id[2], size_t type[2], size_t parent_id) {
    if (t > next_record) {
      trace.emplace_back(Element{time, {counts[0], counts[1]}});
      while (next_record < t)
        next_record += resolution;
    }
    ++counts[type[0]];
    ++counts[type[1]];
    --counts[types[parent_id]];
    time = t;
    if (types.size() < std::max(id[0], id[1]))
      types.resize(types.size() * 2);
    types[id[0]] = type[0];
    types[id[1]] = type[1];
  }


  virtual void terminate(double t, size_t id) {
    if (t > next_record) {
      trace.emplace_back(Element{time, {counts[0], counts[1]}});
      while (next_record < t)
        next_record += resolution;
    }
    --counts[types[id]];
  }


  friend std::ostream &operator<<(std::ostream &out, const RecordList &record);


private:
  double resolution;
  size_t counts[2];
  double time;
  double next_record;
  std::vector<Element> trace;
  std::vector<size_t> types;
};

std::ostream &operator<<(std::ostream &out, const RecordList &record) {
  std::cout << "time\tn1\tn2\n";
  for (auto x: record.trace) {
    std::cout << x.t << '\t' << x.counts[0] << '\t' << x.counts[1] << '\n';
  }
  return out;
}



#endif
