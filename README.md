## Integrantes
Darien Parra Guzman<br>
Cristian Bello Cuesta<br>
Samuel Chaves Mora <br>
Santiago Ruiz Rangel<br>
# Laboratorio-de-simulacion-MESA

[1.) Simulación y Análisis de una Cola M/M/1/K/INF en NetLogo](DiscreteEventSimulationQueuesandServers.pdf)  <br><br>
[2.) Comparación entre Resultados Simulados y Teóricos bajo Diferentes Condiciones](DiscreteEventSimulationQueuesandServers.pdf)  <br><br>
[3.) Simulación de la cola M/M/1/K/INF (MESA)]<br><br>

## Simulación de Cola M/M/1 

```python
from mesa import Agent, Model
from mesa.time import SimultaneousActivation
from mesa.space import MultiGrid
from mesa.datacollection import DataCollector
from mesa.visualization.modules import ChartModule
from mesa.visualization.ModularVisualization import ModularServer
from mesa.visualization.UserParam import Slider, NumberInput
import random

# --- AGENTE DEL CLIENTE ---
class Client(Agent):
    def _init_(self, unique_id, model):
        super()._init_(unique_id, model)
        self.in_queue = True  # Indica si el cliente está en la cola

    def step(self):
        pass  # Los clientes no hacen nada, solo esperan en la cola


# --- AGENTE DEL SERVIDOR ---
class Server(Agent):
    def _init_(self, unique_id, model):
        super()._init_(unique_id, model)
        self.busy = False
        self.service_time_left = 0

    def step(self):
        if self.busy:
            self.service_time_left -= 1
            if self.service_time_left <= 0:
                self.busy = False

        # Si no está ocupado y hay clientes en la cola, atender a uno
        if not self.busy and len(self.model.queue) > 0:
            self.model.queue.pop(0)
            self.busy = True
            self.service_time_left = int(random.expovariate(1 / self.model.mean_service_time))


# --- MODELO ---
class QueueModel(Model):
    def _init_(self, num_servers=1, mean_arrival_rate=1.0, mean_service_time=1.0, max_run_time=1000):
        self.num_servers = num_servers
        self.mean_arrival_rate = mean_arrival_rate
        self.mean_service_time = mean_service_time
        self.max_run_time = max_run_time
        self.current_time = 0

        self.schedule = SimultaneousActivation(self)
        self.grid = MultiGrid(10, 10, False)  # Grilla solo para visualización
        self.queue = []

        # Crear servidores
        for i in range(self.num_servers):
            server = Server(i, self)
            self.schedule.add(server)

        # Recolectar datos
        self.datacollector = DataCollector(
            {
                "Queue Length": lambda m: len(m.queue),
                "Server Utilization": lambda m: sum([1 for s in m.schedule.agents if s.busy]) / len(m.schedule.agents),
            }
        )

    def step(self):
        self.current_time += 1

        # Llegada de nuevos clientes según la tasa de llegada
        if random.random() < self.mean_arrival_rate:
            new_client = Client(self.current_time, self)
            self.queue.append(new_client)

        self.schedule.step()
        self.datacollector.collect(self)

        # Terminar la simulación después de max_run_time
        if self.current_time >= self.max_run_time:
            self.running = False


# --- CONFIGURAR EL SERVIDOR ---
def agent_portrayal(agent):
    if isinstance(agent, Server):
        portrayal = {"Shape": "circle", "Filled": "true", "r": 1, "Color": "red" if agent.busy else "green"}
    else:
        portrayal = {"Shape": "rect", "w": 1, "h": 1, "Color": "blue"}
    return portrayal

grid = MultiGrid(10, 10, False)
chart = ChartModule([{"Label": "Queue Length", "Color": "Black"}, {"Label": "Server Utilization", "Color": "Red"}])

model_params = {
    "num_servers": Slider("Number of servers", 1, 1, 5, 1),
    "mean_arrival_rate": Slider("Mean arrival rate", 1.0, 0.1, 5.0, 0.1),
    "mean_service_time": Slider("Mean service time", 1.0, 0.1, 5.0, 0.1),
    "max_run_time": NumberInput("Max run time", value=1000),
}

server = ModularServer(
    QueueModel,
    [grid, chart],
    "Queue Simulation",
    model_params
)
server.port = 8521  # Puerto por defecto
server.launch()
```
# Simulación de Red con NS-3: Eco UDP

Este proyecto es una simulación de red en *NS-3* que implementa una comunicación UDP sencilla entre dos nodos, uno actuando como cliente y el otro como servidor. La simulación está diseñada para modelar una interacción básica de red y observar el funcionamiento del protocolo UDP en un entorno controlado.

## Descripción de la Simulación

La simulación consta de los siguientes pasos:

1. *Creación de Nodos*: Se crean dos nodos, uno para el cliente y otro para el servidor.
2. *Configuración del Enlace*: Se establece un enlace punto a punto entre ambos nodos, con una velocidad de 5 Mbps y una latencia de 2 ms.
3. *Asignación de Direcciones IP*: Se asignan direcciones IP a los nodos para permitir la comunicación entre ellos.
4. *Instalación del Servidor UDP*: El servidor se configura para escuchar en el puerto 9 y responde a los paquetes recibidos (efecto de "eco").
5. *Configuración del Cliente UDP*: El cliente envía tres paquetes de 1024 bytes al servidor en intervalos de 1 segundo, creando una interacción simple de envío y respuesta.

## Requisitos

- *NS-3*: Esta simulación requiere el simulador NS-3. Asegúrate de tenerlo instalado en tu sistema para ejecutar el programa.

## Ejecución

Para ejecutar la simulación, compila y ejecuta el programa dentro del entorno NS-3. Al iniciar, verás mensajes de log que muestran el proceso de envío y respuesta entre el cliente y el servidor.

## Propósito

Este proyecto es útil para:
- Estudiar el comportamiento básico de una red UDP en un ambiente controlado.
- Analizar tiempos de latencia y rendimiento de una conexión simple punto a punto.
- Desarrollar habilidades en simulación de redes utilizando NS-3.

## Créditos

Este programa fue creado para demostrar el funcionamiento del protocolo UDP en una simulación de red sencilla utilizando NS-3.


<br> [4.) Paradigma de programación por eventos](ParadigmaDeProgramacionPorEventos.pdf)
## Simulación de un Enlace Simple: Análisis del Paradigma de Programación por Eventos
```c++

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

