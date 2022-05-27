//
// Created by lu'ren'jia'd's on 2022/5/2.
//

#ifndef UNTITLED2_MEMORYRIVER_H
#define UNTITLED2_MEMORYRIVER_H

#include <fstream>
#include <cstring>
#include "FileException.h"
namespace ds {
    template<typename T>
    class MemoryRiver {
    private:
        char *file_name;
    public:
        MemoryRiver(char *fn) {
            std::fstream file;
            file_name = new char[strlen(fn) + 1];
            strcpy(file_name, fn);
            file.clear();
            file.open(file_name, std::ios::in);
            if (!file) {
                file.clear();
                file.open(file_name, std::ios::out | std::ios::app);
            }
            if (!file.is_open()){
                ds::OpenException e;
                throw e;
            }
            file.close();
        }

        ~MemoryRiver() {
            delete[]file_name;
        }

        bool Write(const int& index, const T &t) {
            std::fstream file;
            file.clear();
//            if(file.fail())printf("%s","clear error\n");
            file.open(file_name);
//            if(file.fail())printf("%s","open error\n");
            if (!file.is_open()){
                ds::OpenException e;
                throw e;
            }
            file.seekp(index,std::ios::beg);
//            if(file.fail())printf("%s","seek error\n");
            file.write(reinterpret_cast<const char *>(&t), sizeof(T));
            if (file.fail()) {
//                printf("%s", "mr write failed\n");
                return false;
            }
            file.close();
            return true;
        }

        bool Read(const int& index, T &res) {
            std::fstream file;
            file.clear();
            file.open(file_name);
            if (!file.is_open()){
                ds::OpenException e;
                throw e;
            }
            file.seekg(index);
            file.read(reinterpret_cast<char *>(&res), sizeof(T));
            if (file.fail()) {
//                printf("%s", "mr read failed");
                return false;
            }
            file.close();
            return true;
        }

        int Append(const T &t) {
            std::fstream file;
            //write a T at the end of the file and return the location writing in
            file.clear();
            file.open(file_name);
            if (!file.is_open()){
                ds::OpenException e;
                throw e;
            }
            file.seekp(0, std::ios::end);
            int location = file.tellp();
            file.write(reinterpret_cast<const char *>(&t), sizeof(T));
            if (file.fail()){
                ds::AppendException e;
                throw e;
            }
            file.close();
            return location;
        }
        int FindEnd(){
            std::fstream file;
            file.clear();
            file.open(file_name);
            if (!file.is_open()){
                ds::OpenException e;
                throw e;
            }
            file.seekp(0, std::ios::end);
            int location = file.tellp();
            file.close();
            return location;
        }
    };
}
#endif //UNTITLED2_MEMORYRIVER_H
