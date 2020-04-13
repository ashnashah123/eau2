#pragma once

#include "../utils/si_map.h"

/***************************************************************************/
class Summer : public Writer {
public:
  SIMap& map_;
  size_t i = 0;
  size_t j = 0;
  size_t seen = 0;
 
  Summer(SIMap& map) : map_(map) {
      if (!k()) {
          next();
      }
  }
 
  void next() {
      assert(!done());
      if (i == map_.capacity_ ) return;
      j++;
      ++seen;
      if ( j >= map_.items_[i].keys_.length() ) { 
          ++i; 
          j = 0; 
          while( i < map_.capacity_ && map_.items_[i].keys_.length() == 0 ) {
            i++;
          }
      }
  }
 
  String* k() {
      if (i==map_.capacity_ || j == map_.items_[i].keys_.length()) {
          return nullptr;
      }
      return (String*) (map_.items_[i].keys_.get(j));
  }
 
  size_t v() {
      if (i == map_.capacity_ || j == map_.items_[i].keys_.length()) {
          assert(false); return 0;
      }
      return ((Num*)(map_.items_[i].vals_.get(j)))->v;
  }
 
  void visit(Row& r) {
      String & key = *k();
      size_t value = v();
      r.set(0, &key);
      r.set(1, (int) value);
      next();
  }
 
  bool done() {
        return seen == map_.size(); 
    }
};
 