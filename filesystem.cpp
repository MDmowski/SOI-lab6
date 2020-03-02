#include "filesystem.hpp"
#include <cstdlib>
#include <string>
#include <unistd.h>

#define BLOCK_SIZE 4
using namespace std;
VirtualDisk::VirtualDisk(int sizeInKilobytes)
    : diskSizeInKilobytes(sizeInKilobytes), freeSpace(sizeInKilobytes * 1024),
      fileCount(0)
{
    fileList = (FileDescriptor *)malloc(sizeof(FileDescriptor));
    if(access("virtualDisk", F_OK) != -1)
    {
        std::cout << "Virtual disk exists, size ignored" << std::endl;
        mainFile.open("virtualDisk", ios::out | ios::in | ios::binary);
        readFAT();
        readInfoBlock();
    }
    else
    {
        int sizeInBytes = 1024 * sizeInKilobytes;
        fatSize = calculateAvailableBlocks(sizeInBytes);
        createEmptyFile();
        mainFile.open("virtualDisk", ios::out | ios::in | ios::binary);
        initialiseFAT();
        writeInfoBlock();
        writeFAT();
    }
}

VirtualDisk::~VirtualDisk() { mainFile.close(); }

void VirtualDisk::createEmptyFile()
{
    std::cout << "Creating a virtual disk" << std::endl;
    ofstream tempFileHandle;
    tempFileHandle.open("virtualDisk", ios::out | ios::binary);
    tempFileHandle.close();
}

void VirtualDisk::writeInfoBlock()
{
    InfoBlock info;
    info.diskSize = diskSizeInKilobytes;
    info.freeSpace = freeSpace - dataOffset;
    info.fileCount = fileCount;
    info.dataOffset = dataOffset;
    mainFile.seekp(0); // Change current write posision
    mainFile.write((char *)(&info), sizeof(InfoBlock));
}
void VirtualDisk::readInfoBlock()
{
    std::cout << "Reading info block" << std::endl;
    InfoBlock info;
    mainFile.seekg(0);
    mainFile.read((char *)(&info), sizeof(InfoBlock));
    diskSizeInKilobytes = info.diskSize;
    fileCount = info.fileCount;
    dataOffset = info.dataOffset;
    fatSize = (dataOffset - sizeof(InfoBlock)) / sizeof(FATcell);
    std::cout << "FAT size " << fatSize << std::endl;
    std::cout << "Size in kB: " << diskSizeInKilobytes
              << " File counter: " << fileCount
              << " Data offset: " << dataOffset
              << " Free space: " << info.freeSpace << std::endl;
}

void VirtualDisk::initialiseFAT()
{
    fileAllocationTable = new FATcell[fatSize];
    fatTableOffset = sizeof(InfoBlock);
    dataOffset = fatTableOffset + fatSize * sizeof(FATcell);
    for(int i = 0; i < fatSize; i++)
    {
        fileAllocationTable[i].nextItem = 0;
    }
    fileAllocationTable[0].nextItem = -2; // reserved for descriptors
}

void VirtualDisk::writeFAT()
{
    mainFile.seekp(fatTableOffset);
    for(int i = 0; i < fatSize; i++)
    {
        mainFile.write((char *)(&fileAllocationTable[i]), sizeof(FATcell));
    }
}
void VirtualDisk::readFAT()
{
    std::cout << "Reading fat table" << std::endl;
    initialiseFAT();
    mainFile.seekg(fatTableOffset);
    for(int i = 0; i < fatSize; i++)
    {
        mainFile.read((char *)(&fileAllocationTable[i]), sizeof(FATcell));
    }
}
void VirtualDisk::printFAT()
{
    for(int i = 0; i < fatSize; i++)
    {
        std::cout << "TABLE[" << i << "]=" << fileAllocationTable[i].nextItem
                  << std::endl;
    }
}

int VirtualDisk::calculateAvailableBlocks(int sizeInBytes)
{
    int availableSpace = sizeInBytes - sizeof(InfoBlock);
    int pairSize = sizeof(FATcell) + BLOCK_SIZE;
    int blocksCount = availableSpace / pairSize;
    return blocksCount;
}

void VirtualDisk::showMap() { VirtualDisk::printFAT(); }

int VirtualDisk::findFreeBlock()
{
    int i = 0;
    while(fileAllocationTable[i].nextItem != -1 && i < fatSize)
    {
        i++;
    }
    return (i == fatSize) ? -1 : i;
}
