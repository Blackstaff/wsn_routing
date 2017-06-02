# Getting Started
## Installation
1. Clone the repository

    `git clone https://github.com/Blackstaff/wsn_routing.git`

1. Download INET framwork v3.5

    Download link: [Inet 3.5](https://github.com/inet-framework/inet/releases/download/v3.5.0/inet-3.5.0-src.tgz)

1. Extract INET into the same workspace folder as wsn_routing

1. Apply inet.patch from wsn_routing.

        cd inet
        patch -p 1 < ../wsn_routing/inet.patch 
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
        ../src/routing -m -u Qtenv -n "../src;.;../../inet/src" -l ../../inet/src/INET sim.ini
