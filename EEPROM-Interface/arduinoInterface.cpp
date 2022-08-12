// -l:C:\gcc\i686-w64-mingw32\lib\libcomdlg32.a

#include <windows.h>

#include <cstdlib>

#include <string>
#include <cstring>

#include <iostream>
#include <sstream>
#include <fstream>

class Serial {
      private:
            HANDLE xComm;
            DCB dcbSerialParams;
            COMMTIMEOUTS timeouts;
      public:
            Serial(bool* status) {
                  *status= 0;
                  
                  // Open Serial port
                  this->xComm= CreateFileA(
                              "\\\\.\\COM5",                            // Port name
                              GENERIC_READ | GENERIC_WRITE,             // Read/Write
                              0,                                        // No Sharing
                              NULL,                                     // No Security
                              OPEN_EXISTING,                            // Open existing port only
                              0,                                        // Non Overlapped I/O
                              NULL                                      // Null for Comm Devices
                  );
                  
                  // Error Checking
                  if (this->xComm == INVALID_HANDLE_VALUE) {
                        std::cout << "Error opening serial port" << std::endl;
                        *status= 1;
                        return;
                  }
                  
                  // Setting the DCB Parameters for the SerialPort
                  this->dcbSerialParams.DCBlength= sizeof(this->dcbSerialParams);
                  
                  if (!GetCommState(this->xComm, &this->dcbSerialParams)) {
                        std::cout << "Error getting device state" << std::endl;
                        CloseHandle(this->xComm);
                        *status= 1;
                        return;
                  }
                  
                  this->dcbSerialParams.BaudRate= CBR_9600;             // BaudRate= 9600
                  this->dcbSerialParams.ByteSize= 8;                    // ByteSize= 8
                  this->dcbSerialParams.StopBits= ONESTOPBIT;           // StopBits= 1
                  this->dcbSerialParams.Parity= NOPARITY;               // Parity= None
                  
                  if (!SetCommState(this->xComm, &this->dcbSerialParams)) {
                        std::cout << "Error setting device parameters" << std::endl;
                        CloseHandle(this->xComm);
                        *status= 1;
                        return;
                  }
                  
                  // Set COM port timeout settings
                  this->timeouts.ReadIntervalTimeout= 50;
                  this->timeouts.ReadTotalTimeoutConstant= 50;
                  this->timeouts.ReadTotalTimeoutMultiplier= 10;
                  this->timeouts.WriteTotalTimeoutConstant= 50;
                  this->timeouts.WriteTotalTimeoutMultiplier= 10;
                  
                  if (!SetCommTimeouts(this->xComm, &this->timeouts)) {
                        std::cout << "Error setting timeout parameters" << std::endl;
                        CloseHandle(this->xComm);
                        *status= 1;
                        return;
                  }
                  
                  return;
            }
            
            ~Serial() {
                  CloseHandle(this->xComm);                             // Closing the Serial Port
                  
                  return;
            }
            
            std::string readString(bool* status) {
                  *status= 0;
                  
                  DWORD eventMask= 0, bytesRead= 0;
                  std::string input;
                  char newChar;
                  
                  // Set receive mask
                  if (!SetCommMask(this->xComm, EV_RXCHAR)) {
                        std::cout << "Error setting COM mask" << std::endl;
                        CloseHandle(this->xComm);
                        *status= 1;
                        return NULL;
                  }

                  // Set event mask
                  if (!WaitCommEvent(this->xComm, &eventMask, NULL)) {
                        std::cout << "Error setting WaitCommEvent()" << std::endl;
                        CloseHandle(this->xComm);
                        *status= 1;
                        return NULL;
                  }

                  //Read data and store in a buffer
                  do {
                        ReadFile(this->xComm, &newChar, sizeof(newChar), &bytesRead, NULL);
                        if (bytesRead) {
                              input.push_back(newChar);
                        }
                  } while (bytesRead);
                  
                  return input;
            }
            
            void writeString(std::string data, bool* status) {
                  *status= 0;
                  
                  DWORD bytesWritten= 0;
                  int length= data.length();
                  char charData[length + 1];
                  strcpy(charData, data.c_str());
                  
                  WriteFile(this->xComm, charData, length + 1, &bytesWritten, NULL);
                  
                  return;
            }
            
            void writeByte(char data, bool* status) {
                  *status= 0;
                  
                  DWORD bytesWritten= 0;
                  char charData[1];
                  charData[0]= data;
                  
                  WriteFile(this->xComm, charData, 1, &bytesWritten, NULL);
                  
                  return;
            }
};

class EEPROM {
      private:
            bool status;
            Serial* arduino;
      public:
            EEPROM() {
                  arduino= new Serial(&status);
                  if (status) {
                        return;
                  }
                  
                  std::cout << "Reboot your Arduino" << std::endl;
                  std::string newStr= arduino->readString(&status);
                  std::cout << newStr << std::endl;
                  
                  return;
            }
            
            ~EEPROM() {
                  delete arduino;
                  
                  return;
            }
            
