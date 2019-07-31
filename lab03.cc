#include <fstream>
#include <iostream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/aodv-module.h"
#include "ns3/olsr-module.h"
#include "ns3/dsdv-module.h"
#include "ns3/dsr-module.h"
#include "ns3/applications-module.h"
#include "ns3/yans-wifi-helper.h"

using namespace ns3;

void ViewPosition(Ptr<Node> node) {
  std::cout << Simulator::Now().GetSeconds() << std::endl;
  Simulator::Schedule (Seconds (1.0), &ViewPosition);
}

int main (int argc, char *argv[])
{
  NodeContainer nodes;
  nodes.Create (2);

  MobilityHelper mobility;  
  /*
  mobility.SetPositionAllocator("ns3::RandomRectanglePositionAllocator",
    "X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=20.0]"),
    "Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=20.0]")
  );
  */
 /*
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (10),
                                 "DeltaY", DoubleValue (10),
                                 "GridWidth", UintegerValue (5),//Number of node in row
                                 "LayoutType", StringValue ("RowFirst"));
  
  
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  */

  ObjectFactory pos;
  pos.SetTypeId ("ns3::RandomRectanglePositionAllocator");
  pos.Set ("X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1000.0]"));
  pos.Set ("Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1000.0]"));
  Ptr<PositionAllocator> taPositionAlloc = pos.Create ()->GetObject<PositionAllocator> ();

  mobility.SetMobilityModel ("ns3::RandomWaypointMobilityModel",
                                  "Speed", StringValue ("ns3::UniformRandomVariable[Min=10.3|Max=40.7]"),
                                  "Pause", StringValue ("ns3::ConstantRandomVariable[Constant=0.01]"),
                                  "PositionAllocator", PointerValue (taPositionAlloc));

  mobility.Install (nodes);

  Simulator::Schedule (Seconds (1.0), &ViewPosition, nodes.Get(0));

  Simulator::Stop(Seconds (100.0));
  Simulator::Run ();
  Simulator::Destroy ();
}
