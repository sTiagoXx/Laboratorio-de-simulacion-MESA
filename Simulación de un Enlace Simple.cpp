
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;


NS_LOG_COMPONENT_DEFINE ("SimulacionEcoUDP");

int main (int argc, char *argv[])
{
   
    LogComponentEnable ("SimulacionEcoUDP", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

    Time::SetResolution (Time::NS);

    NS_LOG_INFO ("Iniciando simulación de Eco UDP...");
    NS_LOG_INFO ("Creando 2 nodos: cliente y servidor...");
    NodeContainer nodos;
    nodos.Create (2);

    PointToPointHelper configEnlace;
    configEnlace.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    configEnlace.SetChannelAttribute ("Delay", StringValue ("2ms"));

    NetDeviceContainer dispositivos;
    dispositivos = configEnlace.Install (nodos);

    InternetStackHelper pilaDeProtocolos;
    pilaDeProtocolos.Install (nodos);

    Ipv4AddressHelper direccionador;
    direccionador.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = direccionador.Assign (dispositivos);

    NS_LOG_INFO ("Configurando servidor UDP en el puerto 9");
    UdpEchoServerHelper servidor (9);
    ApplicationContainer appsServidor = servidor.Install (nodos.Get (1));
    appsServidor.Start (Seconds (1.0));
    appsServidor.Stop (Seconds (10.0));

    NS_LOG_INFO ("Configurando cliente UDP para enviar 3 paquetes");
    UdpEchoClientHelper cliente (interfaces.GetAddress (1), 9);
    cliente.SetAttribute ("MaxPackets", UintegerValue (3));
    cliente.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
    cliente.SetAttribute ("PacketSize", UintegerValue (1024));

    ApplicationContainer appsCliente = cliente.Install (nodos.Get (0));
    appsCliente.Start (Seconds (2.0));
    appsCliente.Stop (Seconds (10.0));

    NS_LOG_INFO ("Iniciando simulación de eco UDP...");
    Simulator::Run ();
    Simulator::Destroy ();
    NS_LOG_INFO ("Simulación de eco UDP completada exitosamente.");

    return 0;
}

