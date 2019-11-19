# ================================================= GCC
CC = g++ -std=c++17
DEBUG = -g
TARGET = main.exe

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
			$(ODIR_REND)/texture.o 						
LIB_REND = $(LDIR_REND)/librend.a
LD_REND = -L$(LDIR_REND)/ -lrend

# TOTAL
LIB = $(LIB_MATH) $(LIB_GEOM) $(LIB_REND)
LD = $(LIB_REND) $(LIB_GEOM) $(LIB_MATH) -lGL -lSDL2 -lgomp 

# EMCC
EMCC = emcc -std=c++17 -s USE_SDL=2 -O3
PL = ./Shader/red-vert.glsl ./Shader/red-frag.glsl		# Files to pre-load.
EMPL = --preload-file ./Shader/red-vert.glsl --preload-file ./Shader/red-frag.glsl
EMTARGET = main.html

gcc : $(LIB)
	$(CC) main.cpp $(LD) $(DEBUG) -o $(TARGET) 

# TIP : $^ means every dependency, $< means dependency one by one
# MATH
gcc_math : $(OBJ_MATH)
	ar rc $(LIB_MATH) $^	

$(LIB_MATH) : $(OBJ_MATH)
	ar rc $(LIB_MATH) $^

$(ODIR_MATH)/%.o : $(DIR_MATH)/%.cpp
	$(CC) -fopenmp $(DEBUG) -c -o $@ $<

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

#===============================================================

emcc :
	$(EMCC) main.cpp ARender/shader.cpp ARender/io.cpp -o $(EMTARGET) $(EMPL)

#===============================================================

clean_math :
	rm $(ODIR_MATH)/*.o $(LDIR_MATH)/*.a

clean_geom :
	rm $(ODIR_GEOM)/*.o $(LDIR_GEOM)/*.a

clean_rend :
	rm $(ODIR_REND)/*.o $(LDIR_REND)/*.a

clean_gcc :
	rm $(ODIR_MATH)/*.o $(LDIR_MATH)/*.a 	\
	rm $(ODIR_GEOM)/*.o $(LDIR_GEOM)/*.a	\
	rm $(ODIR_REND)/*.o $(LDIR_REND)/*.a	\
	rm *.exe

clean :
	rm *.exe main.html main.js main.wasm main.data \
	rm $(ODIR_MATH)/*.o $(LDIR_MATH)/*.a 	\
	rm $(ODIR_GEOM)/*.o $(LDIR_GEOM)/*.a	\
	rm $(ODIR_REND)/*.o $(LDIR_REND)/*.a
