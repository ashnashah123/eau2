#pragma once
//lang:CwC
#include "object.h"
#include "array.h"

// Represents a Map object of Objects to Objects
// does not owns the values and does not copy on puts (both key and value)
// * @ Implementation authored by: shah.ash@husky.neu.edu | peters.ci@husky.neu.edu
class Map : public Object {
    public:
        Array keys_;
        Array values_;

        Map() : keys_(1), values_(1) {}

        /* Inherited from Object, generates a hash for a Map */
        size_t hash() {
        	return keys_.hash() + values_.hash();
        }
        /* Inherited from Object, checks equality between an Map and an Object */
        bool equals(Object* const obj) {
        	if (obj == nullptr) return false;
        	Map* map = dynamic_cast<Map*>(obj);
        	if (map == nullptr) return false;
        	return keys_.equals(&map->keys_) && values_.equals(&map->values_);
        }
        // associates the value with the key in this map
        void put(Object *key, Object *value) {
        	if (contains(key)) {
        		size_t index = keys_.index_of(key);
        		values_.set(value, index);
        	}
        	else {
	        	keys_.push(key);
	        	values_.push(value);
        	}
        }
        // returns the value associated this key in the map
        Object* get(Object* key) {
			if (!keys_.contains(key)) {
                return nullptr;
            }
        	size_t index = keys_.index_of(key);
        	Object* val = values_.get(index);
        	return val;
        }
        // checks if the map contains a value with this key
        bool contains(Object* key) {
        	return keys_.index_of(key) < keys_.length();
        }
        // returns the number of key value pairs in this map
        size_t size() {
            return keys_.length();
        }

		void delete_all() {
			// keys_.delete_all();
			values_.delete_all();
		}

        // clears the map
        void clear() {
        	keys_.resize(0);
        	values_.resize(0);
        }

        // returns an array of copied keys in map, not sorted
        Object** get_keys() {
        	Object** new_keys_ = new Object*[size()];
        	for (size_t i = 0; i < size(); i++) {
        		new_keys_[i] = keys_.get(i);
        	}
        	return new_keys_;
        }
};