RMF = rm -rf

CC = g++
CO = g++ -c -g

LIBS = -lSDL2 -lSDL2_mixer -lSDL2_image -g

HEADERS = sdl_drums.h \
		  button.h \
		  sound_button.h\
		  drum_loop.h \
          sound_data.h \
		  trig_button.h \
		  control_button.h \
          step_button.h \
          util.h

SOURCES = sdl_drums.cpp \
		  button.cpp \
          sound_button.cpp\
		  drum_loop.cpp \
          sound_data.cpp \
		  trig_button.cpp \
		  control_button.cpp \
          step_button.cpp \
          util.cpp

OBJECTS = sdl_drums.o \
		  button.o \
		  sound_button.o\
          drum_loop.o \
          sound_data.o \
		  trig_button.o \
          control_button.o \
		  step_button.o \
          util.o

TARGET = sdl_drums

.SUFFIXES: .cpp
.cpp.o:
	$(CO) $< -o $@

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)

clean:
	$(RMF) $(OBJECTS)
	$(RMF) $(TARGET)

sdl_drums.o: sdl_drums.cpp sdl_drums.h sound_button.h drum_loop.h sound_data.h \
	button.h trig_button.h control_button.h step_button.h util.h
button.o: button.cpp button.h
sound_button.o: sound_button.cpp sound_button.h sound_data.h button.h
drum_loop.o: drum_loop.cpp drum_loop.h sdl_drums.h
sound_data.o: sound_data.cpp sound_data.h
trig_button.o: trig_button.cpp trig_button.h button.h drum_loop.h
control_button.o: control_button.cpp control_button.h button.h
step_button.o: step_button.cpp step_button.h button.h
util.o: util.cpp util.h
