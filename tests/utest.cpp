#include <clsimple.hpp>

#include <string>
#include <vector>
#include <iostream>

#define check(X) \
    if(!(X)){ \
        std::cout << "ERROR " << __FILE__ << " at line " << __LINE__ << std::endl;\
        std::cout << " => " << #X << std::endl;\
    }

int main(){
    {
        const char *const argv[] = {"exec", "-a", "-b", "c", "d"};
        const int argc = 5;
        CLsimple args("UTest", argc, argv);

        check(args.hasKey("a"));
        check(args.hasKey("b"));
        check(!args.hasKey("c"));
        check(!args.hasKey("d"));
        check(args.hasOneOfKeys({"a"}));
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
            assert(!isOk);
            assert(aVal == false);
        }
        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"a"}, true, &isOk);
            assert(!isOk);
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
    {
        const char *const argv[] = {"exec", "--a=true", "--b=1", "--x=false", "--y=0"};
        const int argc = 5;
        CLsimple args("UTest", argc, argv);

        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"a"}, false, &isOk);
            assert(isOk);
            assert(aVal == true);
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
            assert(isOk);
            assert(aVal == true);
        }
        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"b"}, true, &isOk);
            assert(isOk);
            assert(aVal == true);
        }
        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"x"}, false, &isOk);
            assert(isOk);
            assert(aVal == false);
        }
        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"x"}, true, &isOk);
            assert(isOk);
            assert(aVal == false);
        }
        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"y"}, false, &isOk);
            assert(isOk);
            assert(aVal == false);
        }
        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"y"}, true, &isOk);
            assert(isOk);
            assert(aVal == false);
        }
    }
    {
        const char *const argv[] = {"exec", "--v", "a", "b",  "c", "d"};
        const int argc = 6;
        CLsimple args("UTest", argc, argv);

        {
            bool isOk;
            const auto values = args.getValues<std::string>({"v"}, {"y", "z"}, &isOk);
            assert(isOk);
            assert(values.size() == 4);
            assert(values[0] == "a" && values[1] == "b"
                    && values[2] == "c" && values[3] == "d");
        }
    }
    {
        const char *const argv[] = {"exec", "--v", "b"};
        const int argc = 3;
        CLsimple args("UTest", argc, argv);

        {
            bool isOk;
            const auto key = args.getValue<std::string>({"v"}, {"z"}, &isOk); // Will return b
            assert(isOk);
            assert(key == "b");

            const auto value = args.GetMapping<int>(key, {{"y", 0}, {"z", 1}, {"b", -1}}, -2);
            assert(isOk);
            assert(value == -1);
        }
    }

    return 0;
}
