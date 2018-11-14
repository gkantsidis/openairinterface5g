#!/bin/bash

# Sidelink interface configuration
sudo ifconfig oip0 10.0.0.1

sudo ifconfig oip0 hw ether 00:00:00:00:00:01

# Uu interface configuration for the message exchanges between the eNB and the UE based on loopback interface 
sudo ifconfig lo: 127.0.0.2 netmask 255.0.0.0 up

# Routing and SLRB configuration for specific sidelink (10.0.0.2) and external(e.g., 8.8.8.8) destination addresses.
sudo ip neigh add 10.0.0.2 lladdr 00:00:00:00:00:02 dev oip0 nud permanent

sudo iptables -A POSTROUTING  -t mangle -o oip0 -d 10.0.0.2 -j MARK --set-mark 4

sudo ip route add 8.8.8.8 dev oip1

# Applying NAT so that the Remote UE originating/destined traffic does not get blocked at the PGW.
sudo iptables -t nat -A POSTROUTING -o oip1 -j MASQUERADE



