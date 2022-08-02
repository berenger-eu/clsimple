clsimple is a lightweight command line manager for C++ (17) applications.
It is under MIT license.

# Prerequisite

None. `clsimple.hpp` (located in the src directory) can be included directly in your project.

# Examples

First, create a `CLsimple` object with the name of the application and the args.
```
CLsimple args("MyApp", argc, argv);
```

One can test that a `key` has been passed:
```
const bool hasA = args.hasKey("a"); // Will be true if "-a", "--a", "--a=", ... exist
const bool hasAorB = args.hasOneOfKeys({"a", "b"})); // We can test several values
```

One can ask for a value for a given key:
```
bool isOk;
const bool aVal = args.getValue<bool>({"a"}, false, &isOk); // "--a=true" or "--a=1" will allow return true
```

Or ask for a set of values:
```
bool isOk;
const auto values = args.getValues<std::string>({"param"}, {"default1", "defalt2"}, &isOk);
// If "-param x y" is passed, then values will contain "x" "y"
```

We have an helper function to map a list of keys to a list of values, and get the right choice:
```
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
