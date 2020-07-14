/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   // set _isSorted = true at first
   Array() : _data(0), _size(0), _capacity(0), _isSorted(1){} 
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*_node); }
      T& operator * () { return (*_node); }
      iterator& operator ++ () { _node += 1; return (*this); }
      iterator operator ++ (int) { iterator tmp(_node); _node += 1; return tmp; }
      iterator& operator -- () { _node -= 1; return (*this); } // not sure whether to handle out of index case or not
      iterator operator -- (int) { iterator tmp(_node); _node -= 1; return tmp; }

      iterator operator + (int i) const { return _node + i; } // not sure
      iterator& operator += (int i) { _node += i; return (*this); }

      iterator& operator = (const iterator& i) { _node = i._node; return (*this); }

      // not sure what it really means
      bool operator != (const iterator& i) const { return _node != i._node; }
      bool operator == (const iterator& i) const { return _node == i._node; }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const // not sure
   {
      if(!empty()) 
      {
         iterator b(&_data[0]);
         return b;
      } 
      else 
         return 0;

   }           
   iterator end() const // not sure
   { 
      if(!empty())
      {
         iterator e(&_data[_size - 1]);
         return ++e;
      } 
      else 
         return 0;
   } 
   bool empty() const { return (!_size); }            // not sure
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return _data[i]; }
   const T& operator [] (size_t i) const { return _data[i]; }

   void push_back(const T& x) 
   { 
      // cout << "push_back" << endl;
      if(_capacity == 0)
      {
         // cout << "resize: 0" << endl;
         reserve(1);
         _capacity = 1;
      }
      else if(_size == _capacity) // the array is full
      {
         resize(_capacity * 2);
         // cout << "resize: " << _capacity << endl;
      }
      // cout << "add_data" << endl;
      // cout << "capacity: " << _capacity << endl;
      _data[_size] = x; 
      // cout << _data[_size] << endl;
      _size += 1;
      // cout << "size: " << _size << endl;
   }

   void pop_front() 
   { 
      if(empty())
      {
         // cerr << "No element in the array" << endl;
         return ;
      }
      
      // for(size_t i = 0; i < _size - 1; ++i)
      // {
      //    _data[i] = _data[i + 1];
      // }

      _data[0] = _data[_size - 1];
      
      _size -= 1;
   }
   void pop_back() 
   { 
      if(empty())
      {
         // cerr << "No element in the array" << endl;       
         return ;  
      }
      // _data[_size - 1] = 0; // not sure if to clear it
      _size -= 1;
   }

   bool erase(iterator pos) 
   {
      if(empty())
      {
         // cerr << "No element in the array" << endl;       
         return false;  
      }

      // pos has T* _node
      if(pos._node == 0) return false;
      else
      {
         *pos = _data[_size - 1];
         _size -= 1;
         return true;
      }

   }
   bool erase(const T& x) 
   { 
      if(empty())
      {
         // cerr << "No element in the array" << endl;       
         return false;  
      }

      size_t index = 0;
      bool found = false;

      for(size_t i = 0; i < _size; ++i)
      {
         if(_data[i] == x)
         {
            index = i;
            found = true;
            break;
         }
      }

      if(found)
      {
         // for(size_t i = index; i < _size - 1; ++i)
         // {
         //    _data[i] = _data[i + 1];
         // }
         _data[index] = _data[_size - 1];
         _size -= 1;
      }

      return found; 
   }

   iterator find(const T& x) 
   { 
      for(size_t i = 0; i < _size; ++i)
      {
         if(_data[i] == x) 
         {
            iterator f(&_data[i]);
            return f;
         }
         // return &_data[i];
      }
      return end();
      // return end(); 
   }

   void clear() 
   {
      _size = 0;
   }

   // [Optional TODO] Feel free to change, but DO NOT change ::sort()
   void sort() const { if (!empty()) ::sort(_data, _data + _size); }

   // Nice to have, but not required in this homework...
   void reserve(size_t n) 
   { 
      assert(n >= _capacity); // we assume that we resize the array to a bigger one
      _data = new T[n];      
   }
   void resize(size_t n) 
   {
      assert(n >= _capacity); // we assume that we resize the array to a bigger one

      T* new_array = new T[n];
      for(size_t i = 0; i < _size; ++i)
      {
         new_array[i] = _data[i];
      }

      delete [] _data;
      _capacity = n;
      _data = new_array;  
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
