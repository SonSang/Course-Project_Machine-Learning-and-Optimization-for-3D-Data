# ================================================= GCC
CC = g++ -std=c++17
DEBUG = -g
TARGET = ./Output/GCC/main.exe

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
			$(ODIR_REND)/light.o
LIB_REND = $(LDIR_REND)/librend.a
LD_REND = -L$(LDIR_REND)/ -lrend

# TOTAL
LIB = $(LIB_MATH) $(LIB_GEOM) $(LIB_REND)
LD = $(LIB_REND) $(LIB_GEOM) $(LIB_MATH) -lGL -lSDL2 -lgomp 

gcc : $(LIB)
	$(CC) main.cpp $(LD) $(DEBUG) -o $(TARGET) 

# TIP : $^ means every dependency, $< means dependency one by one
# MATH
gcc_math : $(OBJ_MATH)
	ar rc $(LIB_MATH) $^	

$(LIB_MATH) : $(OBJ_MATH)
	ar rc $(LIB_MATH) $^

$(ODIR_MATH)/%.o : $(DIR_MATH)/%.cpp
	$(CC) $(DEBUG) -c -o $@ $<

# GEOMETRY
gcc_geom : $(OBJ_GEOM)
	ar rc $(LIB_GEOM) $^

$(LIB_GEOM) : $(OBJ_GEOM)
	ar rc $(LIB_GEOM) $^

$(ODIR_GEOM)/%.o : $(DIR_GEOM)/%.cpp
	$(CC) $(DEBUG) -c -o $@ $<

# RENDERER
gcc_rend : $(OBJ_REND)
	ar rc $(LIB_REND) $^

$(LIB_REND) : $(OBJ_REND)
	ar rc $(LIB_REND) $^

$(ODIR_REND)/%.o : $(DIR_REND)/%.cpp
	$(CC) $(DEBUG) -c -o $@ $<

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
			$(EMCC_ODIR_REND)/light.bc

#===============================================================

emcc : $(EMCC_OBJ_MATH) $(EMCC_OBJ_GEOM) $(EMCC_OBJ_REND)
	$(EMCC) main.cpp -c -o Output/EMCC/main.bc
	$(EMCC) Output/EMCC/main.bc $(EMCC_OBJ_REND) $(EMCC_OBJ_GEOM) $(EMCC_OBJ_MATH) -o $(EMTARGET) $(EMPL)

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

clean_gcc :
	rm $(ODIR_MATH)/*.o $(LDIR_MATH)/*.a 	\
	rm $(ODIR_GEOM)/*.o $(LDIR_GEOM)/*.a	\
	rm $(ODIR_REND)/*.o $(LDIR_REND)/*.a	\
	rm Output/GCC/*.exe

clean_emcc :
	rm $(EMCC_ODIR_MATH)/*.bc $(EMCC_LDIR_MATH)/*.bc 	\
	rm $(EMCC_ODIR_GEOM)/*.bc $(EMCC_LDIR_GEOM)/*.bc	\
	rm $(EMCC_ODIR_REND)/*.bc $(EMCC_LDIR_REND)/*.bc	\
	rm Output/EMCC/*.wasm Output/EMCC/*.js Output/EMCC/*.html Output/EMCC/*.data

clean :
	rm $(ODIR_MATH)/*.o $(LDIR_MATH)/*.a 	\
	rm $(ODIR_GEOM)/*.o $(LDIR_GEOM)/*.a	\
	rm $(ODIR_REND)/*.o $(LDIR_REND)/*.a	\
	rm $(EMCC_ODIR_MATH)/*.bc $(EMCC_LDIR_MATH)/*.bc 	\
	rm $(EMCC_ODIR_GEOM)/*.bc $(EMCC_LDIR_GEOM)/*.bc	\
	rm $(EMCC_ODIR_REND)/*.bc $(EMCC_LDIR_REND)/*.bc	\
	rm Output/GCC/*.exe Output/EMCC/*.bc Output/EMCC/*.wasm Output/EMCC/*.js Output/EMCC/*.html Output/EMCC/*.data
