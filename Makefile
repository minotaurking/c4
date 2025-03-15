TARGET = ./code.bin

SOURCES =

PREPROCESSOR = ./preprocess.py

C4 = ./c4

GEN-CODE = ./gen-code

all: $(TARGET)

ifndef DIR
$(error Speicify a subfolder, like make DIR=test or make clean DIR=test)
endif

include $(DIR)/Makefile

PREPROCESS_FILE = $(SOURCES:.c=.cc)

$(TARGET): $(SOURCES) $(PREPROCESSOR) $(C4) $(GEN-CODE)
	python preprocess.py $<
	./c4 -s $(PREPROCESS_FILE)
	./gen-code $(PREPROCESS_FILE)
	cp $(TARGET) ~/git_work/qemu/new-calc-build

clean:
	rm -f $(TARGET)
	rm -f $(PREPROCESS_FILE)
	rm -f $(SOURCES:.c=.cc.*)

.PHONY: all clean