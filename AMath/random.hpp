 
#ifndef __AF_RANDOM_H__
#define __AF_RANDOM_H__

#ifdef _MSC_VER
#pragma once
#endif

namespace AF {
    class random {
    public:
        static double double_(double beg, double end);
        static int int_(double beg, double end);
    };
}

#endif