#include "texture.hpp"
#include "io.hpp"
#include <SDL2/SDL_opengles2.h>

namespace AF {
	texture2D::texture2D() {
			glGenTextures(1, &id);
		}
		texture2D::~texture2D() {
			glDeleteTextures(1, &id);
		}
		void texture2D::load_texture_rgb(const std::string &path) const {
			enable();
			
			unsigned char *image;
			int img_width, img_height;
			IO::read_image(path, &image, img_width, img_height);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			IO::free_image(image);
			
			disable();
		}
		void texture2D::set_mipmap() const {
			enable();
			glGenerateMipmap(GL_TEXTURE_2D);
			disable();
		}
		void texture2D::enable() const noexcept {
			glBindTexture(GL_TEXTURE_2D, id);
		}
		void texture2D::disable() noexcept {
			glBindTexture(GL_TEXTURE_2D, 0);
		}
}