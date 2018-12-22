#include "OrdList.h"
#include <unistd.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <thread>
#include <atomic>
#include <string>
#include "stm/stm.h"
#include "stm/context.h"

#define NUM_ELEMENTS 10000

stm::Context cxt;
stm::TVar<OrdList<int>> global_list;

void push_to_list(int c)
{
  auto f_push_to_list = [&](OrdList<int> l)
  {
    return stm::writeTVar(global_list, l.inserted(c));
  };
  auto push_transaction = stm::withTVar<OrdList<int>, stm::Unit>(global_list, f_push_to_list);
  stm::atomically(cxt, push_transaction);
}

int pop_from_list()
{
  auto f_pop_from_list = [](OrdList<int> l)
  {
    if(l.isEmpty())
      return stm::retry<int>();
    else
    {
      int h = l.front();
      auto write_tvar = stm::writeTVar(global_list, l.popped_front());
      auto f_return_h = [h](stm::Unit)
      {
        return stm::pure(h);
      };
      return stm::bind<stm::Unit, int>(write_tvar, f_return_h);
    }
  };
  auto pop_transaction = stm::withTVar<OrdList<int>, int>(global_list, f_pop_from_list);
  return stm::atomically(cxt, pop_transaction);
}

void merge_list(stm::TVar<OrdList<int>>  a, stm::TVar<OrdList<int>>  b, stm::TVar<OrdList<int>>  m)
{
  auto read_list1 = stm::readTVar(a);
  auto list1 = stm::atomically(cxt, read_list1);

  auto read_list2 = stm::readTVar(b);
  auto list2 = stm::atomically(cxt, read_list2);

  
auto f_push_to_list = [&](OrdList<int> l)
  {
	return stm::writeTVar(m, merged(list1, list2));
  };
  auto push_transaction = stm::withTVar<OrdList<int>, stm::Unit>(m, f_push_to_list);
  stm::atomically(cxt, push_transaction);
}


void print_list()
{
  auto read_list = stm::readTVar(global_list);
  auto list = stm::atomically(cxt, read_list);
  print(list);
}

bool is_list_empty()
{
  auto f_list_empty = [](OrdList<int> l)
  {
    return stm::pure(l.isEmpty());
  };
  auto check_transaction = stm::withTVar<OrdList<int>, bool>(global_list,  f_list_empty);
  return stm::atomically(cxt, check_transaction);
}

void fill_list()
{
  for(int i = 0; i < NUM_ELEMENTS; i++)
  {
    push_to_list(i);
  }
}

void read_from_list(int* acc)
{
  for(int i = 0; i < NUM_ELEMENTS; i++)
  {
    *acc += pop_from_list();
  }
}

int main()
{
    int acc1 = 0;
    int acc2 = 0;
    int acc3 = 0;

    global_list = stm::newTVarIO<OrdList<int>>(cxt, OrdList<int>());
    std::thread fl1(fill_list);
    std::thread fl2(fill_list);
    std::thread fl3(fill_list);
 
    std::thread rd1(read_from_list, &acc1);
    std::thread rd2(read_from_list, &acc2);
    std::thread rd3(read_from_list, &acc3);   

    fl1.join();
    fl2.join();
    fl3.join();
    
    rd1.join();
    rd2.join();
    rd3.join();

    int acc = acc1 + acc2 + acc3;

    std::cout << "acc1 = " << acc1 << std::endl;
    std::cout << "acc2 = " << acc2 << std::endl;
    std::cout << "acc3 = " << acc3 << std::endl;
    std::cout << "acc = " << acc << std::endl;

return 0;

}

