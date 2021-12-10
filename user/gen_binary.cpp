#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

int main(int argc, const char **argv)
{
    // cout << argc << endl;
    // for (int i = 0; i < argc; ++i) {
    //     cout << argv[i] << endl;
    // }

    if (argc < 4) {
        return 0;
    }

    const char *src = argv[1];
    const char *dst = argv[2];
    char buffer[1024];
    char byte_buffer[512];

    ifstream in;
    ofstream out;

    in.open(src, ios::binary | ios::in);
    out.open(dst, ios::binary | ios::out);

    if (!(in.is_open() && out.is_open())) {
        return 0;
    }

    sprintf(buffer, "unsigned char %s [] = {\n", argv[3]);


    out.write(buffer, strlen(buffer));
    
    in.seekg(0, ios::end);
    streamsize size = in.tellg();
    in.seekg(0, ios::beg);


    while(size >= sizeof(buffer)) {
        in.read(buffer, sizeof(buffer));
        for(int i = 0; i < sizeof(buffer); ++i) {
            sprintf(byte_buffer, "0x%x, ", (unsigned char)buffer[i]);
            out.write(byte_buffer, strlen(byte_buffer));
        }
        size -= sizeof(buffer);
    }

    if(size) {
        in.read(buffer, size);
        for(int i = 0; i < size; ++i) {
            sprintf(byte_buffer, "0x%x, ", (unsigned char)buffer[i]);
            out.write(byte_buffer, strlen(byte_buffer));
        }        
    }

    
    sprintf(byte_buffer, "0};");
    out.write(byte_buffer, strlen(byte_buffer));

    in.close();
    out.close();
}