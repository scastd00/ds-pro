#!/bin/bash

# Run 30 clients in background
# and wait for them to finish
for i in {1..30} ; do
    ../bin/mt_client.o 127.0.0.1 60001 &
done
wait
