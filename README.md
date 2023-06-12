# comparing-ns3-propagationLossModels
provides the NS-3 c++ source code for a comparison of 5 different Propagation-Loss-Models in a simple mobile Adhoc WiFi-Network. It meassures the recieving signal strength and UDP-throughput for varying distances.

start the simulation with
"./ns3 run "comparing-ns3-propagationLossModels.cc --models=<N>" 
N specifies the model number you want to test in the simulation.

0 = FriisPropagationLossModel
1 = FixedRssLossModel
2 = ThreeLogDistancePropagationLossModel
3 = TwoRayGroundPropagationLossModel
4 = NakagamiPropagationLossModel
