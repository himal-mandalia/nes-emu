GPP = g++
EXE = nes

all:
	$(GPP) `sdl-config --cflags --libs` -Wall -g *.cpp -o $(EXE)
    
clean:
	rm $(EXE)

