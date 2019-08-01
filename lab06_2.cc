#include <iostream>
#include <cmath>
#include "ns3/aodv-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/v4ping-helper.h"
#include "ns3/yans-wifi-helper.h"

using namespace ns3;

class MyApp : public Application
{
    public: 
        MyApp();
        virtual ~MyApp();
        void Setup(Ptr<Node>, Ipv4Address, Ipv4Address);

    private:
        virtual void StartApplication(void);
        virtual void StopApplication(void);
        void SendPacket (std::string);
        void ReceivePacket (Ptr<Socket>);
    private:
        Ptr<Node>       m_node;
        Ptr<Socket>     m_socket;
        Ipv4Address     m_address;
        Ipv4Address     m_targetAddress;
        uint16_t        m_port;
        EventId         m_event;
        bool            m_running;
};

MyApp::MyApp() : m_socket(0), m_port(8888), m_running (false) {

}
MyApp::~MyApp() {

}
void MyApp::Setup(Ptr<Node> node, Ipv4Address address, Ipv4Address targetAddress) {
    this->m_node = node;
    m_address = address;
    m_targetAddress = targetAddress;
}
void MyApp::StartApplication() {
    m_running = true;
    InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_port);
    
    m_socket = Socket::CreateSocket (m_node, UdpSocketFactory::GetTypeId ());
    m_socket->Bind (local);
    m_socket->SetAllowBroadcast(true);
    m_socket->SetRecvCallback (MakeCallback(&MyApp::ReceivePacket,this));

    m_event = Simulator::Schedule (Seconds(0), &MyApp::SendPacket, this, "Hello");
}
void MyApp::StopApplication() {
    m_running = false;
    if (m_event.IsRunning ()) {
        Simulator::Cancel (m_event);
    }
}
void MyApp::SendPacket(std::string payload) {
    std::cout << "Send" << std::endl;
    std::ostringstream msg; 
    uint16_t packetSize = msg.str().length()+1;
    Ptr<Packet> packet = Create<Packet>((uint8_t*) msg.str().c_str(), packetSize);
    m_socket->SendTo (packet, 0, InetSocketAddress (m_targetAddress, m_port));
}
void MyApp::ReceivePacket (Ptr<Socket> socket) {
    Ptr<Packet> packet = socket->Recv (std::numeric_limits<uint32_t>::max (), 0);
    std::cout << "Receive" << std::endl;
}
class MyLab 
{
public:
  MyLab ();
  /**
   * \brief Configure script parameters
   * \param argc is the command line argument count
   * \param argv is the command line arguments
   * \return true on successful configuration
  */
  bool Configure (int argc, char **argv);
  /// Run simulation
  void Run ();
  /**
   * Report results
   * \param os the output stream
   */
  void Report (std::ostream & os);

private:

  // parameters
  /// Number of nodes
  uint32_t size;
  /// Distance between nodes, meters
  double step;
  /// Simulation time, seconds
  double totalTime;
  /// Write per-device PCAP traces if true
  bool pcap;
  /// Print routes if true
  bool printRoutes;

  // network
  /// nodes used in the example
  NodeContainer nodes;
  /// devices used in the example
  NetDeviceContainer devices;
  /// interfaces used in the example
  Ipv4InterfaceContainer interfaces;

private:
  /// Create the nodes
  void CreateNodes ();
  /// Create the devices
  void CreateDevices ();
  /// Create the network
  void InstallInternetStack ();
  /// Create the simulation applications
  void InstallApplications ();
};

int main (int argc, char **argv)
{
  MyLab test;
  if (!test.Configure (argc, argv))
    NS_FATAL_ERROR ("Configuration failed. Aborted.");

  test.Run ();
  test.Report (std::cout);
  return 0;
}

//-----------------------------------------------------------------------------
MyLab::MyLab () :
  size (10),
  step (100),
  totalTime (100),
  pcap (true),
  printRoutes (true)
{
}

bool
MyLab::Configure (int argc, char **argv)
{
  // Enable AODV logs by default. Comment this if too noisy
  // LogComponentEnable("AodvRoutingProtocol", LOG_LEVEL_ALL);

  SeedManager::SetSeed (12345);
  CommandLine cmd;

  cmd.AddValue ("pcap", "Write PCAP traces.", pcap);
  cmd.AddValue ("printRoutes", "Print routing table dumps.", printRoutes);
  cmd.AddValue ("size", "Number of nodes.", size);
  cmd.AddValue ("time", "Simulation time, s.", totalTime);
  cmd.AddValue ("step", "Grid step, m", step);

  cmd.Parse (argc, argv);
  return true;
}

void
MyLab::Run ()
{
//  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", UintegerValue (1)); // enable rts cts all the time.
  CreateNodes ();
  CreateDevices ();
  InstallInternetStack ();
  InstallApplications ();

  std::cout << "Starting simulation for " << totalTime << " s ...\n";

  Simulator::Stop (Seconds (totalTime));
  Simulator::Run ();
  Simulator::Destroy ();
}

void
MyLab::Report (std::ostream &)
{ 
}

void
MyLab::CreateNodes ()
{
  std::cout << "Creating " << (unsigned)size << " nodes " << step << " m apart.\n";
  nodes.Create (size);
  // Name nodes
  for (uint32_t i = 0; i < size; ++i)
    {
      std::ostringstream os;
      os << "node-" << i;
      Names::Add (os.str (), nodes.Get (i));
    }
  // Create static grid
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (step),
                                 "DeltaY", DoubleValue (0),
                                 "GridWidth", UintegerValue (size),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);
}

void
MyLab::CreateDevices ()
{
  WifiMacHelper wifiMac;
  wifiMac.SetType ("ns3::AdhocWifiMac");
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  wifiPhy.SetChannel (wifiChannel.Create ());
  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("OfdmRate6Mbps"), "RtsCtsThreshold", UintegerValue (0));
  devices = wifi.Install (wifiPhy, wifiMac, nodes); 

  if (pcap)
    {
      wifiPhy.EnablePcapAll (std::string ("scratch/mylab"));
    }
}

void
MyLab::InstallInternetStack ()
{
  AodvHelper aodv;
  // you can configure AODV attributes here using aodv.Set(name, value)
  InternetStackHelper stack;
  stack.SetRoutingHelper (aodv); // has effect on the next Install ()
  stack.Install (nodes);
  Ipv4AddressHelper address;
  address.SetBase ("10.0.0.0", "255.0.0.0");
  interfaces = address.Assign (devices);

  if (printRoutes)
    {
      Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("aodv.routes", std::ios::out);
      aodv.PrintRoutingTableAllAt (Seconds (8), routingStream);
    }
}

void
MyLab::InstallApplications ()
{
  for(uint32_t i = 0; i < size; i++) {
    Ptr<MyApp> app = CreateObject<MyApp> ();
    app->Setup(nodes.Get(i), interfaces.GetAddress(i), interfaces.GetAddress(size-1));
    nodes.Get(i)->AddApplication (app);
    app->SetStartTime (Seconds (0));
    app->SetStopTime (Seconds (10));
  }
}
