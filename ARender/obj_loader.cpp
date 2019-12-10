#include "obj_loader.hpp"
#include "io.hpp"
#include <string>

namespace AF {
    int random_name = 0;

    void obj_loader::vertex::set_position(const vec3d &v) {
        position = v;
    }
    void obj_loader::vertex::set_normal(const vec3d &n) {
        normal = n;
    }
    void obj_loader::vertex::set_texture(const vec2d &t) {
        texture = t;
    }

    vec3d& obj_loader::vertex::get_position() noexcept {
        return position;
    }
    vec3d& obj_loader::vertex::get_normal() noexcept {
        return normal;
    }
    vec2d& obj_loader::vertex::get_texture() noexcept {
        return texture;
    }

    const vec3d& obj_loader::vertex::get_position_c() const noexcept {
        return position;
    }
    const vec3d& obj_loader::vertex::get_normal_c() const noexcept {
        return normal;
    }

    void obj_loader::face::add_vertex(const vertex &v) {
        vertexlist.push_back(v);
    }
    obj_loader::vertex& obj_loader::face::get_vertex(size_t i) {
        return vertexlist.at(i);
    }
    const obj_loader::vertex& obj_loader::face::get_vertex_c(size_t i) const {
        return vertexlist.at(i);
    }
    bool obj_loader::face::is_empty() const noexcept {
        return vertexlist.empty();
    }
    size_t obj_loader::face::size() const noexcept {
        return vertexlist.size();
    }
    void obj_loader::face::set_normal() {
        vec3d
            &pos0 = get_vertex(0).get_position(),
            &pos1 = get_vertex(1).get_position(),
            &pos2 = get_vertex(2).get_position(),
            sub0 = pos1 - pos0,
            sub1 = pos2 - pos1,
            normal = sub0.cross(sub1);
        if (normal.lensq() > 0) {
            normal.normalize();
            size_t
                vnum = size();
            for (int i = 0; i < vnum; i++)
                get_vertex(i).set_normal(normal);
        }
    }

    void obj_loader::group::set_name(const std::string &name) {
        this->name = name;
    }
    const std::string& obj_loader::group::get_name() const noexcept {
        return this->name;
    }
    void obj_loader::group::add_face(const face &f) {
        facelist.push_back(f);
    }
    obj_loader::face& obj_loader::group::get_face(size_t i) {
        return facelist.at(i);
    }
    const obj_loader::face& obj_loader::group::get_face_c(size_t i) const {
        return facelist.at(i);
    }
    bool obj_loader::group::is_empty() const noexcept {
        return facelist.empty();
    }
    size_t obj_loader::group::size() const noexcept {
        return facelist.size();
    }

    void obj_loader::object::set_name(const std::string &name) {
        this->name = name;
    }
    const std::string& obj_loader::object::get_name() const noexcept {
        return this->name;
    }
    void obj_loader::object::add_group(const group &g) {
        grouplist.push_back(g);
    }
    obj_loader::group& obj_loader::object::get_group(size_t i) {
        return grouplist.at(i);
    }
    const obj_loader::group& obj_loader::object::get_group_c(size_t i) const {
        return grouplist.at(i);
    }
    bool obj_loader::object::is_empty() const noexcept {
        return grouplist.empty();
    }
    size_t obj_loader::object::size() const noexcept {
        return grouplist.size();
    }

    obj_loader::obj_loader(const std::string &filename) {
        load(filename);
    }

    void obj_loader::start_object(std::string &line) {
        std::string copy = line;
        std::size_t beg = copy.find_first_of(" ") + 1;
        std::size_t end = copy.find_first_of(" \n\r", beg);
        object no;
        no.set_name(copy.substr(beg, end - beg));
        add_object(no);
    }
    void obj_loader::start_group(std::string &line) {
        std::string copy = line;
        std::size_t beg = copy.find_first_of(" ") + 1;
        std::size_t end = copy.find_first_of(" \n\r", beg);
        group
            ng;
        ng.set_name(copy.substr(beg, end - beg));
        if (is_empty()) {
            object
                no;
            no.set_name(std::to_string(random_name++));
            add_object(no);
        }
        object
            &curobject = get_object(size() - 1);
        curobject.add_group(ng);
    }

