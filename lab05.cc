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
#include "ns3/v4ping-helper.h"

using namespace ns3;

uint32_t MacTxDropCount, PhyTxDropCount, PhyRxDropCount;
void MacTxDrop(std::string path, Ptr<const Packet> p) {
    MacTxDropCount++;
}
void PhyTxDrop(std::string path, Ptr<const Packet> p) {
    PhyTxDropCount++;
}
void PhyRxDrop(std::string path, Ptr<const Packet> p) {
    PhyRxDropCount++;
}

int main (int argc, char *argv[])
{
  NodeContainer nodes;
  nodes.Create (5);

  MobilityHelper mobility; 
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                "MinX", DoubleValue (0.0),
                                "MinY", DoubleValue (0.0),
                                "DeltaX", DoubleValue (100),
                                "DeltaY", DoubleValue (100),
                                "GridWidth", UintegerValue (5),//Number of node in row
                                "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);



  //Option WIFI_PHY_STANDARD_80211a, WIFI_PHY_STANDARD_80211b, WIFI_PHY_STANDARD_80211g, WIFI_PHY_STANDARD_80211_10MHZ, WIFI_PHY_STANDARD_80211_5MHZ,WIFI_PHY_STANDARD_holland, WIFI_PHY_STANDARD_80211n_2_4GHZ, WIFI_PHY_STANDARD_80211n_5GHZ, WIFI_PHY_STANDARD_80211ac, WIFI_PHY_STANDARD_80211ax_2_4GHZ, WIFI_PHY_STANDARD_80211ax_5GHZ, WIFI_PHY_STANDARD_UNSPECIFIED:
  NetDeviceContainer devices;
  WifiMacHelper wifiMac;
  wifiMac.SetType ("ns3::AdhocWifiMac");
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  wifiPhy.SetChannel (wifiChannel.Create ());
  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("OfdmRate6Mbps"), "RtsCtsThreshold", UintegerValue (0));
  devices = wifi.Install (wifiPhy, wifiMac, nodes); 
  
  //Create Internet stack + routing algorithm
  InternetStackHelper internet;
  AodvHelper aodv;
  internet.SetRoutingHelper (aodv);
  
  /*
  OlsrHelper olsr;
  internet.SetRoutingHelper (olsr);
  */
  internet.Install (nodes);
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer interfaces = ipv4.Assign (devices);


  V4PingHelper ping (interfaces.GetAddress (4));
  ping.SetAttribute ("Verbose", BooleanValue (true));

  ApplicationContainer p = ping.Install (nodes.Get (0));
  p.Start (Seconds (0));
  p.Stop (Seconds (50));

  Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("scratch/aodv.routes", std::ios::out);
  aodv.PrintRoutingTableAllAt (Seconds (5), routingStream);
  wifiPhy.EnablePcapAll (std::string ("scratch/aodv"));
  

  for(uint32_t i=0; i < nodes.GetN(); i++){
    Ptr<MobilityModel> m = nodes.Get(i)->GetObject<MobilityModel> ();
    Vector pos = m->GetPosition ();
    std::cout << "node: " << i << "(" << pos.x << "," << pos.y << ")" << std::endl;
  }

  Config::Connect("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTxDrop", MakeCallback(&MacTxDrop));
  Config::Connect("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxDrop", MakeCallback(&PhyRxDrop));
  Config::Connect("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxDrop", MakeCallback(&PhyTxDrop));

  Simulator::Stop( Seconds(100));
  Simulator::Run ();
  Simulator::Destroy ();


  std::cout << "MacTxDropCount: " << MacTxDropCount << std::endl;
  std::cout << "PhyTxDropCount: " << PhyTxDropCount << std::endl;
  std::cout << "PhyRxDropCount: " << PhyRxDropCount << std::endl;
}
