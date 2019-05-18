#!/bin/bash
set -m
echo "nodes\tclientcount\tclienttime\tservercount\tservertime\n" > devicebench.dat
rm server.dat
rm client.dat
rm count.dat

COUNTS=( 10 2500 5000 7500 10000 12500 15000 17500 20000 22500 25000 27500 30000 ) # past 30k won't work due to wspp problems: 40000 50000 60000 70000 80000 100000 )
for VAL in "${COUNTS[@]}"
do
    echo $VAL >> count.dat

    echo "START SERVER $VAL"
    ./ossia_DeviceBenchmark_Nsec_server $VAL | grep Received | awk '{ print $2 "\t" $5 }' >> server.dat &

    sleep 1

    echo "START CLIENT $VAL"
    ./ossia_DeviceBenchmark_Nsec_client | grep Sent | awk '{ print $2 "\t" $5 }' >> client.dat

    echo "STOP CLIENT $VAL"
    jobs
    fg 1
    echo "STOP SERVER $VAL"
    sleep 5
    pkill ossia_Device
    sleep 5

done

paste count.dat client.dat server.dat >> devicebench.dat
