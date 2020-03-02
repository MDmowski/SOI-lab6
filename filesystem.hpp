#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;
class VirtualDisk
{
  public:
    VirtualDisk(int sizeInKilobytes);
    ~VirtualDisk();
    void copyFromMinixToVirtual(char *);
    void listFiles();
    void removeFile(char *);
    void copyFromVirtualToMinix(char *);
    void showMap();

  private:
    typedef struct InfoBlock
    {
        int diskSize;
        int freeSpace;
        int fileCount;
        int dataOffset;
    } InfoBlock;
    typedef struct FATcell
    {
        int nextItem;
    } FATcell;

    typedef struct FileDescriptor
    {
        char *filename;
        int size;
        int firstBlock;
    } FileDescriptor;
    void createEmptyFile();
    void writeInfoBlock();
    void readInfoBlock();
    void writeFAT();
    void readFAT();
    void initialiseFAT();
    void printFAT();
    int calculateAvailableBlocks(int);
    int findFreeBlock();

    fstream mainFile;
    int diskSizeInKilobytes;
    int freeSpace;
    int fileCount;
    int dataOffset;
    int fatTableOffset;
    int firstBlockOffset;
    int fatSize;
    FATcell *fileAllocationTable;
    FileDescriptor *fileList;
};