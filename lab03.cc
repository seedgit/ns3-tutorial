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

int main (int argc, char *argv[])
{
  NodeContainer nodes;
  nodes.Create (10);
  
  MobilityHelper mobility; 
  int mType = 0;
  if(mType == 0) {
    mobility.SetPositionAllocator("ns3::RandomRectanglePositionAllocator",
      "X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=100.0]"),
      "Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=100.0]")
    );
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install (nodes);
  } else if (mType == 1) {
    mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue (0.0),
                                  "MinY", DoubleValue (0.0),
                                  "DeltaX", DoubleValue (10),
                                  "DeltaY", DoubleValue (10),
                                  "GridWidth", UintegerValue (5),//Number of node in row
                                  "LayoutType", StringValue ("RowFirst"));
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install (nodes);
  } else if (mType == 2) {
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
  }
  for(uint32_t i=0; i < nodes.GetN(); i++){
    Ptr<MobilityModel> m = nodes.Get(i)->GetObject<MobilityModel> ();
    Vector pos = m->GetPosition ();
    std::cout << "node: " << i << "(" << pos.x << "," << pos.y << ")" << std::endl;
  }
  //Simulator::Stop(Seconds (0.0));
  Simulator::Run ();
  Simulator::Destroy ();
}
