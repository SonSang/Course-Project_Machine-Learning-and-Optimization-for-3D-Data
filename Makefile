# ================================================= GCC
CC = g++ -std=c++17
DEBUG = -g
OPTIM = -O3
TARGET = ./Output/GCC/main.exe
TARGET_REL = ./Output/GCC/mainr.exe

# ========================================================= GCC DEBUG
# GCC_MATH
DIR_MATH = ./AMath
ODIR_MATH = $(DIR_MATH)/obj
LDIR_MATH = $(DIR_MATH)/lib
OBJ_MATH = $(ODIR_MATH)/math.o \
			$(ODIR_MATH)/random.o \
			$(ODIR_MATH)/timer.o 
LIB_MATH = $(LDIR_MATH)/libmath.a
LD_MATH = -L$(LDIR_MATH)/ -lmath

# GCC_GEOMETRY
DIR_GEOM = ./AGeometry
ODIR_GEOM = $(DIR_GEOM)/obj
LDIR_GEOM = $(DIR_GEOM)/lib
OBJ_GEOM = $(ODIR_GEOM)/geometry.o
LIB_GEOM = $(LDIR_GEOM)/libgeom.a
LD_GEOM = -L$(LDIR_GEOM)/ -lgeom

# GCC_RENDER
DIR_REND = ./ARender
ODIR_REND = $(DIR_REND)/obj
LDIR_REND = $(DIR_REND)/lib
OBJ_REND = $(ODIR_REND)/camera.o 						\
			$(ODIR_REND)/color.o 						\
			$(ODIR_REND)/io.o 							\
			$(ODIR_REND)/material.o 					\
			$(ODIR_REND)/object_manager.o 				\
			$(ODIR_REND)/object.o 						\
			$(ODIR_REND)/property_render_geometry.o 	\
			$(ODIR_REND)/property_render.o 				\
			$(ODIR_REND)/property.o 					\
			$(ODIR_REND)/render_geometry.o 				\
			$(ODIR_REND)/render_manager.o 				\
			$(ODIR_REND)/scene_manager.o 				\
			$(ODIR_REND)/shader.o 						\
			$(ODIR_REND)/texture.o 						\
			$(ODIR_REND)/mouse.o 						\
			$(ODIR_REND)/light.o						\
			$(ODIR_REND)/obj_loader.o
LIB_REND = $(LDIR_REND)/librend.a
LD_REND = -L$(LDIR_REND)/ -lrend

# GCC_SR
DIR_SR = ./ShapeRetrieval
ODIR_SR = $(DIR_SR)/obj
LDIR_SR = $(DIR_SR)/lib
OBJ_SR = 	$(ODIR_SR)/SRsphere.o		\
			$(ODIR_SR)/SRsphere_tree.o	\
			$(ODIR_SR)/SRpcl_interface.o	\
			$(ODIR_SR)/SRcgal_interface.o
LIB_SR = $(LDIR_SR)/libsr.a
LD_SR = -L$(LDIR_SR)/ -lsr

# GCC_IMGUI
DIR_IMGUI = ./Dependencies/imgui
ODIR_IMGUI = $(DIR_IMGUI)/obj
LDIR_IMGUI = $(DIR_IMGUI)/lib
OBJ_IMGUI = $(ODIR_IMGUI)/imgui.o	\
			$(ODIR_IMGUI)/imgui_demo.o 	\
			$(ODIR_IMGUI)/imgui_draw.o 	\
			$(ODIR_IMGUI)/imgui_widgets.o	\
			$(ODIR_IMGUI)/imgui_impl_sdl.o 	\
			$(ODIR_IMGUI)/imgui_impl_opengl3.o
LIB_IMGUI = $(LDIR_IMGUI)/libimgui.a
LD_IMGUI = -L$(LDIR_IMGUI)/ -limgui

# GCC_PCL
INCLUDE_PCL = -I/usr/include/pcl-1.8 -I/usr/include/eigen3
LIB_PCL = -lpcl_features -lpcl_common -lpcl_kdtree -lpcl_octree -llz4

# GCC_CGAL
LIB_CGAL = -lCGAL -lgmp -lmpfr

# ========================================================= GCC RELEASE

# GCC_MATH_REL
DIR_MATH_REL = ./AMath/release
ODIR_MATH_REL = $(DIR_MATH_REL)/obj
LDIR_MATH_REL = $(DIR_MATH_REL)/lib
OBJ_MATH_REL = $(ODIR_MATH_REL)/math.o \
			$(ODIR_MATH_REL)/random.o \
			$(ODIR_MATH_REL)/timer.o 
