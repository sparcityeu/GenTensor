#!/bin/bash

# ./tengen_naive_rand 23776223 22476 22476 -d 2.36749E-09 -o ../data_tensors_random/1998darpa_rand.tns;
# ./tengen_naive_rand 6186 77 32 -d 3.49729E-01 -o ../data_tensors_random/chicago-crime-comm_rand.tns;
# ./tengen_naive_rand 6185 395 380 -d 6.81520E-03 -o ../data_tensors_random/chicago-crime-geo_rand.tns;
# ./tengen_naive_rand 244268 6066 5699 -d 6.41872E-06 -o ../data_tensors_random/enron_rand.tns;
# ./tengen_naive_rand 28153045 1607191 319686 -d 7.80441E-12 -o ../data_tensors_random/flickr-3d_rand.tns;
# ./tengen_naive_rand 28153045 1607191 319686 -d 7.80441E-12 -o ../data_tensors_random/flickr-4d_rand.tns;
# ./tengen_naive_rand 868131 4209 4198 -d 1.10750E-07 -o ../data_tensors_random/lbnl-network_rand.tns;
# ./tengen_naive_rand 28818 12092 9184 -d 2.40224E-05 -o ../data_tensors_random/nell2_rand.tns;
# ./tengen_naive_rand 14036 2862 2482 -d 3.11080E-05 -o ../data_tensors_random/nips_rand.tns;
# ./tengen_naive_rand 1717 1140 183 -d 9.23921E-03 -o ../data_tensors_random/uber_rand.tns;
# ./tengen_naive_rand 165427 11374 2 -d 6.91494E-03 -o ../data_tensors_random/vast-2015-mc1-3d_rand.tns;
# ./tengen_naive_rand 165427 11374 100 -d 1.38299E-04 -o ../data_tensors_random/vast-2015-mc1-5d_rand.tns;

./tengen_naive_rand 22476 22476 23776223 -d 2.36749E-09 -o ../data_tensors_random/1998darpa.tns;
./tengen_naive_rand 6186 24 77 -i 32 -d 3.49729E-01 -o ../data_tensors_random/chicago-crime-comm.tns;  
./tengen_naive_rand 6185 24 380 -i 395 -j 32 -d 6.81520E-03 -o ../data_tensors_random/chicago-crime-geo.tns; 
./tengen_naive_rand 6066 5699 244268 -i 1176 -d 6.41872E-06 -o ../data_tensors_random/enron.tns;  
./tengen_naive_rand 319686 28153045 1607191 -d 7.80441E-12 -o ../data_tensors_random/flickr-3d.tns;
./tengen_naive_rand 319686 28153045 1607191 -i 731 -d 7.80441E-12 -o ../data_tensors_random/flickr-4d.tns; 
./tengen_naive_rand 1605 4198 1631 -i 4209 -j 868131 -d 1.10750E-07 -o ../data_tensors_random/lbnl-network.tns; 
./tengen_naive_rand 12092 9184 28818 -d 2.40224E-05 -o ../data_tensors_random/nell2.tns;
./tengen_naive_rand 2482 2862 14036 -i 17 -d 3.11080E-05 -o ../data_tensors_random/nips.tns; 
./tengen_naive_rand 183 24 1140 -i 1717 -d 9.23921E-03 -o ../data_tensors_random/uber.tns; 
./tengen_naive_rand 165427 11374 2 -d 6.91494E-03 -o ../data_tensors_random/vast-2015-mc1-3d.tns;
./tengen_naive_rand 165427 11374 2 -i 100 -j 89 -o ../data_tensors_random/vast-2015-mc1-5d.tns; 
