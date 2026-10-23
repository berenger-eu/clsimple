#include <clsimple.hpp>

#include <string>
#include <vector>
#include <iostream>

static int failures = 0;

#define check(X) \
    if(!(X)){ \
        ++failures; \
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
            check(!isOk);
            check(aVal == false);
        }
        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"a"}, true, &isOk);
            check(!isOk);
            check(aVal == true);
        }
        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"b"}, false, &isOk);
            check(!isOk);
            check(aVal == false);
        }
        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"b"}, true, &isOk);
            check(!isOk);
            check(aVal == true);
        }
    }
    {
        const char *const argv[] = {"exec", "--a=true", "--b=1", "--x=false", "--y=0"};
        const int argc = 5;
        CLsimple args("UTest", argc, argv);

        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"a"}, false, &isOk);
            check(isOk);
            check(aVal == true);
        }
        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"a"}, true, &isOk);
            check(isOk);
            check(aVal == true);
        }
        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"b"}, false, &isOk);
            check(isOk);
            check(aVal == true);
        }
        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"b"}, true, &isOk);
            check(isOk);
            check(aVal == true);
        }
        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"x"}, false, &isOk);
            check(isOk);
            check(aVal == false);
        }
        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"x"}, true, &isOk);
            check(isOk);
            check(aVal == false);
        }
        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"y"}, false, &isOk);
            check(isOk);
            check(aVal == false);
        }
        {
            bool isOk;
            const bool aVal = args.getValue<bool>({"y"}, true, &isOk);
            check(isOk);
            check(aVal == false);
        }
    }
    {
        const char *const argv[] = {"exec", "--v", "a", "b",  "c", "d"};
        const int argc = 6;
        CLsimple args("UTest", argc, argv);

        {
            bool isOk;
            const auto values = args.getValues<std::string>({"v"}, {"y", "z"}, &isOk);
            check(isOk);
            check(values.size() == 4);
            check(values[0] == "a" && values[1] == "b"
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
            check(isOk);
            check(key == "b");

            const auto value = args.GetMapping<int>(key, {{"y", 0}, {"z", 1}, {"b", -1}}, -2);
            check(isOk);
            check(value == -1);
        }
    }
    {
        const char *const argv[] = {"exec", "--matrix", "foo"};
        const int argc = 3;
        CLsimple args("Cuspmv", argc, argv);

        args.addParameterNoArg({"help"}, "help");

        std::string matrixFile;
        args.addParameter<std::string>({"matrix"}, "matrix file", matrixFile, "");

        args.parse();
        check(args.isValid());
        check(!args.hasKey("help"));
        check(matrixFile == "foo");
    }


    // Missing options always initialize the status flag.
    {
        const char* argv[] = {"exec"};
        CLsimple args("UTest", 1, argv);
        bool ok = true;
        check(args.getValue<int>({"missing"}, 42, &ok) == 42);
        check(!ok);
        ok = true;
        check(args.getValues<int>({"missing"}, {42}, &ok) == std::vector<int>{42});
        check(!ok);
    }
    // Conversion must succeed and consume the entire argument.
    for(const char* input : {"", "abc", "12x", "9999999999999999999999999999"}){
        const char* argv[] = {"exec", "--n", input};
        CLsimple args("UTest", 3, argv);
        int n = 0;
        args.addParameter<int>({"n"}, "number", n, 7);
        check(!args.parse());
        check(n == 7);
    }
    {
        const char* argv[] = {"exec", "--text=hello world", "--empty="};
        CLsimple args("UTest", 3, argv);
        bool ok = false;
        check(args.getValue<std::string>({"text"}, "", &ok) == "hello world");
        check(ok);
        check(args.getValue<std::string>({"empty"}, "fallback", &ok).empty());
        check(ok);
    }
    {
        const char* argv[] = {"exec", "--flag=maybe"};
        CLsimple args("UTest", 2, argv);
        bool value = false;
        args.addParameter<bool>({"flag"}, "boolean", value, true);
        check(!args.parse());
        check(value);
    }
    {
        const char* argv[] = {"exec", "--text", "--help"};
        CLsimple args("UTest", 3, argv);
        std::string text;
        args.addParameter<std::string>({"text"}, "text", text, "fallback");
        args.addParameterNoArg({"help"}, "help");
        check(!args.parse());
        check(text == "fallback");
    }
    {
        const char* argv[] = {"exec", "--values", "-.5", "-1.25", "-2e-3"};
        CLsimple args("UTest", 5, argv);
        std::vector<double> values{99};
        args.addMultiParameter<double>({"values"}, "numbers", values, {42});
        check(args.parse());
        check((values == std::vector<double>{-.5, -1.25, -.002}));
        check(args.parse());
        check(values.size() == 3);
    }
    {
        const char* argv[] = {"exec", "--values", "1", "bad", "3"};
        CLsimple args("UTest", 5, argv);
        std::vector<int> values;
        args.addMultiParameter<int>({"values"}, "numbers", values, {42});
        check(!args.parse());
        check(values == std::vector<int>{42});
        std::ostringstream first, second;
        args.printError(first);
        check(!args.parse());
        args.printError(second);
        check(first.str() == second.str());
    }
    // Registered parameters can validate values without a bound variable.
    {
        const char* argv[] = {"exec", "--n=12", "--values", "true", "0"};
        CLsimple args("UTest", 5, argv);
        args.addParameter<int>({"n"}, "number");
        args.addMultiParameter<bool>({"values"}, "booleans");
        check(args.parse());
    }
    {
        const char* argv[] = {"exec", "--n=bad", "--values", "bad"};
        CLsimple args("UTest", 4, argv);
        args.addParameter<int>({"n"}, "number");
        args.addMultiParameter<int>({"values"}, "numbers");
        check(!args.parse());
    }
    {
        const char* argv[] = {"exec", "--backup=true"};
        CLsimple args("UTest", 2, argv);
        args.addParameter<bool>({"backup"}, "backup", std::nullopt, false, 1);
        args.addParameter<bool>({"restore"}, "restore", std::nullopt, false, 2);
        check(args.parse());
    }
    {
        const char* argv[] = {"exec", "--help=yes"};
        CLsimple args("UTest", 2, argv);
        args.addParameterNoArg({"help"}, "help");
        check(!args.parse());
    }


    {
        const char* argv[] = {"exec", "--text=line one\nline two=three"};
        CLsimple args("UTest", 2, argv);
        bool ok = false;
        check(args.getValue<std::string>({"text"}, "", &ok) == "line one\nline two=three");
        check(ok);
    }
    {
        const char* argv[] = {"exec", "--n=-1"};
        CLsimple args("UTest", 2, argv);
        bool ok = true;
        check(args.getValue<unsigned>({"n"}, 42, &ok) == 42);
        check(!ok);
    }
    for(const char* input : {"1e9999", "1.5x", ""}){
        const char* argv[] = {"exec", "--n", input};
        CLsimple args("UTest", 3, argv);
        bool ok = true;
        check(args.getValue<double>({"n"}, 42, &ok) == 42);
        check(!ok);
    }
    {
        const char* argv[] = {"exec", "--values=1", "2", "3"};
        CLsimple args("UTest", 4, argv);
        bool ok = false;
        check((args.getValues<int>({"values"}, {42}, &ok) == std::vector<int>{1, 2, 3}));
        check(ok);
    }
    {
        const char* argv[] = {"exec", "--values", "--help"};
        CLsimple args("UTest", 3, argv);
        std::vector<int> values;
        args.addMultiParameter<int>({"values"}, "numbers", values, {42});
        args.addParameterNoArg({"help"}, "help");
        check(!args.parse());
        check(values == std::vector<int>{42});
    }
    // Every member of one mandatory group is required.
    for(int argc : {2, 3}){
        const char* argv[] = {"exec", "--a", "--b"};
        CLsimple args("UTest", argc, argv);
        args.addParameterNoArg({"a"}, "first", 1);
        args.addParameterNoArg({"b"}, "second", 1);
        args.addParameterNoArg({"c"}, "alternative", 2);
        check(args.parse() == (argc == 3));
    }
    for(int mode : {0, 1, 2}){
        const char* argv[] = {"exec", "--unknown"};
        CLsimple args("UTest", 2, argv, mode != 1, mode == 2);
        check(args.parse() == (mode != 0));
    }
    // Ignoring unknown arguments must not hide malformed registered values.
    {
        const char* argv[] = {"exec", "--n=bad", "--unknown"};
        CLsimple args("UTest", 3, argv, false, true);
        args.addParameter<int>({"n"}, "number");
        check(!args.parse());
    }
    // A subsequent parse must be able to recover from an earlier failure.
    {
        const char* argv[] = {"exec", "--help"};
        CLsimple args("UTest", 2, argv);
        check(!args.parse());
        args.addParameterNoArg({"help"}, "help");
        check(args.parse());
        std::ostringstream errors;
        args.printError(errors);
        check(errors.str().empty());
    }

    {
        const char* argv[] = {"exec", "--letter=x"};
        CLsimple args("UTest", 2, argv);
        bool ok = false;
        check(args.getValue<char>({"letter"}, 'z', &ok) == 'x');
        check(ok);
    }
    {
        const char* argv[] = {"exec", "--help"};
        CLsimple original("Original", 2, argv);
        original.addParameterNoArg({"help"}, "help");
        CLsimple copy("Copy", 0, nullptr);
        copy = original;
        check(copy.parse());
        check(copy.hasKey("help"));
        std::ostringstream help;
        copy.printHelp(help);
        check(help.str().find("Original") != std::string::npos);
    }

    return failures == 0 ? 0 : 1;
}
