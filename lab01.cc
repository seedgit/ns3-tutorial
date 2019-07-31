#include "ns3/core-module.h"

using namespace ns3;
int 
main (int argc, char *argv[])
{
  std::cout << "Hello NS3" << std::endl;
  Simulator::Run ();
  Simulator::Destroy ();
}