            void wait() {
                  arduino->readString(&status);
            }

            bool getStatus() {
                  return status;
            }
            
            void newChip(int pins) {
                  arduino->writeByte(0x00, &status);
                  arduino->writeByte(pins, &status);
                  
                  return;
            }
            
            void delChip() {
                  arduino->writeByte(0x01, &status);
                  
                  return;
            }
            
            int readChip(int address) {
                  arduino->writeByte(2, &status);
                  arduino->writeByte(address >> 8, &status);
                  arduino->writeByte(address & 0xff, &status);
                  std::string input= arduino->readString(&status);
                  std::stringstream inputStream(input);
                  int data= 0;
                  inputStream >> data;
                  
                  return data;
            }
            
            void writeChip(int data, int address) {
                  arduino->writeByte(3, &status);
                  arduino->writeByte(address >> 8, &status);
                  arduino->writeByte(address & 0xff, &status);
                  arduino->writeByte(data, &status);
                  
                  return;
            }
};

std::string getFilePath();

int main() {
      EEPROM* chip= new EEPROM();
      
      std::string command;
      do {
            std::cout << ">>> ";
            std::string input;
            std::cin >> input;
            std::cin.sync();
            command= input;
            if (command == "clear") {
                  std::system("cls");
                  
            } else if (command == "new") {
                  int pins;
                  std::cout << "   Number of pins: ";
                  std::cin >> pins;
                  std::cin.sync();
                  chip->newChip(pins);
                  
            } else if (command == "del") {
                  chip->delChip();
                  
            } else if (command == "read") {
                  int address;
                  int data;
                  std::cout << "   Address: ";
                  std::cin >> address;
                  std::cin.sync();
                  data= chip->readChip(address);
                  std::cout << "0x" << std::hex << address << ": 0x" << std::hex << data << std::endl;
                  
            } else if (command == "write") {
                  int address;
                  int data;
                  std::cout << "   Address: ";
                  std::cin >> address;
                  std::cin.sync();
                  std::cout << "   Data: ";
                  std::cin >> std::hex >> data;
                  std::cin.sync();
                  chip->writeChip(data, address);
                  chip->wait();
                  
            } else if (command == "flash") {
                  std::string path;
                  int startAddress;
                  std::cout << "   Path: ";
                  path= getFilePath();
                  std::cout << path << std::endl;
                  std::cout << "   Start Address: ";
                  std::cin >> startAddress;
                  std::cin.sync();
                  std::ifstream file;
                  file.open(path.c_str(), std::ifstream::in | std::ifstream::binary);
                  if (file) {
                        file.seekg(0, file.end);
                        int fileSize= file.tellg();
                        char* bytes= new char[fileSize];
                        file.seekg(0, file.beg);
                        file.read(bytes, fileSize);
                        file.close();
                        for (int i= 0; i < fileSize; i++) {
                              chip->writeChip(bytes[i], startAddress + i);
                              chip->wait();
                        }
                        delete bytes;
                  } else {
                        std::cout << "Failed to open file: invalid path" << std::endl;
                  }
            } else if (command == "scan") {
                  int startAddress= -1, stopAddress= -1;
                  std::cout << "   Start Address: ";
                  std::cin >> startAddress;
                  std::cin.sync();
                  std::cout << "   Stop Address: ";
                  std::cin >> stopAddress;
                  std::cin.sync();
                  for (int i= startAddress; i < stopAddress; i++) {
                        std::cout << chip->readChip(i) << ", ";
                  }
                  std::cout << std::endl;
            } else {
                  if (command != "exit") {
                        std::cout << "Error: invalid command" << std::endl;
                  }
            }
      } while (command != "exit");
      
      delete chip;
      
      return 0;
}


std::string getFilePath() {
      OPENFILENAME ofn;
      std::string filterSpec= "BIN Files (*.out)\0*.out\0";
      std::string title= "Open....";
      char szFileName[MAX_PATH];
      char szFileTitle[MAX_PATH];
      
      *szFileName = 0; *szFileTitle = 0;
      
      /* fill in non-variant fields of OPENFILENAME struct. */
      ofn.lStructSize= sizeof(OPENFILENAME);
      ofn.hwndOwner= GetFocus();
      ofn.lpstrFilter= filterSpec.c_str();
      ofn.lpstrCustomFilter= NULL;
      ofn.nMaxCustFilter= 0;
      ofn.nFilterIndex= 0;
      ofn.lpstrFile= szFileName;
      ofn.nMaxFile= MAX_PATH;
      ofn.lpstrInitialDir= ".";                                         // Initial directory.
      ofn.lpstrFileTitle= szFileTitle;
      ofn.nMaxFileTitle= MAX_PATH;
      ofn.lpstrTitle= title.c_str();
      ofn.lpstrDefExt= 0;                                               //I've set to null for demonstration
      
      ofn.Flags= OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
      
      if (!GetOpenFileName((LPOPENFILENAME)&ofn)) {
            return "NULL"; // Failed or cancelled
      } else {
            return szFileName;
      }
}


