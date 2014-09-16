
#include "pump.h"

void Fountain::init(int firstpin)
{
  for (int pp = 0; pp < size(); ++pp)
  {
   pump[pp].init(firstpin + pp);
   pump[pp].test();
  }
}

void Fountain::test(Remote *remote)
{
  for (int pp = 0; pp < size(); ++pp)
  {
   if (remote->pressed())
     break;
   pump[pp].test();
  }
}

void Fountain::off() {
  for (int pp = 0; pp < size(); ++pp)
  {
   pump[pp].off();
  }
}

