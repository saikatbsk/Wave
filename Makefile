OUTPUT = Wave
LIBS_OPENCV = `pkg-config --libs opencv`

all : $(OUTPUT)

$(OUTPUT) : main.cpp Hand.cpp MyROI.cpp Image.cpp
	g++ -o $@ main.cpp Hand.cpp MyROI.cpp Image.cpp $(LIBS_OPENCV) -Wunused-variable

clean :
	rm -v $(OUTPUT)
