#ifndef __AF_TEXTURE_H__
#define __AF_TEXTURE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <string>

namespace AF {
    class texture2D {
    private:
        using uint = unsigned int;
        uint id;
    public:
        texture2D();
        ~texture2D();
        
        // Load texture from [filename] and save it in RGB format.
        // Since this is very simple version, to fully utilize openGL settings,
        // user must call openGL func in own code.
        void load_texture_rgb(const std::string &filename) const;

        // make mipmap out of current texture.
        void set_mipmap() const;
        
        void enable() const noexcept;
        static void disable() noexcept;
    };
}

#endif