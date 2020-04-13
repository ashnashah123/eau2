#pragma once

#include "../store/key.h"
#include "./string.h"
#include "./object.h"

class KeyBuff : public Object {                                                  
  public:                                                                        
  Key* orig_; // external                                                        
  StrBuff buf_;                                                                  
                                                                                 
  KeyBuff(Key* orig) : orig_(orig), buf_(orig->name_) {}                               
                                                                                 
  KeyBuff& c(String &s) { buf_.c(s); return *this;  }                            
  KeyBuff& c(size_t v) { buf_.c(v); return *this; }                              
  KeyBuff& c(const char* v) { buf_.c(v); return *this; }                         
                                                                                 
  Key* get() {                                                                   
    String* s = buf_.get(); // val does NOT get consumed          
    // buf_.c(*orig_->get_name());                                                      
    Key* k = new Key(new String(s->steal()), orig_->home());                                 
    delete s;                                                                    
    return k;                                                                    
  }                                                                              
};                                   
       