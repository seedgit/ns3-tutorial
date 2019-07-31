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
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                "MinX", DoubleValue (0.0),
                                "MinY", DoubleValue (0.0),
                                "DeltaX", DoubleValue (10),
                                "DeltaY", DoubleValue (10),
                                "GridWidth", UintegerValue (5),//Number of node in row
                                "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  for(uint32_t i=0; i < nodes.GetN(); i++){
    Ptr<MobilityModel> m = nodes.Get(i)->GetObject<MobilityModel> ();
    Vector pos = m->GetPosition ();
    std::cout << "node: " << i << "(" << pos.x << "," << pos.y << ")" << std::endl;
  }

  //Option WIFI_PHY_STANDARD_80211a, WIFI_PHY_STANDARD_80211b, WIFI_PHY_STANDARD_80211g, WIFI_PHY_STANDARD_80211_10MHZ, WIFI_PHY_STANDARD_80211_5MHZ,WIFI_PHY_STANDARD_holland, WIFI_PHY_STANDARD_80211n_2_4GHZ, WIFI_PHY_STANDARD_80211n_5GHZ, WIFI_PHY_STANDARD_80211ac, WIFI_PHY_STANDARD_80211ax_2_4GHZ, WIFI_PHY_STANDARD_80211ax_5GHZ, WIFI_PHY_STANDARD_UNSPECIFIED:
  std::string phyMode = "OfdmRate54Mbps";//DsssRate11Mbps
  WifiHelper wifi;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211g);
  WifiMacHelper wifiMac;
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  wifiMac.SetType ("ns3::AdhocWifiMac");
  
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager"
                                , "DataMode", StringValue (phyMode)
                                , "ControlMode", StringValue(phyMode)
                                );
  //wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel", "MaxRange", DoubleValue (100.0));
  wifiPhy.SetChannel (wifiChannel.Create ());
  NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, nodes);
  //Create Internet stack + routing algorithm
  InternetStackHelper internet;

  /*
  AodvHelper aodv;
  internet.SetRoutingHelper (aodv);
  */
  /*
  OlsrHelper olsr;
  internet.SetRoutingHelper (olsr);
  */
  internet.Install (nodes);
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer interfaces = ipv4.Assign (devices);
  

  Simulator::Run ();
  Simulator::Destroy ();
}