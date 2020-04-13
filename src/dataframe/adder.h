#pragma once

#include "../utils/string.h"
#include "./reader.h"
#include "../utils/si_map.h"

/****************************************************************************/
class Adder : public Reader {
public:
  SIMap& map_;  // String to Num map;  Num holds an int
 
  Adder(SIMap& map) : map_(map)  {}
 
  bool visit(Row& r) override {
    String* word = r.get_string(0);
    std::cout << "......................WORD IN VISIT ADDER: " << word->cstr_ << " ....................\n";
    assert(word != nullptr);
    Num* num = map_.contains(*word) ? map_.get(*word) : new Num();
    assert(num != nullptr);
    num->v++;
    std::cout << "......................NUM IN VISIT ADDER: " << num->get_value() << " ....................\n";
    map_.set(*word, num);
    return false;
  }
};