LIB_MATH_REL = $(LDIR_MATH_REL)/libmath.a
LD_MATH_REL = -L$(LDIR_MATH_REL)/ -lmath

# GCC_GEOMETRY_REL
DIR_GEOM_REL = ./AGeometry/release
ODIR_GEOM_REL = $(DIR_GEOM_REL)/obj
LDIR_GEOM_REL = $(DIR_GEOM_REL)/lib
OBJ_GEOM_REL = $(ODIR_GEOM_REL)/geometry.o
LIB_GEOM_REL = $(LDIR_GEOM_REL)/libgeom.a
LD_GEOM_REL = -L$(LDIR_GEOM_REL)/ -lgeom

# GCC_RENDER_REL
DIR_REND_REL = ./ARender/release
ODIR_REND_REL = $(DIR_REND_REL)/obj
LDIR_REND_REL = $(DIR_REND_REL)/lib
OBJ_REND_REL = $(ODIR_REND_REL)/camera.o 						\
			$(ODIR_REND_REL)/color.o 						\
			$(ODIR_REND_REL)/io.o 							\
			$(ODIR_REND_REL)/material.o 					\
			$(ODIR_REND_REL)/object_manager.o 				\
			$(ODIR_REND_REL)/object.o 						\
			$(ODIR_REND_REL)/property_render_geometry.o 	\
			$(ODIR_REND_REL)/property_render.o 				\
			$(ODIR_REND_REL)/property.o 					\
			$(ODIR_REND_REL)/render_geometry.o 				\
			$(ODIR_REND_REL)/render_manager.o 				\
			$(ODIR_REND_REL)/scene_manager.o 				\
			$(ODIR_REND_REL)/shader.o 						\
			$(ODIR_REND_REL)/texture.o 						\
			$(ODIR_REND_REL)/mouse.o 						\
			$(ODIR_REND_REL)/light.o						\
			$(ODIR_REND_REL)/obj_loader.o
LIB_REND_REL = $(LDIR_REND_REL)/librend.a
LD_REND_REL = -L$(LDIR_REND_REL)/ -lrend

# GCC_SR_REL
DIR_SR_REL = ./ShapeRetrieval/release
ODIR_SR_REL = $(DIR_SR_REL)/obj
LDIR_SR_REL = $(DIR_SR_REL)/lib
OBJ_SR_REL = 	$(ODIR_SR_REL)/SRsphere.o		\
			$(ODIR_SR_REL)/SRsphere_tree.o	\
			$(ODIR_SR_REL)/SRpcl_interface.o	\
			$(ODIR_SR_REL)/SRcgal_interface.o
LIB_SR_REL = $(LDIR_SR_REL)/libsr.a
LD_SR_REL = -L$(LDIR_SR_REL)/ -lsr

# GCC_IMGUI_REL
DIR_IMGUI_REL = ./Dependencies/imgui/release
ODIR_IMGUI_REL = $(DIR_IMGUI_REL)/obj
LDIR_IMGUI_REL = $(DIR_IMGUI_REL)/lib
OBJ_IMGUI_REL = $(ODIR_IMGUI_REL)/imgui.o	\
			$(ODIR_IMGUI_REL)/imgui_demo.o 	\
			$(ODIR_IMGUI_REL)/imgui_draw.o 	\
			$(ODIR_IMGUI_REL)/imgui_widgets.o	\
			$(ODIR_IMGUI_REL)/imgui_impl_sdl.o 	\
			$(ODIR_IMGUI_REL)/imgui_impl_opengl3.o
LIB_IMGUI_REL = $(LDIR_IMGUI_REL)/libimgui.a
LD_IMGUI_REL = -L$(LDIR_IMGUI_REL)/ -limgui

# ================================================================================

# TOTAL
LIB = $(LIB_MATH) $(LIB_GEOM) $(LIB_REND) $(LIB_SR) $(LIB_IMGUI)  
LD = $(LIB_SR) $(LIB_REND) $(LIB_GEOM) $(LIB_MATH) $(LIB_IMGUI) $(LIB_PCL) $(LIB_CGAL) -lGL -lSDL2 

LIB_REL = $(LIB_MATH_REL) $(LIB_GEOM_REL) $(LIB_REND_REL) $(LIB_SR_REL) $(LIB_IMGUI_REL)  
LD_REL = $(LIB_SR_REL) $(LIB_REND_REL) $(LIB_GEOM_REL) $(LIB_MATH_REL) $(LIB_IMGUI_REL) $(LIB_PCL) $(LIB_CGAL) -lGL -lSDL2 

gcc : $(LIB)
	$(CC) main.cpp $(INCLUDE_PCL) $(LD) $(DEBUG) -o $(TARGET)

