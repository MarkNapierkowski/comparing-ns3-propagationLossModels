/*
Author: Mark Napierkowski
Purpose: compare five different Propagation Loss Models in NS3
*/

// Includes needed for this project
#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/double.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/log.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/string.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/udp-client-server-helper.h"

// Including Flow monitor

#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"

// Includes for csv-file
#include <iostream>
#include <fstream>
#include <string> 
/*
Default Network Topologie
  Wifi 0.0.0.0

     *        *
     |        |
    n0       n1

*/

// Namespace declaration
using namespace ns3;

double rssv = 0;  //defining the signal strength variable
double d = 1; //defining the distance variable

bool models[5]={false}; //defining bool array for Model-choice
int model; //defining variable for cmd-input

//Callback-function for signalstrength and throughput meassurements 
static void PhyTrace(Ptr<const Packet> packet,
                     uint16_t channelFreqMhz,
                     WifiTxVector txVector,
                     MpduInfo aMpdu,
                     SignalNoiseDbm signalNoise,
                     uint16_t staId)

{
  //Log Components for Signal strength
  // NS_LOG_UNCOND ("Packet has been received with Signal: " << signalNoise.signal <<" dBm");
  // std::cout << "Packet has been received with Signal:"<< signalNoise.signal << " dBm" <<std::endl;
  rssv = signalNoise.signal;
}
// Define Log Component
NS_LOG_COMPONENT_DEFINE("MyProject");

