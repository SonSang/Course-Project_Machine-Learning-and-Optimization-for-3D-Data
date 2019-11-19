#ifndef __AF_IO_H__
#define __AF_IO_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <string>
#include <vector>
#include <fstream>

namespace AF {
    class IO {
    public:
        static std::ifstream open(const std::string &path);

        static std::string					read_text(const std::string &path);
        static std::vector<std::string>	    read_text_multi(const std::string &path);

        // read image at [path] to [bits] and set [width], [height] of it.
        static void read_image(const std::string &path, unsigned char **bits, int &width, int &height);
        static void free_image(unsigned char *bits);
        static void save_image(const std::string &path, unsigned char *bytes, int width, int height);
    };   
}

#endif