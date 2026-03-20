#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/yans-wifi-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("cs169-wifi-power-control");

// This Callback function will be triggered whenever transmit power is changed
void
PowerCallback(std::string path, double oldPower, double newPower, Mac48Address dest)
{
    NS_LOG_UNCOND("Time: " << Simulator::Now().GetSeconds() << "s, Path: " << path
                           << ", Old Power: " << oldPower << " dBm, New Power: " << newPower
                           << " dBm, Destination: " << dest);
}

int
main(int argc, char* argv[])
{
    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(1);

    NodeContainer wifiApNodes;
    wifiApNodes.Create(1);

    // Configure wifi manager
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211a);
    WifiMacHelper wifiMac;
    YansWifiPhyHelper wifiPhy;
    YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();

    wifiPhy.SetChannel(wifiChannel.Create());

    NetDeviceContainer wifiApDevices;
    NetDeviceContainer wifiStaDevices;

    // Configure the STA node
    dBm_u maxPower{30};
    dBm_u minPower{0};
    uint32_t powerLevels{31};
    uint32_t rtsThreshold{2346};
    std::string manager{"ns3::ParfWifiManager"};

    wifi.SetRemoteStationManager("ns3::MinstrelWifiManager",
                                 "RtsCtsThreshold",
                                 UintegerValue(rtsThreshold));
    wifiPhy.Set("TxPowerStart", DoubleValue(maxPower));
    wifiPhy.Set("TxPowerEnd", DoubleValue(maxPower));

    Ssid ssid = Ssid("AP");
    wifiMac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));
    wifiStaDevices.Add(wifi.Install(wifiPhy, wifiMac, wifiStaNodes.Get(0)));

    // Configure the AP node
    wifi.SetRemoteStationManager(manager,
                                 "DefaultTxPowerLevel",
                                 UintegerValue(powerLevels - 1),
                                 "RtsCtsThreshold",
                                 UintegerValue(rtsThreshold));
    wifiPhy.Set("TxPowerStart", DoubleValue(minPower));
    wifiPhy.Set("TxPowerEnd", DoubleValue(maxPower));
    wifiPhy.Set("TxPowerLevels", UintegerValue(powerLevels));

    ssid = Ssid("AP");
    wifiMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    wifiApDevices.Add(wifi.Install(wifiPhy, wifiMac, wifiApNodes));

    // Mobility
    MobilityHelper mobility;

    // Stationary AP
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiApNodes);
    Ptr<MobilityModel> apMobility = wifiApNodes.Get(0)->GetObject<MobilityModel>();
    apMobility->SetPosition(Vector(0.0, 0.0, 0.0));

    // Mobile station
    mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    mobility.Install(wifiStaNodes);
    Ptr<MobilityModel> staMobility = wifiStaNodes.Get(0)->GetObject<MobilityModel>();
    staMobility->SetPosition(Vector(5.0, 0.0, 1.0));

    Ptr<ConstantVelocityMobilityModel> cvmm =
        wifiStaNodes.Get(0)->GetObject<ConstantVelocityMobilityModel>();
    cvmm->SetVelocity(Vector(8.0, 0.0, 0.0));

    // Internet stack
    InternetStackHelper stack;
    stack.Install(wifiApNodes);
    stack.Install(wifiStaNodes);

    Ipv4AddressHelper address;
    address.SetBase("10.0.0.0", "255.255.255.0");
    Ipv4InterfaceContainer staInterfaces = address.Assign(wifiStaDevices);
    Ipv4InterfaceContainer apInterfaces = address.Assign(wifiApDevices);

    // Set up UDP application
    uint16_t port = 50000;
    Ipv4Address sinkAddress = staInterfaces.GetAddress(0);
    PacketSinkHelper sink("ns3::UdpSocketFactory", InetSocketAddress(sinkAddress, port));
    ApplicationContainer apps_sink = sink.Install(wifiStaNodes.Get(0));

    OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress(sinkAddress, port + 1));
    onoff.SetConstantRate(DataRate("54Mb/s"), 1024);
    onoff.SetAttribute("StartTime", TimeValue(Seconds(1)));
    onoff.SetAttribute("StopTime", TimeValue(Seconds(10)));
    ApplicationContainer apps_source = onoff.Install(wifiApNodes.Get(0));

    apps_source.Start(Seconds(1.0));
    apps_source.Stop(Seconds(10));

    apps_sink.Start(Seconds(0.5));
    apps_sink.Stop(Seconds(10));

    // Enable pcap tracing
    stack.EnablePcapIpv4All("wifi");

    // Log transmit power
    Config::Connect("/NodeList/*/DeviceList/*/RemoteStationManager/PowerChange",
                    MakeCallback(&PowerCallback));

    // Simulation
    Simulator::Stop(Seconds(10.0));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}