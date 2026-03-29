#include<iostream>
#include<array>
#include<cstring>
#include<vector>
#include<fstream>

using namespace std;

class Chip8 {
	public:	
		array<unsigned char, 4096> memory{};
		array<unsigned char, 16> V{};
		unsigned short I = 0;
		unsigned short pc = 0x200;
		bool display[2048]={};  
		
		unsigned char fontset[80] ={
			0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
0x20, 0x60, 0x20, 0x20, 0x70, // 1
0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
0x90, 0x90, 0xF0, 0x10, 0x10, // 4
0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
0xF0, 0x10, 0x20, 0x40, 0x40, // 7
0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
0xF0, 0x90, 0xF0, 0x90, 0x90, // A
0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
0xF0, 0x80, 0x80, 0x80, 0xF0, // C
0xE0, 0x90, 0x90, 0x90, 0xE0, // D
0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


		void loadROM(const std::string& filename) {
    			std::ifstream file(filename, std::ios::binary | std::ios::ate);

    			if (!file.is_open()) {
        			std::cout << "Failed to open ROM\n";
        			return;
    				}

    			std::streamsize size = file.tellg();
    			file.seekg(0, std::ios::beg);

    			std::vector<char> buffer(size);

    			file.read(buffer.data(), size);

    			// Load into memory starting at 0x200
   			for (int i = 0; i < size; i++) {
        			memory[0x200 + i] = buffer[i];
    				}
			}

void printDisplay() {
                                        for (int y = 0; y < 32; y++) {
                                                 for (int x = 0; x < 64; x++) {
                                                        cout << (display[y * 64 + x] ? "#" : ".");
                                                }
                                        cout << "\n";
                                        }
                                cout << "----------------------\n";
                                }
		
		Chip8() {
			pc =0x200;
			cout << "Chip8 started";
			for(int i=0; i< 80; i++) {
				memory[0x50 + i] = fontset[i];
			}

		}
};


int main() {
	Chip8 chip;

	chip.loadROM("ibm.ch8"); for (int i = 0; i < 20; i++) {
    printf("%02X ", chip.memory[0x200 + i]);
}
cout << endl;
	bool running = true;

	while(running) {
		// so the instructions are of 2 byte , this opcode is of a 4 nibble , 1 nibble is 4 bits , in pc counter it takes 2 bytes so we increment by 2
		unsigned short opcode = chip.memory[chip.pc] << 8 | chip.memory[chip.pc + 1];
		chip.pc += 2;
		int x = (opcode & 0x0F00) >> 8;
		int y = (opcode & 0x00F0) >> 4;
		int N = opcode & 0x000F;
		switch (opcode & 0xF000) {
			
			case 0x0000:{
				// clear screen
				if (opcode == 0x00E0){
					memset(chip.display, 0, sizeof(chip.display));
				}
			break;
				    }
			
			// write to pc 
			case 0x1000:{
				chip.pc = opcode & 0x0FFF;
				break;
				    }
			case 0x7000: {
    chip.V[x] += opcode & 0x00FF;
    break;
}
			case 0x6000:{
				chip.V[x] = opcode & 0x00FF;
				break;}

			case 0xA000:{
				chip.I = opcode & 0x0FFF;
				break;
				    }
				
			//draw screen 
			case 0xD000: {
				chip.V[0XF] = 0;

				for (int row = 0; row < N ; row++) {
					unsigned char byte =chip.memory[chip.I + row];
					for (int bit = 0; bit < 8 ; bit++){
						if (byte & (0x80 >> bit)) {
						int x_pos = chip.V[x] + bit;
						int y_pos = chip.V[y] + row;
						x_pos %= 64;
y_pos %= 32;
						int index = y_pos *64 + x_pos;

						if (chip.display[index] == 1) {
								chip.V[0xF] = 1;
						}
						chip.display[index] ^= 1;
					}
					}
				}
static bool printed = false;
if (!printed) {
    chip.printDisplay();
}
			break;
		
				     }}


	
	}	
	return 0;		
}