// Main-function
int main(int argc, char *argv[])
{

  //enable Commandline Input 
  CommandLine cmd;
  cmd.AddValue ("model",  "Modelnumber", model);
  cmd.Parse (argc, argv);

  //set the specific bool argument for the chosen Model
  models[model] = true;

  // setting the Time resolution to nanoseconds; has to be outside because it can called only once
  Time::SetResolution(Time::NS);

  // creating the output csv file
  std::ofstream myfile;
  std::string s = "Output" + std::to_string(model) + ".csv";
  myfile.open(s);
  myfile << "distanceInM,RecievedPackets,RSSindBm,DatarateinMbit/s,";
  myfile.close();
  
  //defining important simulation parameters
  double interval = 0.0001545; // Time intervalls provides with 75Mbps UDP-Traffic
  double simulationTime = 50;  // Simulation Time after which the Results doesn't change radically  50
  double throughput = 1; // define the throughput variable


  //write the name of the Model in the .csv for better understanding
  if (models[0])
    {
      std::cout << "FriisPropagationLossModel:" << std::endl;
      myfile.open(s, std::ios::app);
      myfile << "FriisPropagationLossModel\n";
      myfile.close();
    }
    if (models[1])
    {
      std::cout << "FixedRssLossModel:" << std::endl; 
      myfile.open(s, std::ios::app);
      myfile << "FixedRssLossModel\n";
      myfile.close();
    }
    if (models[2])
    {
      std::cout << "ThreeLogDistancePropagationLossModel:" << std::endl; 
      myfile.open(s, std::ios::app);
      myfile << "ThreeLogDistancePropagationLossModel\n";
      myfile.close();
    }
    if (models[3])
    {
      std::cout << "TwoRayGroundPropagationLossModel:" << std::endl; 
      myfile.open(s, std::ios::app);
      myfile << "TwoRayGroundPropagationLossModel\n";
      myfile.close();
    }
    if (models[4])
    {
      std::cout << "NakagamiPropagationLossModel:" << std::endl;
      myfile.open(s, std::ios::app);
      myfile << "TNakagamiPropagationLossModel\n";
      myfile.close(); 
    }

  


  for (double d = 1; throughput != 0;) // set the distance to 1m and stop the loop if the throughput is 0 or distance is higher than 300 (for fixed & Nakagami)
  {
    //distance
    //std::cout << "Distance = " << d << "m : " << std::endl;   //testoutput to see the distance between the nodes
    //SimulationTime
    //std::cout << "Time = " << d << "s: " << std::endl;     //Output used for the optimal simulation time definition

    // Convert interval to time object
    Time interPacketInterval = Seconds(interval);

    // Topology Helpers
    NodeContainer nodes;
    nodes.Create(2); // create the 2 nodes for this experiment

    // Internet stack helper
    InternetStackHelper stack;
    stack.Install(nodes);

    // Adress helper to set the adress range
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    // define the wifi helper and set the standard with 5Ghz
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211n);

    // define YansWifiPhy helper, helper for physcal layer
    YansWifiPhyHelper wifiPhy;
    wifiPhy.Set("TxPowerStart", DoubleValue(10.0)); // set the output power to 10dBm for the Transmitter
    wifiPhy.Set("TxPowerEnd", DoubleValue(10.0));   // set the output power to 10dBm for the Reciever
    wifiPhy.Set("RxGain", DoubleValue(1));          // set Antenna gain for the reciever
    wifiPhy.Set("TxGain", DoubleValue(1));          // set Antenna gain for the transmitter
    wifiPhy.Set("ChannelSettings", StringValue("{0, 40, BAND_5GHZ, 0}")); //set the channel settings

    // YansWifiChannelhelper
    YansWifiChannelHelper wifiChannel;
    // set propagationDelay Model
    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    double z1 = 0;
    double z2 = 0;

    // set propagation loss Model
    if (models[0])
    {
      wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel", "Frequency", DoubleValue(5e9)); //Setting FriisPropagationLossModel for the Frequenzy 5GHz
    }
    if (models[1])
    {
      double rss = -80; // -dBm
      wifiChannel.AddPropagationLoss("ns3::FixedRssLossModel", "Rss", DoubleValue(rss)); //set the fixed Model with a Value of -80 dBm
    }
    if (models[2])
    {
      wifiChannel.AddPropagationLoss("ns3::ThreeLogDistancePropagationLossModel", "Distance0", DoubleValue(1.0), "Distance1", DoubleValue(200.0),"Distance2", DoubleValue(500.0), "ReferenceLoss", DoubleValue(46.77));
    }
    if (models[3])
    {
      wifiChannel.AddPropagationLoss("ns3::TwoRayGroundPropagationLossModel", "Frequency", DoubleValue(5e9), "HeightAboveZ", DoubleValue(1), "MinDistance", DoubleValue(0.5),"SystemLoss", DoubleValue(1)); 
      //setting height variables to set a suitable scenario for this Loss-Model
      z1 = 1;
      z2 = 1;
    }
    if (models[4])
    {
      wifiChannel.AddPropagationLoss("ns3::NakagamiPropagationLossModel", "Distance1", DoubleValue(80.0),"Distance2", DoubleValue(200.0),"m0", DoubleValue(1.5),"m1", DoubleValue(0.75),"m2", DoubleValue(0.75));
    }

    // set Channel
    wifiPhy.SetChannel(wifiChannel.Create());

    // Add a mac 
    WifiMacHelper wifiMac;

    // Set it to adhoc mode
    wifiMac.SetType("ns3::AdhocWifiMac");

    // set mobility helper for the position of the nodes
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, z1));
    positionAlloc->Add(Vector(d, 0.0, z2));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    // installing Wifi-Model to devices
    NetDeviceContainer serverDevice = wifi.Install(wifiPhy, wifiMac, nodes.Get(0));
    NetDeviceContainer clientDevice = wifi.Install(wifiPhy, wifiMac, nodes.Get(1));

    // IP-Adress helper
    Ipv4AddressHelper ipv4;
    // Loginfodefinition
    NS_LOG_INFO("Assign IP Addresses.");
    // set IP-Range, starting from '10.1.1.0' with mask '255.255.255.0'
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    // ceate Interfaces for server & client
    Ipv4InterfaceContainer serverInterface;
    Ipv4InterfaceContainer clientInterface;

    // Assign Adresses to devices
    serverInterface = address.Assign(serverDevice);
    clientInterface = address.Assign(clientDevice);

    // Creating the UDP-Server on Node 1
    NS_LOG_INFO("Create UdpServer application on node 1.");
    ApplicationContainer serverApp;           // declaration server Application Contaienr
    uint16_t port = 9;                        // defining the recieving Port on the server
    UdpServerHelper server(port);             // creating the server with the UdpServerHelper
    serverApp = server.Install(nodes.Get(0)); // install the server on Node 0
    serverApp.Start(Seconds(1.0));            // set the start-Time
    serverApp.Stop(Seconds(simulationTime));  // set the stop-Time atthe end of the simulation Time

    Ptr<UdpServer> serverPtr = server.GetServer(); // get the pointer to the Server
    // std::cout << "serverPtr: " << serverPtr << std::endl; // Testouput if the Server exists

    Address Serveraddr;                                  // declaration of a Serveradress
    Serveraddr = Address(serverInterface.GetAddress(0)); // save the Serveradress
    // std::cout << "Server (0) Adress: " << Serveraddr << std::endl; // Testoutput

    uint32_t maxPacketCount = 60000000; // Variable maximal packets sent by the UDP-Client
    uint32_t MaxPacketSize = 1450;      // Variable packet size

    UdpClientHelper client(Serveraddr, port); // creating the client with the DESTINATION adress and port
    // setting the important Attributes
    client.SetAttribute("MaxPackets", UintegerValue(maxPacketCount));
    client.SetAttribute("Interval", TimeValue(interPacketInterval));
    client.SetAttribute("PacketSize", UintegerValue(MaxPacketSize));

    ApplicationContainer clientApp = client.Install(nodes.Get(1)); // install the client on Node 1
    clientApp.Start(Seconds(2.0));                                 // set the start-Time
    clientApp.Stop(Seconds(simulationTime));                       // set the stop-Time atthe end of the simulation Time

    Address Clientaddr;                                  // declaration of a Clientadress
    Clientaddr = Address(clientInterface.GetAddress(0)); // save the Clientadress
    // std::cout << "Client (1) Adress: " << Clientaddr << std::endl; // Testoutput

    // omnidirectional antenna on both nodes since no directional Antennas are supported by ns3

    // Flow monitor
    Ptr<FlowMonitor> flowMonitor;
    FlowMonitorHelper flowHelper;
    flowMonitor = flowHelper.InstallAll();

    Config::ConnectWithoutContext("/NodeList/0/DeviceList/1/$ns3::WifiNetDevice/Phy/MonitorSnifferRx", MakeCallback(&PhyTrace));

    Simulator::Stop(Seconds(simulationTime + 1)); // define the definitive termination of the Simulation
    Simulator::Run();                             // start the simulation

    
    // accesing the udp server to get all Packets that went through
    uint64_t totalPacketsThrough = DynamicCast<UdpServer>(serverPtr)->GetReceived(); // Get the total recieved packets by the UDP-Server
    // uint64_t lostPackets = DynamicCast<UdpServer>(serverPtr)->GetLost();          // Get the total lost packets in the process
    throughput = totalPacketsThrough * MaxPacketSize * 8 / (simulationTime * 1000000.0); // calculate the Datarate Mbit/s

    // std::cout << " Lost Packets:" << lostPackets << std::endl;                                // Output of total packets lost
    //std::cout << " Total Packets through:" << totalPacketsThrough << " Packets" << std::endl; // Output of total packets recieved
    //std::cout << " throughpout:" << throughput << " Mbit/s" << std::endl;                     // Output of total revieved datarate
    //std::cout << " Received Signal strength (RSS):" << rssv << " dBm" << std::endl;           // Output of recieved Signal strength
                                                                                              // increment of the distance

    flowMonitor->SerializeToXmlFile("flow.xml", true, true); // writing Flow-Monitor-File
    Simulator::Destroy();                                    // Terminate the Simulation

    //write the meassurements in the csv file
    myfile.open(s, std::ios::app);
    myfile << d << "," << totalPacketsThrough << "," << rssv << "," << throughput << "\n";
    myfile.close();

    //output to the console 
    std::cout << "Mode:" << model << ","<< "Distance: " << d << "," << totalPacketsThrough << "," << rssv << "," << throughput << std::endl;

    //increase the distance variable for the next step
    d += 1;
  }
  std::cout << "End of Simulation with model"<< model << std::endl;
}
