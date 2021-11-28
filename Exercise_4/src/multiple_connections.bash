#!/bin/bash

for i in {1..90} ; do
    ../bin/mt_client.o 127.0.0.1 60001 &
done
wait