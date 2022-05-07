# Getting Started
## Installation
1. Clone the repository

    `git clone https://github.com/Blackstaff/wsn_routing.git`

1. Clone modified INET (v 3.5) framework into the same workspace folder as wsn_routing

    `git clone -b wsn_routing_fixes --single-branch https://github.com/Blackstaff/inet.git inet`

## Compile and run (CMD)
1. Compile inet (run in inet folder):

        make makefiles
        make -j 8
    You can use -j [N] flag to parallelize compilation

2. Compile wsn_routing (run in wsn_routing folder):

        make makefiles
        make -j 8
3. Run simulation

        cd wsn_routing/simulations
        ../src/wsn_routing -m -u Qtenv -n "../src;.;../../inet/src" -l ../../inet/src/INET sim.ini