gcc_rel : $(LIB_REL)
	$(CC) main.cpp $(INCLUDE_PCL) $(LD) $(OPTIM) -o $(TARGET_REL) 

# TIP : $^ means every dependency, $< means dependency one by one
# MATH
gcc_math : $(OBJ_MATH)
	ar rc $(LIB_MATH) $^	

$(LIB_MATH) : $(OBJ_MATH)
	ar rc $(LIB_MATH) $^

$(ODIR_MATH)/%.o : $(DIR_MATH)/%.cpp
	$(CC) $(DEBUG) -c -o $@ $<

gcc_math_rel : $(OBJ_MATH_REL)
	ar rc $(LIB_MATH_REL) $^	

$(LIB_MATH_REL) : $(OBJ_MATH_REL)
	ar rc $(LIB_MATH_REL) $^

$(ODIR_MATH_REL)/%.o : $(DIR_MATH)/%.cpp
	$(CC) $(OPTIM) -c -o $@ $<

# GEOMETRY
gcc_geom : $(OBJ_GEOM)
	ar rc $(LIB_GEOM) $^

$(LIB_GEOM) : $(OBJ_GEOM)
	ar rc $(LIB_GEOM) $^

$(ODIR_GEOM)/%.o : $(DIR_GEOM)/%.cpp
	$(CC) $(DEBUG) -c -o $@ $<

gcc_geom_rel : $(OBJ_GEOM_REL)
	ar rc $(LIB_GEOM_REL) $^

$(LIB_GEOM_REL) : $(OBJ_GEOM_REL)
	ar rc $(LIB_GEOM_REL) $^

$(ODIR_GEOM_REL)/%.o : $(DIR_GEOM)/%.cpp
	$(CC) $(OPTIM) -c -o $@ $<

# RENDERER
gcc_rend : $(OBJ_REND)
	ar rc $(LIB_REND) $^

$(LIB_REND) : $(OBJ_REND)
	ar rc $(LIB_REND) $^

$(ODIR_REND)/%.o : $(DIR_REND)/%.cpp
	$(CC) $(DEBUG) -c -o $@ $<

gcc_rend_rel : $(OBJ_REND_REL)
	ar rc $(LIB_REND_REL) $^

$(LIB_REND_REL) : $(OBJ_REND_REL)
	ar rc $(LIB_REND_REL) $^

$(ODIR_REND_REL)/%.o : $(DIR_REND)/%.cpp
	$(CC) $(OPTIM) -c -o $@ $<

# SHAPE_RETRIEVAL
gcc_sr : $(OBJ_SR)
	ar rc $(LIB_SR) $^

$(LIB_SR) : $(OBJ_SR)
	ar rc $(LIB_SR) $^

$(ODIR_SR)/%.o : $(DIR_SR)/%.cpp
	$(CC) $(DEBUG) $(INCLUDE_PCL) -c -o $@ $<

gcc_sr_rel : $(OBJ_SR_REL)
	ar rc $(LIB_SR_REL) $^

$(LIB_SR_REL) : $(OBJ_SR_REL)
	ar rc $(LIB_SR_REL) $^

$(ODIR_SR_REL)/%.o : $(DIR_SR)/%.cpp
	$(CC) $(OPTIM) $(INCLUDE_PCL) -c -o $@ $<

# IMGUI
gcc_imgui : $(OBJ_IMGUI)
	ar rc $(LIB_IMGUI) $^

$(LIB_IMGUI) : $(OBJ_IMGUI)
	ar rc $(LIB_IMGUI) $^

$(ODIR_IMGUI)/%.o : ./Dependencies/imgui/%.cpp 
	$(CC) $(DEBUG) -c -o $@ $<

$(ODIR_IMGUI)/%.o : ./Dependencies/imgui/examples/%.cpp 
	$(CC) $(DEBUG) -I./Dependencies/imgui -c -o $@ $<

gcc_imgui_rel : $(OBJ_IMGUI_REL)
	ar rc $(LIB_IMGUI_REL) $^

$(LIB_IMGUI_REL) : $(OBJ_IMGUI_REL)
	ar rc $(LIB_IMGUI_REL) $^

$(ODIR_IMGUI_REL)/%.o : ./Dependencies/imgui/%.cpp 
	$(CC) $(OPTIM) -c -o $@ $<

$(ODIR_IMGUI_REL)/%.o : ./Dependencies/imgui/examples/%.cpp 
	$(CC) $(OPTIM) -I./Dependencies/imgui -c -o $@ $<

# ================================================= EMCC

