//lang::CwC

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include "object.h"
#include "../store/key.h"

/**
 * @brief      This class describes an array of objects.
 * @ Implementation authored by: shah.ash@husky.neu.edu | peters.ci@husky.neu.edu
 */
class Array : public Object {
    public:
        Object** array_;
        size_t capacity_; // the capacity of the array
        size_t size__ = 0;

        Array(size_t length) {
            capacity_ = length;
            array_ = new Object*[length];
            for(size_t i = 0; i < capacity_; i++) {
                array_[i] = nullptr;
            }
        }

        ~Array() {
            delete[] array_;
        }

        size_t hash() {
            size_t result = 0;
            for (size_t i = 0; i < size__; i++) {
                Object* element = get(i);
                if (element == nullptr) {
                    result = result + 1;
                } else {
                    result += element->hash();
                }
            }
            return result;
        }

        bool equals(Object* other) {
            Array* l = dynamic_cast<Array*>(other);
            if (l == nullptr) return false;

            if (l->length() != size__) {
                return false;
            }

            bool result = true;
            for (int i = 0; i < l->size__; i++) {
                if(array_[i] == nullptr && l->array_[i] == nullptr) {
                    result = result && true;
                }
                else if (array_[i] == nullptr || l->array_[i] == nullptr) {
                    result = result && false;
                }
                else {
                    Object* this_object = get(i);
                    Object* other_object = l->get(i);
                    result = result && this_object->equals(other_object);
                }
            }
            return result;
        }

        size_t length() { return size__; }

        size_t index_of(Object* obj) {
            // std::cout << "NUM ELEMENTS IN ARRAY INDEX_OF: " << size__ << "\n";
            
            for (size_t i = 0; i < size__; i++) {
                // std::cout << "in index_of i: | " << i << " | get(i) = " << dynamic_cast<Key*>(get(i))->get_name()->cstr_ << "\n";
                // std::cout << "in index_of i: | " << i << " | obj = " << dynamic_cast<Key*>(obj)->get_name()->cstr_ << "\n";
                if (get(i)->equals(obj)) {
                    return i;
                }
            }
            return capacity_;
        }

        void set(Object* obj, size_t idx) {
            assert(idx < size__);
            array_[idx] = obj;
        }

        Object* get(size_t idx) {
            assert(idx < size__);
            return array_[idx];
        }

        bool contains(Object* obj) {
            return index_of(obj) < size__;
        }

        void resize(size_t target) {
            if (target < capacity_) return;
            capacity_ = capacity_ * 2 + target;
            Object** temp = new Object*[capacity_];
            
            for (int i = 0; i < size__; i++) {
                temp[i] = array_[i];
            }
            delete[] array_;
            array_ = temp;
        }

        void push(Object* obj) {
            resize(size__ + 1);
            array_[size__] = obj;
            size__++;
        }

        void delete_all() {
            for (size_t i = 0; i < size__; i++) {
                delete array_[i];
            }
        }
};