#!/bin/bash

echo "== SERIAL =="
./pi

echo ""
echo "== DATA PARALLEL =="
./dpi --dataParTasksPerLocale=1
./dpi --dataParTasksPerLocale=2
./dpi --dataParTasksPerLocale=4
./dpi --dataParTasksPerLocale=8
./dpi --dataParTasksPerLocale=16
./dpi --dataParTasksPerLocale=32

echo ""
echo "== TASK PARALLEL =="
./tpi --numThreads=1
./tpi --numThreads=2
./tpi --numThreads=4
./tpi --numThreads=8
./tpi --numThreads=16
./tpi --numThreads=32

