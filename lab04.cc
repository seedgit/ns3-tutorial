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

static void CourseChangeV1 (std::string path, Ptr<const MobilityModel> mobility) {
   Vector pos = mobility->GetPosition ();
   std::cout << "V1," << Simulator::Now().GetSeconds () << ", x:" << pos.x << ", y=" << pos.y << std::endl;
}

static void CourseChangeV2 (std::string ext, std::string path, Ptr<const MobilityModel> mobility) {
   Vector pos = mobility->GetPosition ();
   std::cout << "V2," << ext << ":" << Simulator::Now().GetSeconds () << ", x:" << pos.x << ", y=" << pos.y << std::endl;
}

int main (int argc, char *argv[])
{
  NodeContainer nodes;
  nodes.Create (10);
  
  MobilityHelper mobility;
  ObjectFactory pos;
  pos.SetTypeId ("ns3::RandomRectanglePositionAllocator");
  pos.Set ("X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1000.0]"));
  pos.Set ("Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1000.0]"));
  Ptr<PositionAllocator> taPositionAlloc = pos.Create ()->GetObject<PositionAllocator> ();

  mobility.SetMobilityModel ("ns3::RandomWaypointMobilityModel",
                                "Speed", StringValue ("ns3::UniformRandomVariable[Min=10.0|Max=50.0]"),
                                "Pause", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                                "PositionAllocator", PointerValue (taPositionAlloc));

  mobility.Install (nodes);
  
  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange", MakeCallback (&CourseChangeV1));
  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange", MakeBoundCallback (&CourseChangeV2, "Hello"));
  
  Simulator::Stop(Seconds(100));
  Simulator::Run ();
  Simulator::Destroy ();
}
