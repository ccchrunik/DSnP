/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>
#include <algorithm>

using namespace std;

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
      _isSorted = true;
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n = 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      // prefix, like ++i
      iterator& operator ++ () { _node = _node->_next; return (*this); }
      // postfix, like i++
      iterator operator ++ (int) { iterator tmp(_node); _node = _node->_next; return tmp; }
      // prefix, like --i
      iterator& operator -- () { _node = _node->_prev; return (*this); }
      // postfix, like i--
      iterator operator -- (int) { iterator tmp(_node); _node = _node->_prev; return tmp; }

      iterator& operator = (const iterator& i) { _node = i._node; return (*this); }

      bool operator != (const iterator& i) const { return _node != i._node; }
      bool operator == (const iterator& i) const { return _node == i._node; }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const {  return _head; }           // ** not sure if empty to return dummy or not
   iterator end() const { return empty() ? _head : _head->_prev; } // ** not sure if empty to return dummy or not
   bool empty() const { return _head->_next == _head; }
   size_t size() const 
   { 
      if(empty()) return 0;
      size_t count = 0;
      DListNode<T>* tmp = _head;
      while(tmp != _head->_prev) // not equal to dummy node
      {
         tmp = tmp->_next;
         ++count;
      }
      return count; 
   }

   void push_back(const T& x) 
   { 
      if(empty()) // only dummy node
      {
         // _head is dummy now
         DListNode<T>* new_node = new DListNode<T>(x, _head, _head); // (data, prev, next)
         _head->_next = new_node;
         _head->_prev = new_node;
         _head = new_node;
      }
      else // have some node
      {
         DListNode<T>* tail = _head->_prev->_prev;
         DListNode<T>* new_node = new DListNode<T>(x, tail, tail->_next); // (data, prev, next)
         tail->_next->_prev = new_node; // set dummy node prev to new_node
         tail->_next = new_node;
         // not yet been optimized
         _isSorted = false; // if need to resort
      }
   }

   void pop_front() 
   { 
      if(!empty())
      {
         // _head->_prev->_next = _head->_next; 
         // _head->_next->_prev = _head->_prev;
         // DListNode<T>* tmp = _head; // save the node to be deleted
         // _head = _head->_next;
         // delete tmp;
         erase(begin());
      }
      // else
      // {
      //    cerr << "No element in the list" << endl;
      // }
      
   }
   void pop_back() 
   { 
      if(!empty())
      {
         // DListNode<T>* tail = _head->_prev->_prev;
         // tail->_prev->_next = tail->_next; 
         // tail->_next->_prev = tail->_prev;
         // delete tail;
         iterator e = end();
         e._node = e._node->_prev;
         erase(e);
      }
      // else
      // {
      //    cerr << "No element in the list" << endl;
      // }
      
   }

   // return false if nothing to erase
   // not sure that if pos is not in the list
   bool erase(iterator pos) 
   { 
      if(empty()) return false;
      else if(pos._node == _head->_prev) return false; // point to dummy node
      else
      {
         if(pos == _head) _head = _head->_next;
         DListNode<T>* tmp = pos._node;
         tmp->_prev->_next = tmp->_next;
         tmp->_next->_prev = tmp->_prev;
         delete tmp;
         
         return true;
      }
       
   }

   bool erase(const T& x) 
   { 
      if(empty()) return false;
      else
      {
         DListNode<T>* tmp = _head;
         while(tmp->_next != _head) // tmp is not dummy node
         {
            if(tmp->_data == x)
            {
               if(tmp == _head) _head = _head->_next;
               tmp->_prev->_next = tmp->_next;
               tmp->_next->_prev = tmp->_prev;
               delete tmp;
               return true;
            }
            tmp = tmp->_next; 
         }
      }

      return false;
   }

   iterator find(const T& x) 
   {
      if(empty())
      {
         // cerr << "No element in the list" << endl;
         // cout << "empty" << endl;
         return _head; // may have error
      }
      
      iterator tmp(_head);
      while(tmp != _head->_prev) // tmp != dummy node
      {
         if(*tmp == x)
         {
            // cout << "tmp = " << *tmp << endl;
            // cout << "x = " << x << endl;
            return tmp;
         }
         // tmp._node = tmp._node->_next;
         ++tmp;
      }
      
      return _head->_prev;
   }

   void clear() // delete all nodes except for the dummy node
   { 
      if(empty()) return ;
      iterator dummy(_head->_prev);
      iterator tail(_head->_prev->_prev);

      while(tail != dummy) // which means that now only dummy node left
      {
         iterator tmp(tail);
         tail._node = tail._node->_prev;
         erase(tmp);
      }
      _head = dummy._node;
   }  

   void sort() const 
   { 
      if(!empty() && !_isSorted) // not yet been sort
      {
         size_t length = size();
         T v_1, v_2, tmp;
         // T tmp;
         // T* t1, t2;
         // T* tmp;
         DListNode<T>* bubble;
         for(int i = length - 1; i >= 0; --i) // counting how many times to flip
         {
            // iterator bubble(_head);
            bubble = _head;
            // DListNode<T>* n_bubble = bubble->_next;
            // using bubble sort
            for(int j = 0; j < i; ++j)// reach the end of the list
            {
               // v_1 = *bubble;
               // v_2 = *(++bubble);
               // v_1 = bubble->_data;
               // v_2 = bubble->_next->_data;
               T* t1 = &bubble->_data;
               T* t2 = &bubble->_next->_data;
               
               // if(*bubble > *(++bubble))
               // if(v_1 > v_2) // v1 > v2, // bubble now is next
               if(*t1 > *t2)
               {
                  // exchange value
                  // *bubble = v_1;
                  // *(--bubble) = v_2;
                  // bubble->_data = v_2;
                  // bubble->_next/->_data = v_1;
                  tmp = *t1;
                  *t1 = *t2;
                  *t2 = tmp;
                  // ++bubble;
               }
               // ++bubble;
               bubble = bubble->_next;
            }
         }
         // for(; x!= _head->_prev; x = x->next)
         // for(y=x->_next;y != _head->_prev; y = y->_next)
         // if(x->_data > y->_data) swap();
         _isSorted = true;
      }
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};





#endif // DLIST_H
