/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; } // not yet to handle size() == 0
   void insert(const Data& d) 
   { 
      _data.push_back(d);
      // cout << _data[size() - 1] << endl;
      up_heapify(size() - 1);
   }
   void delMin() 
   { 
      delData(0); 
   }
   void delData(size_t i) 
   { 
      // cout << "heap delelte" << endl;
      size_t s = size();
      if(s == 1) // only one element
      {
         _data.pop_back();
      }
      else
      {
         // replace _data[i] with the last element
         // and the do down_heapify to find the right place
         // cout << "in delete" << endl;
         _data[i] = _data[s - 1];
         _data.pop_back();
         down_heapify(i, s - 1);
      }
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;

   void swap(Data& d1, Data& d2)
   {
      Data tmp;
      tmp = d1;
      d1 = d2;
      d2 = tmp;
   }

   void up_heapify(size_t v)
   {
      if(v == 0) return; // the min element
      else if(v % 2 == 1) // v is odd, left child
      {
         if(_data[v] < _data[v / 2])
         {
            swap(_data[v], _data[v / 2]);
            up_heapify(v / 2);
         }
      }
      else // v is even, right child
      {
         if(_data[v] < _data[v / 2 - 1])
         {
            swap(_data[v], _data[v / 2 - 1]);
            up_heapify(v / 2 - 1);
         }
      }
   }

   void down_heapify(size_t v, size_t s) // s is size of the heap
   {
      if(s <= v * 2) return; // two child are all null
      else if(s <= v * 2 + 1) // only have left child
      {
         if(_data[2 * v + 1] < _data[v])
         {
            swap(_data[v], _data[2 * v + 1]);
            down_heapify(2 * v + 1, s);
         }
      }
      else // has two childs
      {
         if(_data[2 * v + 1] < _data[2 * v + 2]) // choose the smallest child
         {
            if(_data[2 * v + 1] < _data[v]) 
            {
               swap(_data[v], _data[2 * v + 1]);
               down_heapify(2 * v + 1, s);
            }
         }
         else
         {
            if(_data[2 * v + 2] < _data[v])
            {
               swap(_data[v], _data[2 * v + 2]);
               down_heapify(2 * v + 2, s);
            }
         }
      }
      
   }

};

#endif // MY_MIN_HEAP_H
