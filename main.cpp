#include "filesystem.hpp"
#include <cstdlib>
#include <iostream>

int main(int argc, char *argv[])
{
    std::cout << "Start" << std::endl;
    VirtualDisk *disk = new VirtualDisk(4);
    disk->showMap();
    delete disk;
}