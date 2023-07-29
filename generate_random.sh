#!/bin/bash

./tengen 23776223 22476 22476 -d 2.36749E-09 -f 5.26815E-05 -c 0.349034 -v 1.007779 -o ../data_tensors_generated/1998darpa.tns;
./tengen 6186 77 32 -d 3.49729E-01 -f 9.14535E-01 -c 0.238041 -v 0.854434 -o ../data_tensors_generated/chicago-crime-comm.tns;
./tengen 6185 395 380 -d 6.81520E-03 -f 6.03983E-01 -c 0.070892 -v 0.848246 -o ../data_tensors_generated/chicago-crime-geo.tns;
./tengen 244268 6066 5699 -d 6.41872E-06 -f 2.05693E-03 -c 6.876134 -v 5.520073 -o ../data_tensors_generated/enron.tns;
./tengen 28153045 1607191 319686 -d 7.80441E-12 -f 2.49496E-06 -c 1.040028 -v 0.999998 -o ../data_tensors_generated/flickr-3d.tns;
./tengen 28153045 1607191 319686 -d 7.80441E-12 -f 2.49496E-06 -c 1.040028 -v 0.999998 -o ../data_tensors_generated/flickr-4d.tns;
./tengen 868131 4209 4198 -d 1.10750E-07 -f 4.52626E-04 -c 0.388604 -v 1.015046 -o ../data_tensors_generated/lbnl-network.tns;
./tengen 28818 12092 9184 -d 2.40224E-05 -f 6.16512E-02 -c 0.683027 -v 3.325869 -o ../data_tensors_generated/nell2.tns;
./tengen 14036 2862 2482 -d 3.11080E-05 -f 5.72779E-02 -c 1.707878 -v 1.24872 -o ../data_tensors_generated/nips.tns;
./tengen 1717 1140 183 -d 9.23921E-03 -f 3.31806E-02 -c 1.515445 -v 4.138204 -o ../data_tensors_generated/uber.tns;
./tengen 165427 11374 2 -d 6.91494E-03 -f 1.38299E-02 -c 0.491576 -v 0.98617 -o ../data_tensors_generated/vast-2015-mc1-3d.tns;
./tengen 165427 11374 100 -d 1.38299E-04 -f 1.38299E-02 -c 0.491576 -v 0.986172 -o ../data_tensors_generated/vast-2015-mc1-5d.tns;