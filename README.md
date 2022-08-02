clsimple is a lightweight command line manager for C++ (17) applications.
It is under MIT license.

# Prerequisite

None. `clsimple.hpp` (located in the src directory) can be included directly in your project.

# Design

A parameter can be of type "no-arg", "single" or "multi", if no, 1 or several values can be passed with it.
A parameter can also be mandatory, and one can create "group" of parameters by assigning the same group-id.

# Examples

## Use it as a command line manager

```cpp
CLsimple args("MyApp", argc, argv
              // optional: const bool inFailsIfInvalid = true,
              // optional: const bool inAcceptUnregisteredParams = false
              );

// These two parameters without args are added only to be print
// in the help and check the coherency, but no values will be filled
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
// Like "-backup=true", with default value true
// Will be mandatory in group 1

bool restore;
args.addParameter<bool>({"restore"}, "restore the files", restore, false, 2);
// Like "-restore=true", with default value false
// Will be mandatory in group 2

// So either backup or restore must be set

// We parse the parameters
args.parse();

// Check if parse is invalid or if "-help" has been passed
if(!args.isValid() || args.hasKey("help")){
    // Print the help
    args.printHelp(std::cout);
    return -1;
}
```

This will give:
```
$ ./example.exe 
[HELP] 
[HELP] MyApp
[HELP] 
[HELP] 
[HELP] Currant parameters are not valid...
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
[ERROR] Some arguments are mandatory, but none of them were provided...
```

## Use it as a parser

First, create a `CLsimple` object with the name of the application and the args.
```cpp
CLsimple args("MyApp", argc, argv);
```

One can test that a `key` has been passed:
```cpp
const bool hasA = args.hasKey("a"); // Will be true if "-a", "--a", "--a=", ... exist
const bool hasAorB = args.hasOneOfKeys({"a", "b"})); // We can test several values
```

One can ask for a value for a given key:
```cpp
bool isOk;
const bool aVal = args.getValue<bool>({"a"}, false, &isOk); // "--a=true" or "--a=1" will allow return true
```

Or ask for a set of values:
```cpp
bool isOk;
const auto values = args.getValues<std::string>({"param"}, {"default1", "defalt2"}, &isOk);
// If "-param x y" is passed, then values will contain "x" "y"
```

We have an helper function to map a list of keys to a list of values, and get the right choice:
```cpp
bool isOk;
const auto key = args.getValue<std::string>({"v"}, {"z"}, &isOk);
// Will return "b" if there is "-v b"

const auto value = args.GetMapping<int>(key, {{"y", 0}, {"z", 1}, {"b", -1}}, -2);
// Will return -1 if key == "b", -2 otherwise
```


# Support

To get some help, leave in issue on the official repository or on the github mirror:
- https://gitlab.inria.fr/bramas/clsimple
- https://github.com/berenger-eu/clsimple
