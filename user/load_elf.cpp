#include <elf.h>
#include <iostream>
using namespace std;

extern unsigned char zero[];

int main() {
    Elf64_Ehdr *ehdr_ptr = (Elf64_Ehdr *)zero;
    cout << hex << ehdr_ptr->e_entry << endl;
    cout << dec << ehdr_ptr->e_phoff << endl;
    cout << dec << ehdr_ptr->e_phnum << endl;

    Elf64_Phdr *phdr_ptr = nullptr;

    for (int i = 0; i < ehdr_ptr->e_phnum; ++i ) {
        phdr_ptr = (Elf64_Phdr *)(zero + ehdr_ptr->e_phoff + i * ehdr_ptr->e_phentsize);
        cout << hex << phdr_ptr->p_vaddr << " " << (phdr_ptr->p_type == PT_LOAD) << endl;
    }
}