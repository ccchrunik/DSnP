/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
   typedef pair<size_t, size_t> INDEX;

public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator() { bkt = 0; chn = 0; }
      iterator(vector<Data>* d, size_t b, size_t i, size_t j, size_t e = 0) { _d = d; b_num = b; bkt = i; chn = j; atEnd = e; }
      const Data& operator * () const 
      {
         // cout << "get data" << endl;
         return _d[bkt][chn];
      }
      iterator& operator ++ () 
      { 
         if(atEnd) return (*this);
         find_next(); return (*this); 
      }
      iterator operator ++ (int i = 0) 
      { 
         iterator tmp = *this;
         if(atEnd) return tmp;
         find_next(); return tmp; 
      }
      iterator& operator -- () 
      {
         if(atEnd) return (*this);
         find_prev(); return (*this); 
      }
      iterator operator -- (int i = 0) 
      { 
         iterator tmp = *this;
         if(atEnd) return tmp; 
         find_prev(); return tmp; 
      }
      size_t get_bkt() { return bkt; }
      size_t get_chn() { return chn; }

      void operator = (iterator& i) { i._d = _d; i.bkt = bkt; i.chn = chn; i.b_num = b_num; i.atEnd = atEnd; }
      bool operator == (const iterator& i) const 
      { 
         return (i._d == _d && i.bkt == bkt && i.chn == chn || i.atEnd == atEnd); 
      }
      bool operator != (const iterator& i) const { return !(i == (*this)); }
   private:
      vector<Data>* _d;
      // bkt for bucket, chn for chain
      size_t bkt, chn, b_num;
      bool atEnd;

      void find_prev()
      {
         atEnd = 0;
         if(chn > 0) --chn; // directly find the next element in the bucket
         else
         {
            while(true)
            {
               if(bkt == 0) 
               {
                  return ; // reach the head of the vector
               }
               --bkt;
               if(!_d[bkt].empty()) 
               {
                  chn = _d[bkt].size() - 1; return ;
               }
            }
         }
      }

      void find_next()
      {
         if(atEnd == 1) return ;
         if(chn < (_d[bkt].size() - 1)) ++chn; // directly find the next element in the bucket
         else
         {
            while(true)
            {
               if(bkt >= b_num - 1) 
               {
                  atEnd = 1;
                  return ; // reach the end of the vector
               }
               ++bkt;
               if(!_d[bkt].empty()) // has something
               {
                  chn = 0; return ;
               }
            }
         }
      }

   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const 
   {
      for(int i = 0; i < _numBuckets; ++i)
      {
         if(_buckets[i].empty()) continue;
         else return iterator(_buckets, _numBuckets, i, 0);
      }
      return iterator(_buckets, _numBuckets, _numBuckets + 1, 0, 1); 
   }
   // Pass the end
   iterator end() const 
   { 
      return iterator(_buckets, _numBuckets, _numBuckets, 0, 1); 
   }
   // return true if no valid data
   bool empty() const 
   { 
      for(int i = 0; i < _buckets.size(); ++i)
      {
         if(!_buckets[i].empty()) return false; // have something in _buckets[i]
      }
      return true; 
   }
   // number of valid data
   size_t size() const 
   { 
      size_t s = 0;
      for(int i = 0; i < _buckets->size(); ++i)
      {
         s += _buckets[i].size();
      }   
      return s;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const 
   { 
      size_t b = bucketNum(d);
      for(int i = 0; i < _buckets[b].size(); ++i)
      {
         if(_buckets[b][i] == d) return true;
      }
      return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   // bool query(Data& d)
   bool query(Data& d) const 
   { 
      INDEX i = find_query(d);
      if(i.first != _numBuckets + 1)
      {
         d = _buckets[i.first][i.second];
         return true;
      }
      return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false
   bool update(const Data& d) 
   { 
      // INDEX u = find_task(d);
      if(begin() == end()) return false;
      size_t b = bucketNum(d);
      int IN = _numBuckets + 1;
      for(int i = 0; i < _buckets[b].size(); ++i)
      {
         if(_buckets[b][i] == d)
         {
            IN = i; break;
         }
      }
      if(IN != _numBuckets + 1)
      {
         _buckets[b][IN] = d;
         return true;
      }
      else
      {
         insert(d);
         return false; 
      }
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) 
   { 
      if(check(d)) return false;
      // cout << "insert " << d.getName() << endl;
      size_t b = bucketNum(d);
      _buckets[b].push_back(d);
      return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return false otherwise (i.e. nothing is removed)
   bool remove(const Data& d) 
   {
      // cout << "in hash remove" << endl;
      if(begin() == end()) return false;
      size_t b = bucketNum(d);
      int IN = _numBuckets + 1;
      for(int i = 0; i < _buckets[b].size(); ++i)
      {
         if(_buckets[b][i] == d)
         {
            IN = i; break;
         }
      }
      // cout << "after iterate" << endl;
      // cout << r.first << " " << r.second << endl;
      if(IN == _numBuckets + 1) return false;
      else
      {
         // cout << "in remove process" << endl;
         size_t p = b;
         size_t index = IN;
         size_t s = _buckets[p].size();
         if(s == 0)
         {
            return false;
         }
         else if(s == 1)
         {
            _buckets[p].pop_back();
         }
         else
         {
            swap(_buckets[p][index], _buckets[p][s - 1]);
            _buckets[p].pop_back();
         }
         return true;
      } 
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }

   void swap(Data& d1, Data& d2)
   {
      Data tmp = d1 ; d1 = d2; d2 = tmp;
   }

   INDEX find_task(const Data& d)
   {
      size_t p = bucketNum(d);
      size_t terminate = p;
      while(true)
      {
         for(int i = 0; i < _buckets[p].size(); ++i)
         {
            // cout << i << " ";
            if(_buckets[p][i] == d)
            {
               // cout << endl;
               return INDEX(p, i);
            }
         }
         // p = (p + 1) % numBuckets;
         p = (++p) % _numBuckets;
         // if(p == terminate) return INDEX(_numBuckets + 1, 0);
      }
      return INDEX(_numBuckets + 1, 0);
   }

   INDEX find_query(Data& d) const
   {
      size_t p = bucketNum(d);
      size_t terminate = p;
      while(true)
      {
         auto iter = find(_buckets[p].begin(), _buckets[p].end(), d);
         if(iter != _buckets[p].end()) 
         {
            size_t index = distance(_buckets[p].begin(), iter);
            return INDEX(p, index);
         }
         // p = (p + 1) % numBuckets;
         p = (++p) % _numBuckets;
         if(p == terminate) return INDEX(_numBuckets + 1, 0);
      }
      return INDEX(_numBuckets + 1, 0);
   }

   size_t find_insert(const Data& d)
   {
      size_t p = bucketNum(d);
      size_t terminate = p;
      while(true)
      {
         if(_buckets[p].size() < _numBuckets) // still have space
         {
            return p;
         }
         // p = (p + 1) % numBuckets;
         p = (++p) % _numBuckets;
         if(p == terminate) return _numBuckets + 1;
      }
      return _numBuckets + 1;
   }

};

#endif // MY_HASH_SET_H
