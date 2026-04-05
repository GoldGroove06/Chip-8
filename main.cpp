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

	chip.loadROM("ibm.ch8");
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
			
			// write to pc (Jump to 0x0NNN)
			case 0x1000:{
				chip.pc = opcode & 0x0FFF;
				break;
				}
			// Add value to register VX 
			case 0x7000: {
    				chip.V[x] += opcode & 0x00FF;
    				break;
				}
			// set register VX
			case 0x6000:{
				chip.V[x] = opcode & 0x00FF;
				break;
				}

			// set register VX to VY
			case 0x8000:{
				switch (opcode & 0x000F){
					case 0x0: {
					    	chip.V[y] = chip.V[x];
						break;
						  }

					case 0x1: {
						chip.V[x] = chip.V[x] | chip.V[y];
						break;
						  }
					case 0x2: {
						chip.V[x] = chip.V[x] & chip.V[y];
						break;
						  }
					case 0x3: {
						chip.V[x] = chip.V[x] ^ chip.V[y];
						break;
						  }
					case 0x4: {
						int sum = chip.V[x] + chip.V[y];

						if (sum >255) chip.V[0xF] = 1;
						else chip.V[0xF] = 0;
						chip.V[x] = sum & 0xFF;
						break;
						  }
					case 0x5: {
						if (chip.V[x] >= chip.V[y]) chip.V[0xF] = 1;
						else chip.V[0xF] = 0;

						chip.V[x] = chip.V[x] - chip.V[y];
						break;
						  }
					case 0x7: {
						if (chip.V[x] >= chip.V[y]) chip.V[0xF] = 1;
						else chip.V[0xF] = 0;

						chip.V[x] = chip.V[y] - chip.V[x];
						break;
						  }
					case 0x6: {
						chip.V[0xF] = chip.V[x] & 0x01;
						chip.V[x] >>= 1;
						break;
						  }
					case 0xE:  {
						chip.V[0xF] = (chip.V[x] & 0x80) >> 7;
						chip.V[x] <<= 1;
						break;
						  }
						}
				break;
				}
			//set register I 0xANNN
			case 0xA000:{
				chip.I = opcode & 0x0FFF;
				break;
				}
			
			// skip if 3xNN is equal to VX
			case 0x3000:{
				if (chip.V[x] == (opcode & 0x00FF)){
					chip.pc += 2;
				}
				break;
				    }
			
			// skip if not equal 4xNN to VX
			case 0x4000:{
				if(chip.V[x] != (opcode & 0x00FF)){
					chip.pc += 2;
				}
				break;
				    }

			// skip if VX is equal to VY
			case 0x5000:{
				if ( (opcode & 0x000F) == 0){ 
				if(chip.V[x] == chip.V[y]){
					chip.pc += 2;
				}
				}
				break;
				    }
			// skip if VX is equal to VY		    
			case 0x9000:{
				if ( (opcode & 0x000F) == 0){ 
				if(chip.V[x] != chip.V[y]){
					chip.pc += 2;
				}
				}
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
    			chip.printDisplay();

			break;
		
				     }}


	
	}	
	return 0;		
}
