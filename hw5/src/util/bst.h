/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>
#include <queue>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;

public:
   // TODO: design your own class!!
   BSTreeNode(const T& d, BSTreeNode<T>* p = 0, BSTreeNode<T>* l = 0, BSTreeNode<T>* r = 0) :
      _data(d), _parent(p), _left(l), _right(r) {}
private:
   T _data;
   BSTreeNode<T>* _parent;
   BSTreeNode<T>* _left;
   BSTreeNode<T>* _right;
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
   // typedef pair<BSTreeNode<T>*, bool> t_node;
   
public:
   BSTree() 
   { 
      _root = new BSTreeNode<T>(T());
      _size = 0;
   }
   ~BSTree() { clear(); delete _root; }

   class iterator
   {
      friend class BSTree;
      // typedef pair<BSTreeNode<T>*, bool> t_node;
      
   public:
      // for each iterator, it must pass both begin and end
      iterator(BSTreeNode<T>* n = 0, BSTreeNode<T>* b = 0, BSTreeNode<T>* e = 0, bool d = 0): _node(n), _begin(b), _end(e), _dummy(d) {}
      iterator(const iterator& i): _node(i._node), _begin(i._begin), _end(i._end), _dummy(i._dummy) {}
      ~iterator() { /*_trace.clear();*/ } // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      // prefix, like ++i
      iterator& operator ++ () 
      { 
         // cout << "++" << endl;
         successor(1);
         return (*this);
      }
      // postfix, like i++
      iterator operator ++ (int) 
      {
         // cout << "(++)" << endl;
         iterator tmp(*this);
         successor(1);
         return tmp;
      }
      // prefix, like --i
      iterator& operator -- () 
      { 
         // cout << "--" << endl;
         if(_dummy == 1) // at the end
         {
            _dummy = 0;
            return (*this);
         }
         successor(0);
         return (*this);
      }
      // postfix, like i--
      iterator operator -- (int) 
      {
         // cout << "(--)" << endl;
         if(_dummy == 1) // at the end
         {
            _dummy = 0;
            return iterator(_end->_right, _begin, _end, 0);
         }
         iterator tmp(*this);
         successor(0);
         return tmp;
      }

      iterator& operator = (const iterator& i) 
      { 
         _node = i._node; _begin = i._begin; _end = i._end;
         return (*this); 
      }

      bool operator != (const iterator& i) const { return _node != i._node || _dummy != i._dummy; }
      bool operator == (const iterator& i) const { return _node == i._node && _dummy == i._dummy; }

   private:
      BSTreeNode<T>* _node;
      BSTreeNode<T>* _begin;
      BSTreeNode<T>* _end;
      bool _dummy;

      void successor(bool next = 0)
      {
         // cout << "iterate" << endl;
         if(!next) // that is, --, find smaller one
         {
            if(_dummy == 1) _dummy = 0;
            // cout << "--" << endl;
            if(_node == _begin)
            {
               // cout << "reach-head" << endl;
               return ;
            } 
            
            if(_node->_left != 0) // still have left child
            {
               _node = _node->_left;
               while (_node->_right != 0) // keep finding the relative largest node but smaller that this node
               {
                  _node = _node->_right;
               }
            }
            else
            {
               while(true)
               {
                  // cout << "3" << endl;
                  if(_node->_parent == 0) return ; // root

                  if(_node->_parent->_data <= _node->_data) // it comes from the right edge of preious node
                  {
                     _node = _node->_parent;
                     return ;
                  }
                  else 
                  {
                     _node = _node->_parent;
                  }
               }
            }
         }
         else
         {
            // cout << "++" << endl;

            if(_node == _end) 
            {
               _dummy = 1;
               // cout << "reach end" << endl;
               return ; // may have error
            } 

            if(_node->_right != 0) // still have right child
            {
               // cout << "have right child" << endl;
               _node = _node->_right;
               while (_node->_left != 0) // keep finding the relative smallest node but larger that this node
               {
                  _node = _node->_left;
               }
            }
            else
            {
               // cout << "no right child" << endl;
               while(true)
               {
                  if(_node->_parent == 0) return ; // root

                  if(_node->_parent->_data > _node->_data) // it comes from the left edge of preious node
                  {
                     _node = _node->_parent;
                     return ;
                  }
                  else 
                  {
                     _node = _node->_parent;
                  }
               }
            }
            
         }
         
      }

      

   };

   iterator begin() const 
   { 
      if(!empty())
      {
         // cout << "new a begin" << endl;
         BSTreeNode<T>* b = find_first();
         BSTreeNode<T>* e = find_last();   
         return iterator(b, b, e);
      } 
      else return 0;
   }
   iterator end() const
   { 
      if(!empty()) 
      {
         BSTreeNode<T>* b = find_first();
         BSTreeNode<T>* e = find_last();  
         return iterator(e, b, e, 1);
      } 
      else return 0;
   }
   bool empty() const { return (!_size); } // even _root is not created
   size_t size() const { return _size; }

   
   void pop_front() 
   { 
      // erase(find_first());
      erase(begin());
   }
   void pop_back() 
   { 
      // erase(find_last());
      erase(end());
   }
   void insert(const T& x) 
   { 
      if(_size == 0)
      {
         _root->_data = x;
         // cout << "x = " << x << endl;
         _size += 1;
         return ;
      }
      BSTreeNode<T>* node = find_insert_node(x, _root, _root);
      if(x < node->_data)
      {
         // cout << node->_data << endl;
         // cout << "insert-left" << endl;
         node->_left = new BSTreeNode<T>(x, node, 0, 0);
      } 
      else
      {
         // cout << node->_data << endl;
         // cout << "insert-right" << endl;
         node->_right = new BSTreeNode<T>(x, node, 0, 0);
      }
      // cout << "x = " << x << endl;
      _size += 1;
   }

   bool erase(iterator pos) 
   { 
      if(_size == 0) return false;
      // note that size should -= 1
      if(pos._node == 0) return false;

      if(_size == 1)
      {
         _size -= 1;
         return true;
      }

      bool isBegin = 0, isEnd = 0;
      if(pos._node == pos._end) // change the end node
      {
         isEnd = 1;
      }

      if(pos._node == pos._begin)
      {
         isBegin = 1;
      }

      
      while(true)
      {
         if(pos._node->_left == 0 && pos._node->_right == 0) // the node has no child
         {
            cut_edge_node(pos);
            delete pos._node;
            _size -= 1;
            if(isBegin) pos._begin = begin()._node;
            if(isEnd) pos._end = end()._node;
            return true;
         }
         else if(pos._node->_left == 0 || pos._node->_right == 0) // the node has one child
         {
            cut_one_child_node(pos);
            delete pos._node;
            _size -= 1;
            if(isBegin) pos._begin = begin()._node;
            if(isEnd) pos._end = end()._node;
            return true;
         }
         else // the node has two childs
         {
            move_right(pos);
         }
      }
   }
   bool erase(const T& x) 
   { 
      iterator tmp = find(x);
      if(tmp == 0) return false; 
      else return erase(tmp);
   }

   iterator find(const T& x)
   { 
      if(!empty())
      {
         BSTreeNode<T>* b = find_first();
         BSTreeNode<T>* e = find_last();
         iterator node(_root, b, e);
         if(find_node(x, node)) return node;
         else return end();
      }
      else
      {
         return end();
      }
      
   }

   void clear() 
   {
      // search geekforgeeks delete binary tree  
      if(_size == 0) return ;
      queue<BSTreeNode<T>*> q;

      if(_root->_left != 0) q.push(_root->_left);
      if(_root->_right != 0) q.push(_root->_right);
      _root->_left = 0; _root->_right = 0;

      while(!q.empty()) // while queue still has elements
      {
         BSTreeNode<T>* tmp = q.front();
         if(tmp->_left != 0) q.push(tmp->_left);
         if(tmp->_right != 0) q.push(tmp->_right);
         q.pop();
         delete tmp;
      }
      _size = 0; 
   }

   void sort() { } // actually do nothing

   void print() const { }

