#include <clsimple.hpp>

#include <string>
#include <vector>
#include <iostream>

int omp_get_max_threads(){ return 10;}// TODO

int main(int argc, char** argv){
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

    return 0;
}
