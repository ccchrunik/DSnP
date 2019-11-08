/****************************************************************************
  FileName     [ memMgr.h ]
  PackageName  [ cmd ]
  Synopsis     [ Define Memory Manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LABs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MEM_MGR_H
#define MEM_MGR_H

#include <cassert>
#include <iostream>
#include <iomanip>
#include <stdlib.h>

using namespace std;

// Turn this on for debugging
// #define MEM_DEBUG

//--------------------------------------------------------------------------
// Define MACROs
//--------------------------------------------------------------------------
#define MEM_MGR_INIT(T) \
MemMgr<T>* const T::_memMgr = new MemMgr<T>

#define USE_MEM_MGR(T)                                                      \
public:                                                                     \
   void* operator new(size_t t) { return (void*)(_memMgr->alloc(t)); }      \
   void* operator new[](size_t t) { return (void*)(_memMgr->allocArr(t)); } \
   void  operator delete(void* p) { _memMgr->free((T*)p); }                 \
   void  operator delete[](void* p) { _memMgr->freeArr((T*)p); }            \
   static void memReset(size_t b = 0) { _memMgr->reset(b); }                \
   static void memPrint() { _memMgr->print(); }                             \
private:                                                                    \
   static MemMgr<T>* const _memMgr

// You should use the following two MACROs whenever possible to 
// make your code 64/32-bit platform independent.
// DO NOT use 4 or 8 for sizeof(size_t) in your code
//
#define SIZE_T      sizeof(size_t)
#define SIZE_T_1    (sizeof(size_t) - 1)

// TODO: Define them by SIZE_T and/or SIZE_T_1 MACROs.
//
// To promote 't' to the nearest multiple of SIZE_T; 
// e.g. Let SIZE_T = 8;  toSizeT(7) = 8, toSizeT(12) = 16
#define toSizeT(t)      t % 8 == 0 ? t : (t / 8 + 1) * 8  // TODO
//
// To demote 't' to the nearest multiple of SIZE_T
// e.g. Let SIZE_T = 8;  downtoSizeT(9) = 8, downtoSizeT(100) = 96
#define downtoSizeT(t)  t - t % 8  // TODO

// R_SIZE is the size of the recycle list
#define R_SIZE 256

//--------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------
template <class T> class MemMgr;


//--------------------------------------------------------------------------
// Class Definitions
//--------------------------------------------------------------------------
// T is the class that use this memory manager
//
// Make it a private class;
// Only friend to MemMgr;
//
template <class T>
class MemBlock
{
   friend class MemMgr<T>;

   // Constructor/Destructor
   MemBlock(MemBlock<T>* n, size_t b) : _nextBlock(n) {
      _begin = _ptr = new char[b]; _end = _begin + b; }
   ~MemBlock() { delete [] _begin; }

   // Member functions
   void reset() { _ptr = _begin; }
   // 1. Get (at least) 't' bytes memory from current block
   //    Promote 't' to a multiple of SIZE_T
   // 2. Update "_ptr" accordingly
   // 3. The return memory address is stored in "ret"
   // 4. Return false if not enough memory
   bool getMem(size_t t, T*& ret) {
      // TODO
      size_t size = toSizeT(t); // promote t to the nearst multiple of 8
      if(size > getRemainSize()) // exceed the block
      {
         // not sure whether to allocate a new block
         return false;
      } 
      else // can accomodate new block
      {
         ret = (T*)(void*)_ptr;
         _ptr = (_ptr + size); // move the ptr based on size
         return true;
      }
   }
   size_t getRemainSize() const { return size_t(_end - _ptr); }
      
   MemBlock<T>* getNextBlock() const { return _nextBlock; }

   // Data members
   char*             _begin;
   char*             _ptr;
   char*             _end;
   MemBlock<T>*      _nextBlock;
};

// Make it a private class;
// Only friend to MemMgr;
//
template <class T>
class MemRecycleList
{
   friend class MemMgr<T>;

   // Constructor/Destructor
   MemRecycleList(size_t a = 0) : _arrSize(a), _first(0), _nextList(0) {}
   ~MemRecycleList() { reset(); }

   // Member functions
   // ----------------
   size_t getArrSize() const { return _arrSize; }
   MemRecycleList<T>* getNextList() const { return _nextList; }
   void setNextList(MemRecycleList<T>* l) { _nextList = l; }
   // pop out the first element in the recycle list
   T* popFront() {
      // TODO
      if(!_first) // _first contains nothing, actually, it is !0 = 1
      {
         throw "no element in the recycle list!"; 
      }

      T* addr;
      size_t* ptr = (size_t*)(void*)_first;
      if(*ptr == 0) // only one element
      {
         addr = _first;   // set addr to _first
         _first = 0;      // set _first to NULL
      }
      else // have at least two element
      {
         // retrieve the first address
         addr = _first;
         // set the next address
         _first = (T*)(void*)(*ptr); // *ptr is the address of next element
      }
      
      return addr;
   }
   // push the element 'p' to the beginning of the recycle list
   void  pushFront(T* p) {
      // TODO
      if(!_first) // no element now
      {
         size_t* next = (size_t*)(void*)p;
         *next = 0;   // set first element point to null
         _first = p;  // set the first element
      }
      else // has some element
      {
         // set the next of the new_first to current _first
         size_t next_addr = (size_t)(void*)_first;  // find _first address
         size_t* next = (size_t*)(void*)p;          // cast p to size_t*
         *next = next_addr;                         // store _first address into p
         _first = p;                                // set _first to p
      }
      
   }
   // Release the memory occupied by the recycle list(s)
   // DO NOT release the memory occupied by MemMgr/MemBlock
   void reset() {
      // TODO

      // clear the pointer to all the address to 0
      _first = 0;

      MemRecycleList<T>* next;
      
      // size_t* ptr = (size_t*)(void*)_first;

      // *ptr = 0;
      // only one recycle list
      if(!getNextList())
      {
         return ;
      }

      next = getNextList();
      
      while(next->getNextList())
      {
         // ptr = (size_t*)(void*)next->_first;
         // *ptr = 0;
         next->_first = 0;
         next = next->getNextList();
         // cout << "in list reset" << endl;
      }
      
      // the last case
      // ptr = (size_t*)(void*)next->_first;
      // *ptr = 0;
      next->_first = 0;

   }        

   // Helper functions
   // ----------------
   // count the number of elements in the recycle list
   size_t numElm() const {
      // TODO
      // !!!!!!!May have error!!!!!!!!
      size_t count = 0;
      size_t* ptr = (size_t*)(void*)_first;
      if(!ptr) return 0; // no element now
      size_t next_addr = *ptr;
      while(next_addr != 0) 
      {
         count++;
         ptr = (size_t*)(void*)next_addr;
         next_addr = *ptr;
      }
      // the last case, next_addr == 0
      count++;
      return count;
   }

   // Data members
   size_t              _arrSize;   // the array size of the recycled data
   T*                  _first;     // the first recycled data
   MemRecycleList<T>*  _nextList;  // next MemRecycleList
                                   //      with _arrSize + x*R_SIZE
};

template <class T>
class MemMgr
{
   #define S sizeof(T)

public:
   MemMgr(size_t b = 65536) : _blockSize(b) {
      assert(b % SIZE_T == 0);
      _activeBlock = new MemBlock<T>(0, _blockSize);
      for (int i = 0; i < R_SIZE; ++i)
         _recycleList[i]._arrSize = i;
   }
   ~MemMgr() { reset(); delete _activeBlock; }

   // 1. Remove the memory of all but the firstly allocated MemBlocks
   //    That is, the last MemBlock searched from _activeBlock.
   //    reset its _ptr = _begin (by calling MemBlock::reset())
   // 2. reset _recycleList[]
   // 3. 'b' is the new _blockSize; "b = 0" means _blockSize does not change
   //    if (b != _blockSize) reallocate the memory for the first MemBlock
   // 4. Update the _activeBlock pointer
   void reset(size_t b = 0) {
      assert(b % SIZE_T == 0);
      #ifdef MEM_DEBUG
      cout << "Resetting memMgr...(" << b << ")" << endl;
      #endif // MEM_DEBUG
      // TODO

      _blockSize = b;
      // size_t now_size = b;
      size_t now_size = _blockSize;
      MemBlock<T>* next = _activeBlock;
      size_t block_size = (size_t)(void*)(next->_end - next->_begin);

      while(next->getNextBlock()) // next is not 0
      {
         
         // cout << next << endl;
         next = next->getNextBlock();        // move pointer to the next block
         // delete _activeBlock;                // delete memeory block
         block_size = (size_t)(void*)(next->_end - next->_begin);
         // cout << "in mgr reset" << endl;
      }
     
      // now left the last block, then reset it(clear all its data)
      if(block_size != now_size) // different size
      {
         MemBlock<T>* new_block = new MemBlock<T>(0, now_size);
         // delete _activeBlock;
         _activeBlock = new_block;
         // cout << "_activeBlock->_ptr" << _activeBlock->_ptr << endl;
         // MemBlock<T>* block = _activeBlock;
         // _activeBlock = new MemBlock<T>(0, now_size);
         // delete block;
         // cout << "_activeBlock->_ptr" << _activeBlock->_ptr << endl;
      } 
      else // they are the same size
      {
         next->reset();
      }

      // clear a pointer to memory in the recycle list
      for(int i = 0; i < R_SIZE; ++i)
      {
         _recycleList[i].reset();
      }
   }
   // Called by new
   T* alloc(size_t t) {
      assert(t == S);
      #ifdef MEM_DEBUG
      cout << "Calling alloc...(" << t << ")" << endl;
      #endif // MEM_DEBUG
      return getMem(t);
   }
   // Called by new[]
   T* allocArr(size_t t) {
      #ifdef MEM_DEBUG
      cout << "Calling allocArr...(" << t << ")" << endl;
      #endif // MEM_DEBUG
      // Note: no need to record the size of the array == > system will do
      return getMem(t);
   }
   // Called by delete
   void  free(T* p) {
      #ifdef MEM_DEBUG
      cout << "Calling free...(" << p << ")" << endl;
      #endif // MEM_DEBUG
      getMemRecycleList(0)->pushFront(p);
   }
   // Called by delete[]
   void  freeArr(T* p) {
      #ifdef MEM_DEBUG
      cout << "Calling freeArr...(" << p << ")" << endl;
      #endif // MEM_DEBUG
      // TODO
      // Get the array size 'n' stored by system,
      // which is also the _recycleList index

      size_t* tmp = (size_t*)(void*)p;
      size_t n = *tmp;
      
      #ifdef MEM_DEBUG
      cout << ">> Array size = " << n << endl;
      cout << "Recycling " << p << " to _recycleList[" << n << "]" << endl;
      #endif // MEM_DEBUG
      // add to recycle list...
      getMemRecycleList(n)->pushFront(p);
   }
   void print() const {
      cout << "=========================================" << endl
           << "=              Memory Manager           =" << endl
           << "=========================================" << endl
           << "* Block size            : " << _blockSize << " Bytes" << endl
           << "* Number of blocks      : " << getNumBlocks() << endl
           << "* Free mem in last block: " << _activeBlock->getRemainSize()
           << endl
           << "* Recycle list          : " << endl;
      int i = 0, count = 0;
      while (i < R_SIZE) {
         const MemRecycleList<T>* ll = &(_recycleList[i]);
         while (ll != 0) {
            size_t s = ll->numElm();
            // cout << "s = " << s << endl;
            // cout << "i = " << i << endl;
            // cout << "ll = " << ll << endl;
            if (s) {
               cout << "[" << setw(3) << right << ll->_arrSize << "] = "
                    << setw(10) << left << s;
               if (++count % 4 == 0) cout << endl;
            }
            ll = ll->_nextList;
         }
         ++i;
      }
      cout << endl;
   }

private:
   size_t                     _blockSize;
   MemBlock<T>*               _activeBlock;
   MemRecycleList<T>          _recycleList[R_SIZE];

   // Private member functions
   //
   // t: #Bytes; MUST be a multiple of SIZE_T
   // return the size of the array with respect to memory size t
   // [Note] t must >= S
   // [NOTE] Use this function in (at least) getMem() to get the size of array
   //        and call getMemRecycleList() later to get the index for
   //        the _recycleList[]
   size_t getArraySize(size_t t) const {
      // cout << "in getArraySize: " << t << endl;
      assert(t % SIZE_T == 0);
      assert(t >= S);
      // TODO
      // First, substract SIZE_T
      // Next, divide size by sizeof(T) to get the size of array
      
      // size_t size = (t - SIZE_T) / S; 

      return (t - SIZE_T) / S; 
   }
   // Go through _recycleList[m], its _nextList, and _nexList->_nextList, etc,
   //    to find a recycle list whose "_arrSize" == "n"
   // If not found, create a new MemRecycleList with _arrSize = n
   //    and add to the last MemRecycleList
   // So, should never return NULL
   // [Note]: This function will be called by MemMgr->getMem() to get the
   //         recycle list. Therefore, the recycle list is first created
   //         by the MTNew command, not MTDelete.
   MemRecycleList<T>* getMemRecycleList(size_t n) {
      // TODO
      size_t m = n % R_SIZE;
      MemRecycleList<T>* list = &_recycleList[m];
      while(list->getNextList() != 0) // keep iterating until we find the match, otherwise fail
      {
         if(list->getArrSize() == n) // match size
         {
            return list;
         }
         else 
         {
            list = list->getNextList(); // keep finding the next recycle list
         }
      }
      
      // the boundary case(last recycle list)
      if(list->getArrSize() == n)
      {
         return list;
      } 
      else // no matching, then to create a new recycle list to return
      {
         MemRecycleList<T>* new_list = new MemRecycleList<T>(n); // create a new recycle list
         // new_list->setNextList(&_recycleList[m]);
         // _recycleList[m] = *new_list;
         list->setNextList(new_list);
         return new_list;
      }
   }
   // t is the #Bytes requested from new or new[]
   // Note: Make sure the returned memory is a multiple of SIZE_T
   T* getMem(size_t t) {
      T* ret = 0;

      // cout << "_activeBlock->_ptr" << _activeBlock->_ptr << endl;
      #ifdef MEM_DEBUG
      cout << "Calling MemMgr::getMem...(" << t << ")" << endl;
      #endif // MEM_DEBUG
      // 1. Make sure to promote t to a multiple of SIZE_T
      t = toSizeT(t);
      // 2. Check if the requested memory is greater than the block size.
      //    If so, throw a "bad_alloc()" exception.
      //    Print this message for exception
      //    cerr << "Requested memory (" << t << ") is greater than block size"
      //         << "(" << _blockSize << "). " << "Exception raised...\n";
      // TODO
      if(t > _blockSize) // the required size is too big
      {
         throw bad_alloc();  // may be wrong
         cerr << "Requested memory (" << t << ") is greater than block size"
              << "(" << _blockSize << "). " << "Exception raised...\n";
      }

      // 3. Check the _recycleList first...
      //    Print this message for memTest.debug
      //    #ifdef MEM_DEBUG
      //    cout << "Recycled from _recycleList[" << n << "]..." << ret << endl;
      //    #endif // MEM_DEBUG
      //    => 'n' is the size of array
      //    => "ret" is the return address
      // t means the size of required memory
      // cout << "t: " << t << endl;
      size_t n = getArraySize(t);
      // cout << "n = " << n << endl;
      MemRecycleList<T>* re = getMemRecycleList(n);
      
      // if find a recycle list, check if _first is null
      // if not null, mean that there is memory we can use
      // else, we should resort to memory block
      if(re->_first != 0)
      {
         ret = re->popFront();
         return ret;
      }

      #ifdef MEM_DEBUG
      cout << "Recycled from _recycleList[" << n << "]..." << ret << endl;
      #endif // MEM_DEBUG
      // TODO

      // If no match from recycle list...
      // 4. Get the memory from _activeBlock
      // 5. If not enough, recycle the remained memory and print out ---
      //    Note: recycle to the as biggest array index as possible
      //    Note: rn is the array size
      //    Print this message for memTest.debug
      //    #ifdef MEM_DEBUG
      //    cout << "Recycling " << ret << " to _recycleList[" << rn << "]\n";
      //    #endif // MEM_DEBUG
      //    ==> allocate a new memory block, and print out ---
      //    #ifdef MEM_DEBUG
      //    cout << "New MemBlock... " << _activeBlock << endl;
      //    #endif // MEM_DEBUG
      // TODO
      
      // cout << "re->_first = " << re->_first << endl;
      // re must contain the address of the specific recycle list
      if(re->_first == 0) // null, means the recycle lists has no memory to return, resort to memory block
      {
         size_t size = toSizeT(_activeBlock->getRemainSize());
         if(t > size) // _activeBlock has no enough memory, require a new block from system
         {
            // add remain memory into recycle list
            // notice that we should first check is size >= S
            if(size >= S)
            {
               size_t rn = getArraySize(size);                    // determine the remained array size
               MemRecycleList<T>* search = getMemRecycleList(rn); // find the size to recycle _activeBlock memory  

               #ifdef MEM_DEBUG
               cout << "Recycling " << ret << " to _recycleList[" << rn << "]\n";
               #endif // MEM_DEBUG

               // Since getMemRecycleList must return a recycle list
               T* addr = (T*)(void*)_activeBlock->_ptr;
               search->pushFront(addr);
            } 
            // else 
            // {
            //    cout << "not enough space in _activeBlock" << endl;
            //    cout << "getting memory from new MemBlock" << endl;
            // }

            
            
            // now allocate memory based on new memory block
            MemBlock<T>* new_block = new MemBlock<T>(_activeBlock, _blockSize); // require a new memory block
            _activeBlock = new_block;
            ret = (T*)(void*)_activeBlock->_ptr;                 // set ret the new block address
            _activeBlock->_ptr = (_activeBlock->_ptr + t);       // move current pointer to size

            #ifdef MEM_DEBUG
            cout << "New MemBlock... " << _activeBlock << endl;
            #endif // MEM_DEBUG
         }
         else  // _activeBlock has enough memory to allocate
         {
            // cout << "_activeBlock->_ptr" << _activeBlock->_ptr << endl;
            ret = (T*)(void*)_activeBlock->_ptr;           // set ptr as return address                         
            _activeBlock->_ptr = (_activeBlock->_ptr + t); // move current pointer to t
         }
         
      }

      // 6. At the end, print out the acquired memory address
      #ifdef MEM_DEBUG
      cout << "Memory acquired... " << ret << endl;
      #endif // MEM_DEBUG
      return ret;
   }

   // Get the currently allocated number of MemBlock's
   size_t getNumBlocks() const {
      // TODO
      size_t count = 0;
      MemBlock<T>* nextBlock = _activeBlock->getNextBlock();
      while(nextBlock != 0) // keep finding until the nextBlock = 0
      {
         count++;
         nextBlock = nextBlock->getNextBlock();
      }
      // count for the last block
      count++;
      return count;
   }

};

#endif // MEM_MGR_H
