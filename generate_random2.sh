#!/bin/bash

./genten_naive_rand 22476 22476 23776223 -n 28397869 -o ../data_tensors_random2/1998darpa.tns;
./genten_naive_rand 532924 17262471 2480308 -n 139994992 -o ../data_tensors_random2/delicious-3d.tns;
./genten_naive_rand 319686 28153045 1607191 -n 113726748 -o ../data_tensors_random2/flickr-3d.tns;
./genten_naive_rand 23343790 23344784 166 -n 102919918 -o ../data_tensors_random2/freebase_music.tns;
./genten_naive_rand 38954435 38955429 532 -n 145810929 -o ../data_tensors_random2/freebase_sampled.tns;
./genten_naive_rand 2902330 2143368 25495389 -n 145154088 -o ../data_tensors_random2/nell1.tns;
./genten_naive_rand 12092 9184 28818 -n 76204397 -o ../data_tensors_random2/nell2.tns;
./genten_naive_rand 165427 11374 2 -n 25800183 -o ../data_tensors_random2/vast-2015-mc1-3d.tns;
./genten_naive_rand 6186 24 77 -i 32 -n 5131221 -o ../data_tensors_random2/chicago-crime-comm.tns;
./genten_naive_rand 6066 5699 244268 -i 1176 -n 57021427 -o ../data_tensors_random2/enron.tns;
./genten_naive_rand 319686 28153045 1607191 -i 731 -n 113641030 -o ../data_tensors_random2/flickr-4d.tns;
./genten_naive_rand 2482 2862 14036 -i 17 -n 3040652 -o ../data_tensors_random2/nips.tns;
./genten_naive_rand 183 24 1140 -i 1717 -n 3283148 -o ../data_tensors_random2/uber.tns;
./genten_naive_rand 6185 24 380 -i 395 -j 32 -n 6364316 -o ../data_tensors_random2/chicago-crime-geo.tns;
./genten_naive_rand 1605 4198 1631 -i 4209 -j 868131 -n 1784327 -o ../data_tensors_random2/lbnl-network.tns;
./genten_naive_rand 165427 11374 2 -i 100 -j 89 -n 26021841 -o ../data_tensors_random2/vast-2015-mc1-5d.tns;

