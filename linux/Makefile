LDFLAGS = -L. -lglfw -lGL -ldl # -lpthread
CXXFLAGS = -g -Wall -Wno-write-strings -Wno-parentheses -Wno-deprecated-declarations -DLINUX #-pthread

vpath %.cpp ../src
vpath %.c   ../src/glad/src

OBJS = main.o editor.o canvas.o gpuProgram.o linalg.o strokefont.o fg_stroke.o glad.o texture.o drawSegs.o lodepng.o

EXEC = editor

all:    $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJS) $(LDFLAGS) 

clean:
	rm -f *~ $(EXEC) $(OBJS) Makefile.bak

depend:	
	makedepend -Y ../src/*.h ../src/*.cpp 2> /dev/null
	@echo ""
	@echo "Now edit the Makefile to remove the ../src/ prefix of all of the dependences for the .o files"
	@echo ""

# DO NOT DELETE

canvas.o: ../src/headers.h ../src/glad/include/glad/glad.h
canvas.o: ../src/glad/include/KHR/khrplatform.h ../src/linalg.h
canvas.o: ../src/main.h ../src/texture.h ../src/gpuProgram.h
canvas.o: ../src/seq.h ../src/drawSegs.h
drawSegs.o: ../src/headers.h ../src/glad/include/glad/glad.h
drawSegs.o: ../src/glad/include/KHR/khrplatform.h ../src/linalg.h
drawSegs.o: ../src/gpuProgram.h ../src/seq.h
editor.o: ../src/headers.h ../src/glad/include/glad/glad.h
editor.o: ../src/glad/include/KHR/khrplatform.h ../src/linalg.h
editor.o: ../src/texture.h ../src/gpuProgram.h ../src/seq.h
fg_stroke.o: ../src/headers.h ../src/glad/include/glad/glad.h
fg_stroke.o: ../src/glad/include/KHR/khrplatform.h ../src/linalg.h
gpuProgram.o: ../src/headers.h ../src/glad/include/glad/glad.h
gpuProgram.o: ../src/glad/include/KHR/khrplatform.h ../src/linalg.h
gpuProgram.o: ../src/seq.h
headers.o: ../src/glad/include/glad/glad.h
headers.o: ../src/glad/include/KHR/khrplatform.h ../src/linalg.h
strokefont.o: ../src/headers.h ../src/glad/include/glad/glad.h
strokefont.o: ../src/glad/include/KHR/khrplatform.h ../src/linalg.h
strokefont.o: ../src/gpuProgram.h ../src/seq.h
texture.o: ../src/headers.h ../src/glad/include/glad/glad.h
texture.o: ../src/glad/include/KHR/khrplatform.h ../src/linalg.h
texture.o: ../src/gpuProgram.h ../src/seq.h
canvas.o: ../src/canvas.h ../src/headers.h
canvas.o: ../src/glad/include/glad/glad.h
canvas.o: ../src/glad/include/KHR/khrplatform.h ../src/linalg.h
canvas.o: ../src/main.h ../src/texture.h ../src/gpuProgram.h
canvas.o: ../src/seq.h ../src/drawSegs.h ../src/strokefont.h
canvas.o: ../src/editor.h
drawSegs.o: ../src/headers.h ../src/glad/include/glad/glad.h
drawSegs.o: ../src/glad/include/KHR/khrplatform.h ../src/linalg.h
drawSegs.o: ../src/drawSegs.h ../src/gpuProgram.h ../src/seq.h
editor.o: ../src/editor.h ../src/headers.h
editor.o: ../src/glad/include/glad/glad.h
editor.o: ../src/glad/include/KHR/khrplatform.h ../src/linalg.h
editor.o: ../src/texture.h ../src/gpuProgram.h ../src/seq.h
fg_stroke.o: ../src/fg_stroke.h ../src/headers.h
fg_stroke.o: ../src/glad/include/glad/glad.h
fg_stroke.o: ../src/glad/include/KHR/khrplatform.h ../src/linalg.h
gpuProgram.o: ../src/gpuProgram.h ../src/headers.h
gpuProgram.o: ../src/glad/include/glad/glad.h
gpuProgram.o: ../src/glad/include/KHR/khrplatform.h ../src/linalg.h
gpuProgram.o: ../src/seq.h
linalg.o: ../src/linalg.h
lodepng.o: ../src/lodepng.h
main.o: ../src/headers.h ../src/glad/include/glad/glad.h
main.o: ../src/glad/include/KHR/khrplatform.h ../src/linalg.h
main.o: ../src/gpuProgram.h ../src/seq.h ../src/canvas.h ../src/main.h
main.o: ../src/texture.h ../src/drawSegs.h ../src/editor.h
main.o: ../src/strokefont.h
strokefont.o: ../src/strokefont.h ../src/headers.h
strokefont.o: ../src/glad/include/glad/glad.h
strokefont.o: ../src/glad/include/KHR/khrplatform.h ../src/linalg.h
strokefont.o: ../src/gpuProgram.h ../src/seq.h ../src/fg_stroke.h
texture.o: ../src/texture.h ../src/headers.h
texture.o: ../src/glad/include/glad/glad.h
texture.o: ../src/glad/include/KHR/khrplatform.h ../src/linalg.h
texture.o: ../src/gpuProgram.h ../src/seq.h ../src/lodepng.h