private:
   BSTreeNode<T>* _root;
   size_t _size;

   int find_pos(BSTreeNode<T>* p, BSTreeNode<T>* n)
   {
      if(p->_left == n) return 0;      // left child
      else if(p->_right == n) return 1; // right child
      else return -1; // error
   }

   void rePoint(BSTreeNode<T>* p, BSTreeNode<T>* n, bool right)
   {
      // p is parent, n is node, right is for parent
      if(!right)
      {
         p->_left = n;
         n->_parent = p;
      }
      else
      {
         p->_right = n;
         n->_parent = p;
      }
   }

   void cut_edge_node(iterator& pos)
   {
      if(find_pos(pos._node->_parent, pos._node) == 0) // the deleted node is the left node of parent node
      {
         pos._node->_parent->_left = 0;
      }
      else // the deleted node is the right node of parent node
      {
         pos._node->_parent->_right = 0;
      }
   }

   void cut_one_child_node(iterator& pos)
   {
      if(pos._node == _root)
      {
         if(pos._node->_left == 0) // no left child
         {
            _root = pos._node->_right;
            _root->_parent = 0;   
         }
         else // no right child
         {
            _root = pos._node->_left;
            _root->_parent = 0;
         }
         return ;
      }

      if(pos._node->_left == 0) // has right child
      { 
         if(find_pos(pos._node->_parent, pos._node) == 0) //  the deleted node is the left node of parent node
         {
            rePoint(pos._node->_parent, pos._node->_right, 0); // parent->left = right child
         }
         else
         {
            rePoint(pos._node->_parent, pos._node->_right, 1); // parent->right = left child
         }
      }
      else // has left child
      {
         if(find_pos(pos._node->_parent, pos._node) == 0) //  the deleted node is the left node of parent node
         {
            rePoint(pos._node->_parent, pos._node->_left, 0); // parent->left = right child
         }
         else
         {
            rePoint(pos._node->_parent, pos._node->_left, 1); // parent->right = left child
         }
      }
   } 

   void swap_data(BSTreeNode<T>* node, bool right = 0) 
   { 
      if(!right) // swap value with left child
      {
         T tmp = node->_data;
         node->_data = node->_left->_data;
         node->_left->_data = tmp;
      }
      else // swap value with right child
      {
         T tmp = node->_data;
         node->_data = node->_right->_data;
         node->_right->_data = tmp;
      }

   }

   void move_left(iterator& iter) 
   {
      swap_data(iter._node, 0);
      iter._node = iter._node->_left;
   }

   void move_right(iterator& iter) 
   { 
      swap_data(iter._node, 1);
      iter._node = iter._node->_right;
   }

   BSTreeNode<T>* find_edge_node(BSTreeNode<T>* node, bool right = 0) const
   {
      if(!right) // find the smallest node
      {
         BSTreeNode<T>* tmp = node;
         while(true)
         {
            if(tmp->_left == 0) return tmp;
            tmp = tmp->_left;
         }
      }
      else // find the largest node
      {
         BSTreeNode<T>* tmp = node;
         while(true)
         {
            if(tmp->_right == 0) return tmp;
            tmp = tmp->_right;
         }
      }
   }

   BSTreeNode<T>* findRecur(BSTreeNode<T>* node, bool right = 0) const
   {
      if(!right) return find_edge_node(node, 0);
      else return find_edge_node(node, 1);
   }

   BSTreeNode<T>* find_first() const
   {
      // find the smallest element
      return find_edge_node(_root, 0);  
   }

   BSTreeNode<T>* find_last() const
   {
      // find the largest element
      return find_edge_node(_root, 1);
   }

   
   bool find_node(const T& x, iterator& iter)
   {
      // cout << iter._node->_data << endl;
      if(iter._node == 0) return false;
      else if(x == iter._node->_data) return true;
      else if(x < iter._node->_data)
      {
         iter._node = iter._node->_left;
         return find_node(x, iter);
      }
      else
      {
         iter._node = iter._node->_right;
         return find_node(x, iter);
      }
   }

   BSTreeNode<T>* find_insert_node(const T& x, BSTreeNode<T>* node, BSTreeNode<T>* parent) 
   {
      // find node until node if null
      if(node == 0) return parent;
      else if(x < node->_data) return find_insert_node(x, node->_left, node);
      else return find_insert_node(x, node->_right, node);
   }
   
};



#endif // BST_H
