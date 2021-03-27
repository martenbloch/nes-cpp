g++ -c -fPIC mapper000.cpp -o mapper000.o
g++ -c -fPIC mapper001.cpp -o mapper001.o
g++ -c -fPIC mapper002.cpp -o mapper002.o
g++ -c -fPIC mapper004.cpp -o mapper004.o
g++ -c -fPIC mapper071.cpp -o mapper071.o
g++ -c -fPIC mapper232.cpp -o mapper232.o
g++ -c -fPIC utils.cpp -o utils.o
g++ -c -fPIC bus.cpp -o bus.o
g++ -c -fPIC cartridge.cpp -o cartridge.o
g++ -c -fPIC ram.cpp -o ram.o
g++ -c -fPIC ppu.cpp -o ppu.o
g++ -c -fPIC apu.cpp -o apu.o
g++ -c -fPIC addressModes.cpp -o addressModes.o
g++ -c -fPIC instructions.cpp -o instructions.o
g++ -c -fPIC cpu.cpp -o cpu.o
g++ -c -fPIC controller.cpp -o controller.o
g++ -c -fPIC nes.cpp -o nes.o
g++ -c -fPIC libNesApi.cpp -o libNesApi.o
g++ -shared -Wl,-soname,libNesApi.so -o libNesApi.so libNesApi.o nes.o controller.o cpu.o instructions.o addressModes.o apu.o ppu.o ram.o cartridge.o bus.o utils.o mapper000.o mapper002.o mapper071.o mapper232.o mapper001.o mapper004.o
mv libNesApi.so ../../nes_emulator/src/cpu/
rm *.o