    void obj_loader::parse_vertex(std::string &line, std::vector<vec3d> &vlist) {
        vec3d
            val(0, 0, 0);
        double
            v0 = 0, v1 = 0, v2 = 0;
        std::string copy = line;

        std::size_t beg = copy.find_first_of(" ") + 1;        
        std::size_t end = copy.find_first_of(" ", beg);
        v0 = ::atof(copy.substr(beg, end - beg).c_str());

        beg = end + 1;        
        end = copy.find_first_of(" ", beg);
        v1 = ::atof(copy.substr(beg, end - beg).c_str());

        beg = end + 1;        
        end = copy.find_first_of(" \n\r", beg);
        v2 = ::atof(copy.substr(beg, end - beg).c_str());

        val.set(v0, v1, v2);
        vlist.push_back(val);
    }
    void obj_loader::parse_normal(std::string &line, std::vector<vec3d> &nlist) {
        vec3d
            val(0, 0, 0);
        double
            v0 = 0, v1 = 0, v2 = 0;
        std::string copy = line;

        std::size_t beg = copy.find_first_of(" ") + 1;        
        std::size_t end = copy.find_first_of(" ", beg);
        v0 = ::atof(copy.substr(beg, end - beg).c_str());

        beg = end + 1;        
        end = copy.find_first_of(" ", beg);
        v1 = ::atof(copy.substr(beg, end - beg).c_str());

        beg = end + 1;        
        end = copy.find_first_of(" \n\r", beg);
        v2 = ::atof(copy.substr(beg, end - beg).c_str());

        val.set(v0, v1, v2);
        nlist.push_back(val);
    }
    void obj_loader::parse_texture(std::string &line, std::vector<vec2d> &tlist) {
        vec2d
            val(0, 0);
        double
            v0 = 0, v1 = 0;
        std::string copy = line;

        std::size_t beg = copy.find_first_of(" ") + 1;        
        std::size_t end = copy.find_first_of(" ", beg);
        v0 = ::atof(copy.substr(beg, end - beg).c_str());

        beg = end + 1;        
        end = copy.find_first_of(" \n\r", beg);
        v1 = ::atof(copy.substr(beg, end - beg).c_str());

        val.set(v0, v1);
        tlist.push_back(val);
    }

    void obj_loader::parse_face(std::string &line,
        const std::vector<vec3d> &vlist,
        const std::vector<vec3d> &nlist,
        const std::vector<vec2d> &tlist) {
        face
            f;
        std::string copy = line;
        std::size_t beg = copy.find_first_of(" ") + 1;
        std::vector<std::string>
            tokenlist;
        std::string token;
        while (true) {
            std::size_t end = copy.find_first_of(" \r\n", beg);
            token = copy.substr(beg, end - beg);
            tokenlist.push_back(token);
            if(end == std::string::npos)
                break;
            beg = end + 1;
        }
        size_t
            len = tokenlist.size();
        std::set<int> testface;     // To prevent duplicate faces.
        size_t tlen = len;
        for (int i = 0; i < len; i++) {
            std::string
                str = tokenlist.at(i);
            size_t
                len = str.length();
            if(len == 0)
                continue;
            int
                delim[3];
            int
                idx = 0;
            vertex
                vert;
            for (int j = 0; j < len; j++) {
                if (str[j] == '/')
                    delim[idx++] = j;
            }
            if (idx == 0) // only vertex info
            {
                size_t
                    vertexindex = atoi(str.c_str());
                if (vertexindex < 0) {
                    size_t
                        curvertnum = vlist.size();
                    vertexindex += curvertnum + 1;
                }
                vert.set_position(vlist.at(vertexindex - 1));
                vert.set_normal(vec3d(0, 0, 0));
                vert.set_texture(vec2d(0, 0));
                f.add_vertex(vert);
                testface.insert(vertexindex - 1);
                continue;
            }
            else if (idx == 1) // vertex and texture info
            {
                str[delim[0]] = '\0';
                size_t
                    vertexindex = atoi(str.c_str());
                if (vertexindex < 0) {
                    size_t
                        curvertnum = vlist.size();
                    vertexindex += curvertnum + 1;
                }
                size_t
                    textureindex = atoi(str.c_str() + delim[0] + 1);
                if (textureindex < 0) {
                    size_t
                        curtexnum = tlist.size();
                    textureindex += curtexnum + 1;
                }
                vert.set_position(vlist.at(vertexindex - 1));
                vert.set_texture((textureindex - 1) < tlist.size() ? tlist.at(textureindex - 1) : vec2d(0, 0));
                vert.set_normal(vec3d(0, 0, 0));
                str[delim[0]] = '/';
                f.add_vertex(vert);
                testface.insert(vertexindex - 1);
                continue;
            }
            else // all 3 info
            {
                str[delim[0]] = '\0';
                str[delim[1]] = '\0';
                bool
                    has_texture_info = false;
                int
                    vertexindex = 0,
                    normalindex = 0,
                    textureindex = 0;
                vertexindex = atoi(str.c_str());
                if (vertexindex < 0) {
                    size_t
                        curvertnum = vlist.size();
                    vertexindex += (int)curvertnum + 1;
                }
                if (delim[1] == delim[0] + 1) // no texture info
                    textureindex = -1;
                else {
                    textureindex = atoi(str.c_str() + delim[0] + 1);
                    has_texture_info = true;
                }
                if (has_texture_info && textureindex < 0) {
                    size_t
                        curtexnum = tlist.size();
                    textureindex += (int)curtexnum + 1;
                }
                normalindex = atoi(str.c_str() + delim[1] + 1);
                if (normalindex < 0) {
                    size_t
                        curnormnum = nlist.size();
                    normalindex += (int)curnormnum + 1;
                }
                vert.set_position(vlist.at(vertexindex - 1));
                vert.set_normal((normalindex - 1 < nlist.size()) ? nlist.at(normalindex - 1) : vec3d(0, 0, 0));
                vert.set_texture((textureindex < 0) ? vec2d(0, 0) : (textureindex - 1 < tlist.size() ? tlist.at(textureindex - 1) : vec2d(0, 0)));
                f.add_vertex(vert);
                testface.insert(vertexindex - 1);
                str[delim[0]] = '/';
                str[delim[1]] = '/';
                continue;
            }
        }
        // Check for duplicate face.
        if(uface.find(testface) != uface.end())
            return;

        if (is_empty()) { // no prev object.
            object
                no;
            no.set_name(std::to_string(random_name++));
            add_object(no);
        }
        object
            &o = get_object(size() - 1);
        if (o.is_empty()) { // no prev group.
            group
                ng;
            ng.set_name(std::to_string(random_name++));
            o.add_group(ng);
        }
        group
            &g = o.get_group(o.size() - 1);
        g.add_face(f);
        uface.insert(testface);
    }

