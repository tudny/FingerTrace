#include <bits/stdc++.h>

using namespace std;

int main() {
    string path = "path\\to\\your\\folder\\values.txt";
    string pathXML = "vpath\\to\\your\\folder\\aluesXML.txt";
    fstream in, out;
    in.open(path);
    out.open(pathXML);
    for(int y = 0; y < 10; y++){
        for(int x = 0; x < (28 * 28); x++){
            string value;
            in >> value;
            out << ("<item>a" + value + "</item>\n");
        }
    }
    in.close();
    out.close();
    return 0;
}
