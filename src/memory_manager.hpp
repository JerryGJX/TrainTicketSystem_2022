#ifndef MEMORY_MANAGER_HPP
#define MEMORY_MANAGER_HPP

#include <fstream>
#include <iostream>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

template<class T, int info_len = 2>
class MemoryRiver {
private:
   fstream file;
   string file_name;
   string recycle_file_name;
   int sizeofT = sizeof(T);
   int recycle_num;
public:
   MemoryRiver() = default;

   MemoryRiver(const string& file_name) : file_name(file_name) {
      initialise();
   }

   void initialise(string FN = "") {
      if (FN != "") {
         file_name = FN;
         recycle_file_name = file_name + "_recycle";
      }
      file.open(file_name);
      if (!file) {
         file.close();
         file.open(file_name, std::ios::out);
         int tmp = 0;
         for (int i = 0; i < info_len; ++i) {
            file.seekp(i * sizeof(int));
            file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
         }
      }
      file.close();
      file.open(recycle_file_name);
      if (!file) {
         file.close();
         file.open(recycle_file_name, std::ios::out);
         recycle_num = 0;
         file.seekp(0);
         file.write(reinterpret_cast<char *> (&recycle_num), sizeof(int));
      }
      else {
         file.seekg(0);
         file.read(reinterpret_cast<char *> (&recycle_num), sizeof(int));
      }
      file.close();
   }

   //读出第n个int的值赋给tmp，1_base
   void get_info(int &tmp, int n) {
      if (n > info_len) return;
      file.open(file_name);
      file.seekg((n - 1) * sizeof(int));
      file.read(reinterpret_cast<char *> (&tmp), sizeof(int));
      file.close();
   }

   //将tmp写入第n个int的位置，1_base
   void write_info(int tmp, int n) {
      if (n > info_len) return;
      file.open(file_name);
      file.seekp((n - 1) * sizeof(int));
      file.write(reinterpret_cast<char *> (&tmp), sizeof(int));
      file.close();
   }

   //在文件合适位置写入类对象t，并返回写入的位置索引index
   //位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
   //位置索引index可以取为对象写入的起始位置
   int write(T &t) {
      if (!recycle_num) {
         file.open(file_name);
         file.seekp(0, std::ios::end);
         int index = (file.tellp() - info_len * sizeof(int)) / sizeofT;
         //std::cout << file.tellp() << std::endl;
         //std::cout << file.tellp() - info_len * sizeof(int) << ' ' << std::endl;
         file.write(reinterpret_cast<char *> (&t), sizeofT);
         file.close();
         return index;
      }
      else {
         int index;
         file.open(recycle_file_name);
         file.seekg(recycle_num-- * sizeof(int));
         file.read(reinterpret_cast<char *> (&index), sizeof(int));
         file.seekp(0);
         file.write(reinterpret_cast<char *> (&recycle_num), sizeof(int));
         file.close();
         file.open(file_name);
         file.seekp(info_len * sizeof(int) + index * sizeofT);
         file.write(reinterpret_cast<char *> (&t), sizeofT);
         file.close();
         return index;
      }
   }

   //用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生
   void update(T &t, const int index) {
      file.open(file_name);
      file.seekp(info_len * sizeof(int) + index * sizeofT);
      file.write(reinterpret_cast<char *> (&t), sizeofT);
      file.close();
   }

   //读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
   void read(T &t, const int index) {
      file.open(file_name);
      file.seekg(info_len * sizeof(int) + index * sizeofT);
      file.read(reinterpret_cast<char *> (&t), sizeofT);
      file.close();
   }

   //删除位置索引index对应的对象(不涉及空间回收时，可忽略此函数)，保证调用的index都是由write函数产生
   void Delete(int index) {
      file.open(recycle_file_name);
      file.seekp(++recycle_num * sizeof(int));
      file.write(reinterpret_cast<char *> (&index), sizeof(int));
      file.seekp(0);
      file.write(reinterpret_cast<char *> (&recycle_num), sizeof(int));
      file.close();
   }

   void clear() {
      recycle_num = 0;
      file.open(file_name, std::ios::out | std::ios::trunc);
      file.close();
      file.open(recycle_file_name, std::ios::out | std::ios::trunc);
      file.close();
   }
};


#endif //MEMORY_MANAGER_HPP
 