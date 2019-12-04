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