    void obj_loader::load(const std::string &filename) {
        uface.clear();
        std::vector<std::string>
            linelist = IO::read_text_multi(filename);
        std::vector<vec3d>
            vertexlist;
        std::vector<vec3d>
            normallist;
        std::vector<vec2d>
            texturelist;
        for (auto it = linelist.begin(); it != linelist.end(); it++) {
            std::string
                &line = *it;
            if (line[0] == 'o') {
                start_object(line);
            }
            else if (line[0] == 'g') {
                start_group(line);
            }
            else if (line[0] == 'v') {
                if (line[1] == ' ') {
                    parse_vertex(line, vertexlist);
                }
                else if (line[1] == 'n') {
                    parse_normal(line, normallist);
                }
                else if (line[1] == 't') {
                    parse_texture(line, texturelist);
                }
            }
            else if (line[0] == 'f') {
                parse_face(line, vertexlist, normallist, texturelist);
            }
        }
    }

    void obj_loader::add_object(const object &o) {
        objectlist.push_back(o);
    }
    obj_loader::object& obj_loader::get_object(size_t i) {
        return objectlist.at(i);
    }
    const obj_loader::object& obj_loader::get_object_c(size_t i) const {
        return objectlist.at(i);
    }

    bool obj_loader::is_empty() const noexcept {
        return objectlist.empty();
    }
    size_t obj_loader::size() const noexcept {
        return objectlist.size();
    }

    void obj_loader::clear() {
        objectlist.clear();
    }

    void obj_loader::set_normal() {
        for (auto o = objectlist.begin(); o != objectlist.end(); o++) {
            size_t
                gsize = o->size();
            for (int i = 0; i < gsize; i++) {
                group
                    &g = o->get_group(i);
                size_t
                    fsize = g.size();
                for (int j = 0; j < fsize; j++) {
                    face
                        &f = g.get_face(j);
                    f.set_normal();
                }
            }
        }
    }
    void obj_loader::get_vertex_list(std::vector<vertex> &vlist) const noexcept {
        auto osize = this->size();
        for (int i = 0; i < osize; i++) {
            const auto &object = this->get_object_c(i);
            auto gsize = object.size();
            for (int j = 0; j < gsize; j++) {
                const auto &group = object.get_group_c(j);
                auto fsize = group.size();
                for (int k = 0; k < fsize; k++) {
                    const auto &face = group.get_face_c(k);
                    auto vsize = face.size();
                    for (int m = 0; m < vsize; m++) {
                        vlist.push_back(face.get_vertex_c(m));
                    }
                }
            }
        }
    }
    void obj_loader::get_vertex_pos_list(std::vector<vec3d> &vlist) const noexcept {
        auto osize = this->size();
        for (int i = 0; i < osize; i++) {
            const auto &object = this->get_object_c(i);
            auto gsize = object.size();
            for (int j = 0; j < gsize; j++) {
                const auto &group = object.get_group_c(j);
                auto fsize = group.size();
                for (int k = 0; k < fsize; k++) {
                    const auto &face = group.get_face_c(k);
                    auto vsize = face.size();
                    for (int m = 0; m < vsize; m++) {
                        vlist.push_back(face.get_vertex_c(m).get_position_c());
                    }
                }
            }
        }
    }
}