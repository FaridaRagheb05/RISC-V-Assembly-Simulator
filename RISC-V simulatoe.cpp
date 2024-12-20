#include<iostream>
#include <vector>
#include <string>
#include <fstream>
#include <limits>
using namespace std;

int PC; // Program Counter
int initialPC;
int reg[32]; // Array of 32 registers
vector<pair<int, int> > memory; // Memory representation (not used in this code)
vector<string> instruction; // Vector to store instructions
//const reg[0] = 0; // should not be able to be modified

void driversprogram() {
    cout << "Please enter the starting address of the program: ";
    cin >> PC; // Set the starting address
    initialPC = PC;
    for (int i = 0; i < 32; i++) {
        reg[i] = 0; // Initialize registers to 0
    }
}

void read_input_from_file(const string& filename) {
    ifstream inputFile(filename);
    if (!inputFile) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    int i = 0;
    int increment = 0;
    while (getline(inputFile, line)) {
        /* if (line == "HALT") { // Stop //(Halting should be done by ECALL, EBREAK, PAUSE, FENCE, and FENCE.TSO)
            break;
        } */ 
        instruction.push_back(line); // Store the instruction
        pair<int, string> x (PC + increment, line);
        memory.push_back(x);
        increment += 4;
        i++;
    } 
    for (int i = 0; i < memory.size(); i++)
    {
        cout << memory[i].first << " " << memory[i].second << endl;
    }
    inputFile.close(); // Close the file
}