# EMCC
EMCC = emcc -std=c++17 -s USE_SDL=2 -O3 -s DISABLE_EXCEPTION_CATCHING=0
PL = ./Shader/render_geometry-vert.glsl ./Shader/render_geometry-frag.glsl		# Files to pre-load.
EMPL = --preload-file ./Shader/render_geometry-vert.glsl 	\
		--preload-file ./Shader/render_geometry-frag.glsl	\
		--preload-file ./Assets/Greek_Vase/Greek_Vase_3.obj
EMTARGET = ./Output/EMCC/main.html
EMCCDIR = emcc

# EMCC_MATH
EMCC_ODIR_MATH = $(DIR_MATH)/$(EMCCDIR)/obj
EMCC_LDIR_MATH = $(DIR_MATH)/$(EMCCDIR)/lib
EMCC_OBJ_MATH = $(EMCC_ODIR_MATH)/math.bc \
				$(EMCC_ODIR_MATH)/random.bc \
				$(EMCC_ODIR_MATH)/timer.bc

# EMCC_GEOM
EMCC_ODIR_GEOM = $(DIR_GEOM)/$(EMCCDIR)/obj
EMCC_LDIR_GEOM = $(DIR_GEOM)/$(EMCCDIR)/lib
EMCC_OBJ_GEOM = $(EMCC_ODIR_GEOM)/geometry.bc

# GCC_RENDER
EMCC_ODIR_REND = $(DIR_REND)/$(EMCCDIR)/obj
EMCC_LDIR_REND = $(DIR_REND)/$(EMCCDIR)/lib
EMCC_OBJ_REND = $(EMCC_ODIR_REND)/camera.bc 						\
			$(EMCC_ODIR_REND)/color.bc 						\
			$(EMCC_ODIR_REND)/io.bc							\
			$(EMCC_ODIR_REND)/material.bc 					\
			$(EMCC_ODIR_REND)/object_manager.bc				\
			$(EMCC_ODIR_REND)/object.bc 					\
			$(EMCC_ODIR_REND)/property_render_geometry.bc 	\
			$(EMCC_ODIR_REND)/property_render.bc			\
			$(EMCC_ODIR_REND)/property.bc 					\
			$(EMCC_ODIR_REND)/render_geometry.bc 				\
			$(EMCC_ODIR_REND)/render_manager.bc 				\
			$(EMCC_ODIR_REND)/scene_manager.bc 				\
			$(EMCC_ODIR_REND)/shader.bc 						\
			$(EMCC_ODIR_REND)/texture.bc 					\
			$(EMCC_ODIR_REND)/mouse.bc						\
			$(EMCC_ODIR_REND)/light.bc						\
			$(EMCC_ODIR_REND)/obj_loader.bc

# EMCC_SR
EMCC_ODIR_SR = $(DIR_SR)/$(EMCCDIR)/obj
EMCC_LDIR_SR = $(DIR_SR)/$(EMCCDIR)/lib
EMCC_OBJ_SR = $(EMCC_ODIR_SR)/SRsphere.bc					\
				$(EMCC_ODIR_SR)/SRsphere_tree.bc	

# EMCC_IMGUI
EMCC_ODIR_IMGUI = $(DIR_IMGUI)/$(EMCCDIR)/obj
EMCC_OBJ_IMGUI = $(EMCC_ODIR_IMGUI)/imgui.bc	\
			$(EMCC_ODIR_IMGUI)/imgui_demo.bc 	\
			$(EMCC_ODIR_IMGUI)/imgui_draw.bc 	\
			$(EMCC_ODIR_IMGUI)/imgui_widgets.bc	\
			$(EMCC_ODIR_IMGUI)/imgui_impl_sdl.bc 	\
			$(EMCC_ODIR_IMGUI)/imgui_impl_opengl3.bc

#===============================================================

emcc : $(EMCC_OBJ_MATH) $(EMCC_OBJ_GEOM) $(EMCC_OBJ_REND) $(EMCC_OBJ_SR) $(EMCC_OBJ_IMGUI)
	$(EMCC) main.cpp -c -o Output/EMCC/main.bc
	$(EMCC) Output/EMCC/main.bc $(EMCC_OBJ_IMGUI) $(EMCC_OBJ_SR) $(EMCC_OBJ_REND) $(EMCC_OBJ_GEOM) $(EMCC_OBJ_MATH) -o $(EMTARGET) $(EMPL)

# TIP : $^ means every dependency, $< means dependency one by one
# MATH
emcc_math : $(EMCC_OBJ_MATH)
	
$(EMCC_ODIR_MATH)/%.bc : $(DIR_MATH)/%.cpp
	$(EMCC) -c -o $@ $<

