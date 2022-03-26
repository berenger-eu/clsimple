#include <clsimple.hpp>

#include <string>
#include <vector>
#include <iostream>

#define check(X) \
    if(!(X)){ \
        std::cout << "ERROR " << __FILE__ << " at line " << __LINE__ << std::endl;\
        std::cout << " => " << #X << std::endl;\
    }

int main(int argc, char** argv){
    {
        const char *const argv[] = {"exec", "-a", "-b", "c", "d"};
        const int argc = 3;
        CLsimple args("UTest", argc, argv);

        check(args.hasKey("a"));
        check(args.hasKey("b"));
        check(!args.hasKey("c"));
        check(!args.hasKey("d"));
    }

    return 0;
}