void simulator(int& programcount) {
    string action;
    int i = 0;
    //every iteration in the loop one must change i and PC and size according to the 
    // instruction that is currently executed
    while(true){
        action = "";
        string rd = "";
        string imm = "";

        // Extract the action (instruction) part
        for (int j = 0; j < instruction[i].size(); j++) {
            if (instruction[i][j] == ' ') {
                break;
            }
            action += instruction[i][j];
        }

        cout << action << endl;
        if (action == "ECALL" || "EBREAK" || "PAUSE" || "FENCE" || "FENCE.TSO") // Stop the program
        {
            return;
        }

        if (action == "LUI") {
            int rdIndex;
            int immediateValue;
            PC = PC + 4;
            

            // Parse `rd` and `imm` from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);
                imm = instruction[i].substr(instruction[i].find(",") + 1);
            }

            // Convert `rd` to register index (assuming rd is in the form "x0", "x1", etc.)
            rdIndex = stoi(rd.substr(1)); // Convert "x1" to 1, "x2" to 2, etc.

            // Convert `imm` (string) to an integer directly as decimal
            immediateValue = stoi(imm); // Treat the input as a decimal value

            // Perform LUI operation (load upper immediate)
            reg[rdIndex] = immediateValue * 4096; // Store the immediate value multiplied by 4096

            cout << "LUI: Loaded " << (immediateValue * 4096) << " into register " << rd << endl;
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;
            cout << rdIndex << endl;
            cout << reg[rdIndex] << endl;
            i++;
        }
        else if (action == "AUIPC")
        {
            int rdIndex;
            int immediateValue;
            // Parse `rd` and `imm` from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);
                imm = instruction[i].substr(instruction[i].find(",") + 1);
            }

            // Convert `rd` to register index (assuming rd is in the form "x0", "x1", etc.)
            rdIndex = stoi(rd.substr(1)); // Convert "x1" to 1, "x2" to 2, etc.

            // Convert `imm` (string) to an integer directly as decimal
            immediateValue = stoi(imm); // Treat the input as a decimal value

            // Perform AUIPC operation (add immediate to PC and store in rd)
            reg[rdIndex] = PC + (immediateValue << 12); // Store the PC value + (imm << 12) in register

            cout << "AUIPC: Loaded " << reg[rdIndex] << " into register " << rd << endl;
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;
            i++;

        }
        else if (action == "JAL") 
        {
            int rdIndex;
            int immediateValue;
           
            PC = PC + 4;
            // Parse `rd` and `imm` from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);
                imm = instruction[i].substr(instruction[i].find(",") + 1);
            }

            // Convert `rd` to register index (assuming rd is in the form "x0", "x1", etc.)
            rdIndex = stoi(rd.substr(1)); // Convert "x1" to 1, "x2" to 2, etc.

            // Convert `imm` (string) to an integer directly as decimal
            cout << imm << endl;
            immediateValue = stoi(imm); // Treat the input as a decimal value

            cout << immediateValue;
            int nextPC = PC + 4;
            PC += immediateValue;
            cout << PC << endl;

            // Save the return address in rd
            reg[rdIndex] = nextPC;

            
            cout << "JAL: Jumping to address " << PC << " and link to register " << rd << " with return address " << nextPC << endl;
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;
            i = i+immediateValue/4 ;
        }
        else if (action == "JALR")
        {
            int rdIndex, rs1Index;
            int immediateValue;

            // Parse `rd`, `rs1`, and `imm` from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                // Find `rs1` by locating the second comma and extracting the register
                size_t pos2 = instruction[i].find(",", pos + 1);
                string X= instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1).substr(1);
                if (X.length() == 3)
                {
                    X = X.erase(0, 1);
                }
                rs1Index = stoi(X);

                // Extract `imm` after the last comma
                imm = instruction[i].substr(instruction[i].rfind(",") + 1);
            }

            // Convert `rd` to register index (assuming rd is in the form "x0", "x1", etc.)
            rdIndex = stoi(rd.substr(1)); // Convert "x1" to 1, "x2" to 2, etc.

            // Convert `imm` (string) to an integer directly as decimal
            immediateValue = stoi(imm); // Treat the input as a decimal value
            cout << immediateValue << " " << rs1Index << " " << reg[rs1Index];

            // Update PC to the address in rs1 plus immediate value
            PC = reg[rs1Index] + immediateValue;
            cout << "PC: " << PC << endl;

            cout << "JALR: Jumping to address " << PC << " from register " << "x" << rs1Index << endl;
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;
            //exit(0);
            // Update loop index based on new PC value
            i = (PC - initialPC) / 4 ;
        }

        else if (action == "SUB"){
            int rdIndex, rs1Index, rs2Index;
            string rd, rs1, rs2;
            // Parsing of rd, rs1, rs2 from instruction
            size_t pos = instruction[i].find(" "); // First space after the instruction name
            size_t pos2 = instruction[i].find(",", pos + 1); // First comma
            size_t pos3 = instruction[i].find(",", pos2 + 1); // Second comma


            if (pos != string::npos){
                // Extracting rd
                rd = instruction[i].substr(pos + 1, pos2 - pos - 1);  
                rdIndex = stoi(rd.substr(1)); // converts register number into its index (e.g. x1 -> 1)

                // Extracting rs1 
                rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1);  
                rs1Index = stoi(rs1.substr(1));

                // Extracting rs2
                rs2 = instruction[i].substr(pos3 + 1);
                rs2Index = stoi(rs2.substr(1));
            }
            reg[rdIndex] = reg[rs1Index] - reg[rs2Index]; // The subtraction process itself

            cout << "SUB: Subtracting x" << rs2Index << " from x" << rs1Index << " and storing result in x" << rdIndex << endl;
            cout << "Register x" << rdIndex << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

            PC += 4; // updating program counter
            i++; // moving to the next instruction
        }

        else if (action == "SLL"){
            int rdIndex, rs1Index, rs2Index;
            string rd, rs1, rs2;

            // Parsing rd, rs1, and rs2 from instruction
            size_t pos = instruction[i].find(" ");
            size_t pos2 = instruction[i].find(",", pos + 1);
            size_t pos3 = instruction[i].find(",", pos2 + 1);

            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs1
                rs2 = instruction[i].substr(pos3 + 1); // Extract rs2
            }
            // converts register number into its index
            rdIndex = stoi(rd.substr(1));    
            rs1Index = stoi(rs1.substr(1));
            rs2Index = stoi(rs2.substr(1));

            // Shift the value in rs1 by the amount in rs2
            reg[rdIndex] = reg[rs1Index] << (reg[rs2Index]); 
       
            cout << "SLL: Shifted value in register " << rs1 << " left by " << (reg[rs2Index]) 
                << " positions. Stored result in register " << rd << endl;
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

            PC += 4;
            i++;  
        }


        else if (action == "SLT"){
            int rdIndex, rs1Index, rs2Index;
            string rd, rs1, rs2;

            // Parse rd, rs1, and rs2 from the instruction
            size_t pos = instruction[i].find(" "); 
            size_t pos2 = instruction[i].find(",", pos + 1); 
            size_t pos3 = instruction[i].find(",", pos2 + 1); 

            if (pos != string::npos) {

                rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs1
                rs2 = instruction[i].substr(pos3 + 1); // Extract rs2
            }

            // converts register number into its index
            rdIndex = stoi(rd.substr(1));
            rs1Index = stoi(rs1.substr(1));
            rs2Index = stoi(rs2.substr(1));

            // Perform the SLT operation
            if (reg[rs1Index] < reg[rs2Index]) {
                reg[rdIndex] = 1; // if rs1 less rs2 than, set value to 1
            } else {
                reg[rdIndex] = 0; // if not, set to 0
            }

            cout << "SLT: Compared registers " << rs1 << " and " << rs2 << ". ";
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;


            PC += 4; 
            i++;
        }

        else if (action == "SLTU") {
            int rdIndex, rs1Index, rs2Index;
            string rd, rs1, rs2; 

            // Parse rd, rs1, and rs2 from the instruction
            size_t pos = instruction[i].find(" "); 
            size_t pos2 = instruction[i].find(",", pos + 1); 
            size_t pos3 = instruction[i].find(",", pos2 + 1); 

            if (pos != string::npos) {
            
                rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs1
                rs2 = instruction[i].substr(pos3 + 1); // Extract rs2
            }

            // converts register number into its index
            rdIndex = stoi(rd.substr(1));
            rs1Index = stoi(rs1.substr(1));
            rs2Index = stoi(rs2.substr(1));

            // Perform the SLTU operation (same as SLT but unsigned)
            if (static_cast<unsigned>(reg[rs1Index]) < static_cast<unsigned>(reg[rs2Index])) {
                reg[rdIndex] = 1;
            } else {
                reg[rdIndex] = 0;
            }

  
            cout << "SLTU: Compared registers " << rs1 << " and " << rs2 << ". ";
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

            PC += 4;
            i++;
        }

        else if (action == "SRL") {
            int rdIndex, rs1Index, rs2Index;
            string rd, rs1, rs2;

            // Parse rd, rs1, and rs2 from the instruction
            size_t pos = instruction[i].find(" ");
            size_t pos2 = instruction[i].find(",", pos + 1);
            size_t pos3 = instruction[i].find(",", pos2 + 1);

            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1);  // Extract rs1
                rs2 = instruction[i].substr(pos3 + 1); // Extract rs2
            }

            // Convert register number into its index
            rdIndex = stoi(rd.substr(1));
            rs1Index = stoi(rs1.substr(1));
            rs2Index = stoi(rs2.substr(1));

            // Perform the shift right logical operation
            reg[rdIndex] = static_cast<unsigned>(reg[rs1Index]) >> (reg[rs2Index]); 


            cout << "SRL: Shifted value in register " << rs1 << " right by " << (reg[rs2Index])
                << " positions. Stored result in register " << rd << endl;
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

            PC += 4;
            i++;
        }

        else if (action == "XOR") {

            int rdIndex, rs1Index, rs2Index;
            string rd, rs1, rs2; 

            // Parse rd, rs1, and rs2 from the instruction
            size_t pos = instruction[i].find(" "); 
            size_t pos2 = instruction[i].find(",", pos + 1); 
            size_t pos3 = instruction[i].find(",", pos2 + 1); 

            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs1
                rs2 = instruction[i].substr(pos3 + 1); // Extract rs2
            }

            // Convert register number into its index
            rdIndex = stoi(rd.substr(1));
            rs1Index = stoi(rs1.substr(1));
            rs2Index = stoi(rs2.substr(1));

            // Perform the bitwise XOR operation
            reg[rdIndex] = reg[rs1Index] ^ reg[rs2Index];

            cout << "XOR: XORed registers " << rs1 << " and " << rs2 
                << ". Stored result in register " << rd << endl;
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

            PC += 4;
            i++;
        }

        else if (action == "OR") {
            int rdIndex, rs1Index, rs2Index;
            string rd, rs1, rs2;

            // Parse rd, rs1, and rs2 from the instruction
            size_t pos = instruction[i].find(" ");
            size_t pos2 = instruction[i].find(",", pos + 1);
            size_t pos3 = instruction[i].find(",", pos2 + 1);

            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs1
                rs2 = instruction[i].substr(pos3 + 1); // Extract rs2
            }

            // Convert register numbers to indices
            rdIndex = stoi(rd.substr(1));
            rs1Index = stoi(rs1.substr(1));
            rs2Index = stoi(rs2.substr(1));

            // ORing rs1 and rs2 (bitwise OR)
            reg[rdIndex] = reg[rs1Index] | reg[rs2Index];


            cout << "OR: Performed bitwise OR between registers " << rs1 << " and " << rs2;
            cout << ". Stored result in register " << rd << endl;
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;
 
            PC += 4; 
            i++;
        }

        else if (action == "AND") {
            int rdIndex, rs1Index, rs2Index;
            string rd, rs1, rs2;

            // Parse rd, rs1, and rs2 from the instruction
            size_t pos = instruction[i].find(" ");
            size_t pos2 = instruction[i].find(",", pos + 1);
            size_t pos3 = instruction[i].find(",", pos2 + 1);

            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs1
                rs2 = instruction[i].substr(pos3 + 1); // Extract rs2
            }

            // Convert register numbers to indices
            rdIndex = stoi(rd.substr(1));
            rs1Index = stoi(rs1.substr(1));
            rs2Index = stoi(rs2.substr(1));

            // AND: rs1 & rs2 (bitwise AND)
            reg[rdIndex] = reg[rs1Index] & reg[rs2Index];

            cout << "AND: Performed bitwise AND between registers " << rs1 << " and " << rs2;
            cout << ". Stored result in register " << rd << endl;
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

            PC += 4; 
            i++;
        }

        else if (action == "ADD"){
            int rdIndex, rs1Index, rs2Index;
            string rd, rs1, rs2;

            // Parse rd, rs1, and rs2 from the instruction
            size_t pos = instruction[i].find(" ");
            size_t pos2 = instruction[i].find(",", pos + 1);
            size_t pos3 = instruction[i].find(",", pos2 + 1);

            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1);  // Extract rs1
                rs2 = instruction[i].substr(pos3 + 1); // Extract rs2
            }

            // Convert register names to indices
            rdIndex = stoi(rd.substr(1));
            rs1Index = stoi(rs1.substr(1));
            rs2Index = stoi(rs2.substr(1));

            // Perform the addition
            reg[rdIndex] = reg[rs1Index] + reg[rs2Index];

            cout << "ADD: Added registers " << rs1 << " and " << rs2;
            cout << ". Stored result in register " << rd << endl;
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

            PC += 4; // update program counter
            i++;
        }

        
        else if (action == "SRA") {
            int rdIndex, rs1Index, rs2Index;
            string rd, rs1, rs2;

            // Parse rd, rs1, and rs2 from the instruction
            size_t pos = instruction[i].find(" ");
            size_t pos2 = instruction[i].find(",", pos + 1);
            size_t pos3 = instruction[i].find(",", pos2 + 1);

            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs1
                rs2 = instruction[i].substr(pos3 + 1); // Extract rs2
            }

            // Convert register number into its index
            rdIndex = stoi(rd.substr(1));
            rs1Index = stoi(rs1.substr(1));
            rs2Index = stoi(rs2.substr(1));

            // SRA (preserves the sign value)
            if (reg[rs1Index] < 0) {

                // to manually extend the sign bit for negative numbers
                reg[rdIndex] = (reg[rs1Index] >> reg[rs2Index]) | (0xFFFFFFFF << (32 - reg[rs2Index]));
                // that way, if the value is negative, it does the regular shift right operation
                // then, it masks the bits placed after the shifting by using a bitwise OR operation with a 32-bit value of 1

            } else {

                // a regular right shift for positive numbers
                reg[rdIndex] = reg[rs1Index] >> reg[rs2Index];
            }



            cout << "SRA: Shifted value in register " << rs1 << " right (arithmetic) by " 
                << (reg[rs2Index]) << " positions. Stored result in register " << rd << endl;
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;


            PC += 4;
            i++;
        }

        else if (action == "SRAI") {
            int rdIndex, rs1Index;
            string rd, rs1;
            int immediate;

            // Parse rd, rs1, and immediate from the instruction
            size_t pos = instruction[i].find(" ");
            size_t pos2 = instruction[i].find(",", pos + 1);
            size_t pos3 = instruction[i].find(",", pos2 + 1);

            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs1
                immediate = stoi(instruction[i].substr(pos3 + 1)); // Extract immediate value and convert to integer
            }

            // Convert register names to indices
            rdIndex = stoi(rd.substr(1));
            rs1Index = stoi(rs1.substr(1));

            // Performs arithmetic shift by immediate value (preserves the sign bit)
            reg[rdIndex] = reg[rs1Index] >> (immediate); 

            cout << "SRAI: Shifted register " << rs1 << " right by " << (immediate) 
                << " positions. Stored result in register " << rd << endl;
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

            PC += 4; 
            i++;
        }

        else if (action == "SRLI") {
            int rdIndex, rs1Index;
            int immediate;

            // Parse rd, rs1, and immediate from the instruction
            size_t pos = instruction[i].find(" ");
            size_t pos2 = instruction[i].find(",", pos + 1);
            size_t pos3 = instruction[i].find(",", pos2 + 1);

            if (pos != string::npos) {
                string rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                string rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs1
                string immediate_string = instruction[i].substr(pos3 + 1); // Extract immediate

                // Convert register names to indices
                rdIndex = stoi(rd.substr(1));                                      
                rs1Index = stoi(rs1.substr(1));   
                immediate = stoi(immediate_string);                           
            }

            // Perform the SRLI operation (logical right shift)
            reg[rdIndex] = static_cast<unsigned>(reg[rs1Index]) >> (immediate);

            cout << "SRLI: Shifted value in register x" << rs1Index << " right by " << immediate 
                << " positions. Stored result in register x" << rdIndex << endl;
            cout << "Register x" << rdIndex << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

            
            PC += 4; 
            i++;
        }

        else if (action == "SLLI") {
            int rdIndex, rs1Index;
            int immediate;

            // Parse rd, rs1, and immediate from the instruction
            size_t pos = instruction[i].find(" ");
            size_t pos2 = instruction[i].find(",", pos + 1);
            size_t pos3 = instruction[i].find(",", pos2 + 1);

            if (pos != string::npos) {
                string rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                string rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs1
                string immediate_string = instruction[i].substr(pos3 + 1); // Extract immediate

                // Convert register names to indices
                rdIndex = stoi(rd.substr(1));                                      
                rs1Index = stoi(rs1.substr(1));    
                immediate = stoi(immediate_string);                                  
            }

            // Perform the SLLI operation (logical left shift)
            reg[rdIndex] = reg[rs1Index] << (immediate);

            cout << "SLLI: Shifted value in register x" << rs1Index << " left by " << immediate 
                << " positions. Stored result in register x" << rdIndex << endl;
            cout << "Register x" << rdIndex << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

            PC += 4;
            i++;
        }

        else if (action == "BLTU"){
            int rs1Index, rs2Index, offset;

            // Parse rd, rs1, and immediate from the instruction
            size_t pos = instruction[i].find(" ");
            size_t pos2 = instruction[i].find(",", pos + 1);
            size_t pos3 = instruction[i].find(",", pos2 + 1);

            if (pos != string::npos) {
                string rs1 = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rs1
                string rs2 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs2
                string offset_string = instruction[i].substr(pos3 + 1); // Extract offset

                // Convert register names to indices
                rs1Index = stoi(rs1.substr(1));                                      
                rs2Index = stoi(rs2.substr(1));  
                offset = stoi(offset_string);                               
            }
            if (static_cast<unsigned>(reg[rs1Index]) < static_cast<unsigned>(reg[rs2Index])){
                PC += offset;
                i = (PC - initialPC) / 4; // move to the next instruction according to where the PC is pointing to
            }
            else {
                PC += 4;
                i++;
            }
        }

        else if (action == "BGEU"){
            int rs1Index, rs2Index, offset;

            // Parse rd, rs1, and immediate from the instruction
            size_t pos = instruction[i].find(" ");
            size_t pos2 = instruction[i].find(",", pos + 1);
            size_t pos3 = instruction[i].find(",", pos2 + 1);

            if (pos != string::npos) {
                string rs1 = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rs1
                string rs2 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs2
                string offset_string = instruction[i].substr(pos3 + 1); // Extract offset

                // Convert register names to indices
                rs1Index = stoi(rs1.substr(1));                                      
                rs2Index = stoi(rs2.substr(1));  
                offset = stoi(offset_string);                               
            }
            if (static_cast<unsigned>(reg[rs1Index]) >= static_cast<unsigned>(reg[rs2Index])){
                PC += offset;
                i = (PC - initialPC) / 4; // move to the next instruction according to where the PC is pointing to
            }
            else {
                PC += 4;
                i++;
            }
        }


        // Add additional instructions here as needed
        else {
            cout << "Unknown instruction: " << action << endl;
        }
        cout << endl;
        cout << "I" << i << endl;
        cout << "PC" << PC << endl;
        cout << endl;
    }

}

int main() {
    // Start of the driver program
    driversprogram();
    // End of the driver program

    // Read the RISC-V code from a file
    string filename = "/Users/faridaragheb/Desktop/RISC-V-RV32I-Simulator/testcase3.txt";
    ifstream infile(filename);

    // Check if the file opens successfully
    if (!infile.is_open()) {
        cerr << "Error: Could not open the file " << filename << endl;
        return 1;  // Exit with an error if file opening fails
    }

    // Read the RISC-V code from the file
    read_input_from_file(filename);

    // Start computing the RISC-V code
    simulator(PC);
    // End of computing the RISC-V code

    infile.close();  // Close the file after reading

    return 0;
}