# GEOMETRY
emcc_geom : $(EMCC_OBJ_GEOM)
	
$(EMCC_ODIR_GEOM)/%.bc : $(DIR_GEOM)/%.cpp
	$(EMCC) -c -o $@ $<

# RENDERER
emcc_rend : $(EMCC_OBJ_REND)
	
$(EMCC_ODIR_REND)/%.bc : $(DIR_REND)/%.cpp
	$(EMCC) -c -o $@ $<

# SR
emcc_sr : $(EMCC_OBJ_SR)

$(EMCC_ODIR_SR)/%.bc : $(DIR_SR)/%.cpp
	$(EMCC) -c -o $@ $<

# IMGUI
emcc_imgui : $(EMCC_OBJ_IMGUI)

$(EMCC_ODIR_IMGUI)/%.bc : ./Dependencies/imgui/%.cpp
	$(EMCC) -c -o $@ $<

$(EMCC_ODIR_IMGUI)/%.bc : ./Dependencies/imgui/examples/%.cpp
	$(EMCC) -c -o $@ $< -I./Dependencies/imgui

#===============================================================

clean_math :
	rm $(ODIR_MATH)/*.o $(LDIR_MATH)/*.a	\
	rm $(EMCC_ODIR_MATH)/*.bc $(EMCC_LDIR_MATH)/*.bc

clean_geom :
	rm $(ODIR_GEOM)/*.o $(LDIR_GEOM)/*.a	\
	rm $(EMCC_ODIR_GEOM)/*.bc $(EMCC_LDIR_GEOM)/*.bc

clean_rend :
	rm $(ODIR_REND)/*.o $(LDIR_REND)/*.a	\
	rm $(EMCC_ODIR_REND)/*.bc $(EMCC_LDIR_REND)/*.bc

clean_sr :
	rm $(ODIR_SR)/*.o $(LDIR_SR)/*.a		\
	rm $(EMCC_ODIR_SR)/*.bc $(EMCC_LDIR_SR)/*.bc

clean_imgui :
	rm $(ODIR_IMGUI)/*.o $(LDIR_IMGUI)/*.a 	\
	rm $(EMCC_ODIR_IMGUI)/*.bc

clean_gcc :
	rm $(ODIR_MATH)/*.o $(LDIR_MATH)/*.a 	\
	rm $(ODIR_GEOM)/*.o $(LDIR_GEOM)/*.a	\
	rm $(ODIR_REND)/*.o $(LDIR_REND)/*.a	\
	rm $(ODIR_SR)/*.o $(LDIR_SR)/*.a		\
	rm $(ODIR_IMGUI)/*.o $(LDIR_IMGUI)/*.a 	\
	rm Output/GCC/*.exe

clean_emcc :
	rm $(EMCC_ODIR_MATH)/*.bc $(EMCC_LDIR_MATH)/*.bc 	\
	rm $(EMCC_ODIR_GEOM)/*.bc $(EMCC_LDIR_GEOM)/*.bc	\
	rm $(EMCC_ODIR_REND)/*.bc $(EMCC_LDIR_REND)/*.bc	\
	rm $(EMCC_ODIR_SR)/*.bc $(EMCC_LDIR_SR)/*.bc		\
	rm $(EMCC_ODIR_IMGUI)/*.bc							\
	rm Output/EMCC/*.wasm Output/EMCC/*.js Output/EMCC/*.html Output/EMCC/*.data

clean :
	rm $(ODIR_MATH)/*.o $(LDIR_MATH)/*.a 	\
	rm $(ODIR_GEOM)/*.o $(LDIR_GEOM)/*.a	\
	rm $(ODIR_REND)/*.o $(LDIR_REND)/*.a	\
	rm $(ODIR_SR)/*.o $(LDIR_SR)/*.a		\
	rm $(ODIR_IMGUI)/*.o $(LDIR_IMGUI)/*.a 	\
	rm $(EMCC_ODIR_MATH)/*.bc $(EMCC_LDIR_MATH)/*.bc 	\
	rm $(EMCC_ODIR_GEOM)/*.bc $(EMCC_LDIR_GEOM)/*.bc	\
	rm $(EMCC_ODIR_REND)/*.bc $(EMCC_LDIR_REND)/*.bc	\
	rm $(EMCC_ODIR_SR)/*.bc $(EMCC_LDIR_SR)/*.bc		\
	rm $(EMCC_ODIR_IMGUI)/*.bc							\
	rm Output/GCC/*.exe Output/EMCC/*.bc Output/EMCC/*.wasm Output/EMCC/*.js Output/EMCC/*.html Output/EMCC/*.data
