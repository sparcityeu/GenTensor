#!/bin/bash

./tengen 23776223 22476 22476 -d 2.36749E-09 -o ../data_tensors_random/1998darpa_rand.tns;
./tengen 6186 77 32 -d 3.49729E-01 -o ../data_tensors_random/chicago-crime-comm_rand.tns;
./tengen 6185 395 380 -d 6.81520E-03 -o ../data_tensors_random/chicago-crime-geo_rand.tns;
./tengen 244268 6066 5699 -d 6.41872E-06 -o ../data_tensors_random/enron_rand.tns;
./tengen 28153045 1607191 319686 -d 7.80441E-12 -o ../data_tensors_random/flickr-3d_rand.tns;
./tengen 28153045 1607191 319686 -d 7.80441E-12 -o ../data_tensors_random/flickr-4d_rand.tns;
./tengen 868131 4209 4198 -d 1.10750E-07 -o ../data_tensors_random/lbnl-network_rand.tns;
./tengen 28818 12092 9184 -d 2.40224E-05 -o ../data_tensors_random/nell2_rand.tns;
./tengen 14036 2862 2482 -d 3.11080E-05 -o ../data_tensors_random/nips_rand.tns;
./tengen 1717 1140 183 -d 9.23921E-03 -o ../data_tensors_random/uber_rand.tns;
./tengen 165427 11374 2 -d 6.91494E-03 -o ../data_tensors_random/vast-2015-mc1-3d_rand.tns;
./tengen 165427 11374 100 -d 1.38299E-04 -o ../data_tensors_random/vast-2015-mc1-5d_rand.tns;