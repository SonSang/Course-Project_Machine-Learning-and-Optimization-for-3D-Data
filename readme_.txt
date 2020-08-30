1. PCL
- FLANN : sudo apt-get install libflann-dev
- BOOST : sudo apt-get install libboost-all-dev

sudo add-apt-repository ppa:v-launchpad-jochen-sprickerhof-de/pcl
sudo apt-get update
sudo apt-get install libpcl-dev

2. Emscripten
- git clone ~

3. stb
- git clone ~

4. imgui
- git clone ~

5. masbcpp
- git clone ~

6. nanoflann
- git clone ~

7. cgal
sudo apt-get install libcgal-dev
- dependency : -lgmp -lmpfr

8. dlib
sudo apt-get install libx11-dev


* pkg-config : We do not have to use -I"..", -l".." ... by using pkg-config.
e.g.) $ gcc -o test main.c `pkg-config --cflags --libs glib-2.0`
instead of ... $ gcc -o test main.c -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -lglib-2.0
(https://tjcplpllog.blogspot.com/2014/08/pkg-config.html)

pkg-config needs .pc file, which is stored at /usr/lib/x86_64.../pkgconfig/.

* Model info
- 02876657 : bottle
- 02691156 : plane
- 04004475 : printer
- 03938244 : pillow
- 03710193 : postbox
- 02843684 : birdbox
- 02828884 : bench
- 04379243 : table
- 02808440 : bathtub
- 02954340 : hat
- 03467517 : guitar
- 03513137 : helmet
- 04074963 : remote control
- 03636649 : lamp
- 03691459 : audio
- 03001627 : chair
- 03207941 : fridge
- 03642806 : laptop
- 02924116 : bus
- 02818832 : double bed
- 02880940 : bowl
- 03085013 : keyboard
- 03261776 : headphone
- 02958343 : car
- 04256520 : couch 
- 04460130 : church
- 02773838 : handbag
- 04530566 : ship
- 03797390 : cup
- 04401088 : phone
- 03761084 : hotplate
- 03948459 : gun
- 02747177 : trashcan
- 03046257 : clock
- 03325088 : tab
- 02942699 : webcam
- 02834778 : bike
- 04330267 : oven
- 02933112 : bookshelf
- 04099429 : missile
- 03211117 : monitor
- 04225987 : skateboard
- 03593526 : vase
- 03624134 : knife
- 02871439 : bookshelf2
- 04468005 : train
- 04554684 : washer
- 03337140 : drawer
- 03928116 : piano
- 03759954 : mike
- 04090263 : rifle
- 02946921 : can
- 03790512 : motorcycle

* Figure 1.
Comparison of residual spheres :
- 02691156/model_000081.obj vs 02691156/model_001063.obj
- 02691156/model_000081.obj vs 03790512/model_006763.obj

* Figure 2.
Single model's BVH :
- 03948459/011216.obj

* Figure 3.
DB's BVH :
- Node 17-28-(29, 79)
- Node 35-77-(92, 93)

* Search Results
- 02691156 : plane : 000081
- 03467517 : guitar : 014704
- 02924116 : bus : 037281
- 03085013 : keyboard : 027690
- 02958343 : car : 009608
- 02773838 : handbag : 011318
- 04530566 : ship : 048027
- 03948459 : gun : 013828
- 02834778 : bike : 040852
- 03624134 : knife : 048837