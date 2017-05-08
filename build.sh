#!/bin/bash
g++ -o htcl main.cpp -std=c++11
# run sample1
./htcl sample1.csv

cat doc_src/readme1.part > README.md
cat sample1.csv >> README.md
cat doc_src/readme2.part >> README.md
cat htcl_cloud.html >> README.md
cat doc_src/readme3.part >> README.md
cat htcl_links.html >> README.md
cat doc_src/readme4.part >> README.md


