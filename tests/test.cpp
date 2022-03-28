#include <clsimple.hpp>

#include <string>
#include <vector>
#include <iostream>

int omp_get_max_threads(){ return 10;}// TODO

int main(int argc, char** argv){
    CLsimple args("Complexespp", argc, argv);

    bool isOk;
    const bool directBackup = args.getValue<bool>({"backup"}, false, &isOk);
    std::cout << "directBackup: " << directBackup << std::endl;

    const std::vector<std::string> directMultiDir = args.getValues<std::string>({"multidir"}, {}, &isOk);
    std::cout << "values directMultiDir (" << directMultiDir.size() << ") ";
    for(auto& dr : directMultiDir){
        std::cout << " " << dr;
    }
    std::cout << std::endl;

    ////////////////////////////////////////////////////

    args.addParameterNoArg({"help"}, "help");
    args.addParameterNoArg({"version"}, "version");

    std::vector<std::string> multidir;
    args.addMultiParameter<std::string>({"multidir"}, "multiple simulation directories",
                                    multidir);

    std::string config;
    args.addParameter<std::string>({"config"}, "config file",
                                   config, "");

    bool backup;
    args.addParameter<bool>({"backup"}, "backup of files",
                            backup, true);

    bool rerun;
    args.addParameter<bool>({"rerun"}, "recalculate energies from trajectory",
                            rerun, true);


    std::string restart;
    args.addParameter<std::string>({"restart"}, "restart file",
                                   restart, "");

    bool version;
    args.addParameter<bool>({"version"}, "show version",
                            version, false);

    int replex;
    args.addParameter<int>({"replex"}, "number of sweeps between exchanges",
                           replex, 0);

    int replex_stat;
    args.addParameter<int>({"replex-stat"}, "number of sweeps between statistic output",
                           replex_stat, 1000);

    std::string replex_accept;
    args.addParameter<std::string>({"replex-accept"}, "exchange accept function",
                                   replex_accept, "");

    std::string movestats;
    args.addParameter<std::string>({"movestats"},
                                 "specify the move statistics to show. Could be pertype, "
                                 "perdomain, all, none",
                                  movestats, "pertype");

    int nb_threads;
    args.addParameter<int>({"nb-threads"}, "number of threads",
                           nb_threads, omp_get_max_threads());

    std::string replex_verbosity;
    args.addParameter<std::string>({"replex-verbosity"}, "exchange log verbosity (stats, all, none)",
                                   replex_verbosity, "stats");

    ////////////////////////////////////////////////////

    std::cout << "CL fails if invalid: " << args.failsIfInvalid() << std::endl;
    std::cout << "CL accept other param: " << args.acceptUnregisteredParams() << std::endl;

    args.parse();

    std::cout << "CL is valid: " << args.isValid() << std::endl;

    ////////////////////////////////////////////////////

    std::cout << "hasKey help: " << args.hasKey("help") << std::endl;
    std::cout << "hasKey version: " << args.hasKey("version") << std::endl;

    std::cout << "values multidir (" << multidir.size() << ") ";
    for(auto& dr : multidir){
        std::cout << " " << dr;
    }
    std::cout << std::endl;

    std::cout << "hasKey help: " << args.hasKey("help") << std::endl;
    std::cout << "config: " << config << std::endl;
    std::cout << "backup: " << backup << std::endl;
    std::cout << "rerun: " << rerun << std::endl;
    std::cout << "restart: " << restart << std::endl;
    std::cout << "version: " << version << std::endl;
    std::cout << "replex: " << replex << std::endl;
    std::cout << "replex_stat: " << replex_stat << std::endl;
    std::cout << "replex_accept: " << replex_accept << std::endl;
    std::cout << "movestats: " << movestats << std::endl;
    std::cout << "nb_threads: " << nb_threads << std::endl;
    std::cout << "replex_verbosity: " << replex_verbosity << std::endl;

    ////////////////////////////////////////////////////

    args.printHelp(std::cout);

    return 0;
}
