clsimple is a lightweight command line manager for C++17 applications.
It is distributed under the MIT license.

# Requirements

A C++17 compiler is required. The library is header-only and has no external dependencies: include `src/clsimple.hpp` directly in your project.

To build the examples and run the tests with CMake 3.10 or newer:

```sh
mkdir -p build
cd build
cmake .. -DUSE_TESTING=ON
cmake --build .
ctest --output-on-failure
```

# Design

A parameter accepts no value (`addParameterNoArg`), one value (`addParameter`), or one or more values (`addMultiParameter`). Register names without leading dashes; several names in the same list are aliases.

Assign a group ID to make a parameter mandatory. Every parameter in a group must be present and valid, and at least one mandatory group must be complete. Parameters with the default group ID, `CLsimple::NotMandatory`, are optional. Malformed values in any supplied parameter make parsing fail.

Supported argument forms are `-name value`, `--name value`, and `--name=value`. Inline values require two leading dashes. Lists may use `--name=first second third`. Negative integers, decimals, and scientific notation are accepted as values. A value that resembles an option, such as `-filename`, can be supplied to a single parameter as `--name=-filename`. There is no positional-argument or `--` end-of-options mode.

String values retain spaces and may be empty (`--name=""`). Boolean values accept `true`, `True`, `TRUE`, `false`, `False`, `FALSE`, or an integer (zero is false). Other Boolean text, numeric overflow, and negative numbers for unsigned types are rejected. No-value flags reject `--flag=value`.

Missing parameters use their defaults. Invalid single values or lists also restore the complete default, and `parse()` returns false. Parsing again resets bound values and diagnostics. Bound variables must outlive the parser and its copies; copies share parameter registrations and diagnostics.

By default, unregistered or unused arguments cause parsing to fail. Either `inAcceptUnregisteredParams=true` or `inFailsIfInvalid=false` disables that check; malformed registered parameters and incomplete mandatory groups still fail. Register each parameter once: repeated occurrences are not accumulated, and unused duplicates are rejected by default. When several aliases are present, the first registered alias found is used.

# Examples

## Use it as a command line manager

```cpp
#include "clsimple.hpp"
#include <iostream>

int main(int argc, char** argv){
    CLsimple args("MyApp", argc, argv
                  // optional: const bool inFailsIfInvalid = true,
                  // optional: const bool inAcceptUnregisteredParams = false
                  );

    // No-value flags appear in help and are checked by parse().
    args.addParameterNoArg({"help"}, "help"); // for "-help"
    args.addParameterNoArg({"version"}, "version"); // for "-version"

    std::vector<std::string> multidir;
    args.addMultiParameter<std::string>({"multidir"}, "multiple simulation directories", multidir);
    // Like "-multidir dir1 dir2", with empty default

    std::string config;
    args.addParameter<std::string>({"config"}, "config file", config, "");
    // Like "-config file", with empty default

    bool backup;
    args.addParameter<bool>({"backup"}, "backup of files", backup, true, 1);
    // Like "--backup=true", with default value true
    // Will be mandatory in group 1

    bool restore;
    args.addParameter<bool>({"restore"}, "restore the files", restore, false, 2);
    // Like "--restore=true", with default value false
    // Will be mandatory in group 2

    // So either backup or restore must be set

    // Help can be requested without satisfying mandatory groups.
    if(args.hasKey("help")){
        args.printHelp(std::cout);
        return 0;
    }

    // Parse and validate the parameters.
    args.parse();

    // Report invalid arguments.
    if(!args.isValid()){
        // Print the help
        args.printHelp(std::cout);
        return 1;
    }
    return 0;
}
```

This will give:
```
$ ./example.exe 
[HELP] 
[HELP] MyApp
[HELP] 
[HELP] 
[HELP] Current parameters are not valid...
[HELP] 
[HELP] Parameter names: {help}
[HELP]  - Description: help
[HELP]  - Type: (No argument)
[HELP]
[HELP] Parameter names: {version}
[HELP]  - Description: version
[HELP]  - Type: (No argument)
[HELP]
[HELP] Parameter names: {multidir}
[HELP]  - Description: multiple simulation directories
[HELP]  - Type: List of Strings
[HELP]
[HELP] Parameter names: {config}
[HELP]  - Description: config file
[HELP]  - Type: String
[HELP]
[HELP] Parameter names: {backup}
[HELP]  - Description: backup of files
[HELP]  - Type: Boolean
[HELP]  - Is mandatory (group 1)
[HELP]
[HELP] Parameter names: {restore}
[HELP]  - Description: restore the files
[HELP]  - Type: Boolean
[HELP]  - Is mandatory (group 2)
[HELP]
[HELP]
[HELP] Command line with all args: ./example.exe --help --version --multidir [List of Strings] --config=[String] --backup=[Boolean] --restore=[Boolean] 
[HELP]
[ERROR] There are some errors...
[ERROR] No mandatory parameter group was fully provided with valid values.
```

## Use it as a parser

First, create a `CLsimple` object with the name of the application and the args.
```cpp
CLsimple args("MyApp", argc, argv);
```

One can test that a `key` has been passed:
```cpp
const bool hasA = args.hasKey("a"); // Will be true if "-a", "--a", "--a=", ... exist
const bool hasAorB = args.hasOneOfKeys({"a", "b"}); // We can test several values
```

One can ask for a value for a given key. The optional status flag is always set: it is true only when the key is present and its value is valid. These accessors do not change `isValid()`; use `parse()` to validate registered parameters:
```cpp
bool isOk;
const bool aVal = args.getValue<bool>({"a"}, false, &isOk); // "--a=true" or "--a=1" will allow return true
```

Or ask for a set of values:
```cpp
bool isOk;
const auto values = args.getValues<std::string>({"param"}, {"default1", "default2"}, &isOk);
// If "-param x y" is passed, then values will contain "x" "y"
```

Use the helper function to map a list of keys to a list of values, to select a value:
```cpp
bool isOk;
const auto key = args.getValue<std::string>({"v"}, {"z"}, &isOk);
// Will return "b" if there is "-v b"

const auto value = args.GetMapping<int>(key, {{"y", 0}, {"z", 1}, {"b", -1}}, -2);
// Returns -1 for "b", 0 for "y", 1 for "z", or the fallback -2 for any other key.
```


# Support

To get some help, open an issue on the official repository or on the GitHub mirror:

- [Official repository](https://gitlab.inria.fr/bramas/clsimple)
- [GitHub mirror](https://github.com/berenger-eu/clsimple)
