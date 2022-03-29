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
        const int argc = 5;
        CLsimple args("UTest", argc, argv);

        check(args.hasKey("a"));
        check(args.hasKey("b"));
        check(!args.hasKey("c"));
        check(!args.hasKey("d"));
    }
    {
        const char *const argv[] = {"exec", "---a", "--b", "x-c", "-f-d"};
        const int argc = 5;
        CLsimple args("UTest", argc, argv);

        check(!args.hasKey("a"));
        check(args.hasKey("b"));
        check(!args.hasKey("c"));
        check(!args.hasKey("d"));
    }
    {
        const char *const argv[] = {"exec", "---a=", "--b=", "x-c=", "-f-d="};
        const int argc = 5;
        CLsimple args("UTest", argc, argv);

        check(!args.hasKey("a"));
        check(args.hasKey("b"));
        check(!args.hasKey("c"));
        check(!args.hasKey("d"));
    }
    {
        const char *const argv[] = {"exec", "---a=", "--b=", "x-c=", "-f-d="};
        const int argc = 5;
        CLsimple args("UTest", argc, argv);

        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"a"}, false, &isOk);
            assert(isOk);
            assert(aVal == false);
        }
        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"a"}, true, &isOk);
            assert(isOk);
            assert(aVal == true);
        }
        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"b"}, false, &isOk);
            assert(!isOk);
            assert(aVal == false);
        }
        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"b"}, true, &isOk);
            assert(!isOk);
            assert(aVal == true);
        }
    }

    return 0;
}
