#ifndef CLSIMPLE_HPP
#define CLSIMPLE_HPP

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <regex>
#include <sstream>
#include <iostream> // TODO

class CLsimple{
    template <class ParamType>
    static ParamType Convert(const std::string& str, bool* outFlag = nullptr){
        std::istringstream iss(str, std::istringstream::in);
        ParamType value;
        iss >> value;
        if(outFlag){
            (*outFlag) = bool(iss.eof());
        }
        return value;
    }

    class AbstractParam{
        const std::string _key;
        const std::string _help;
        const bool _isMulti;

    public:
        AbstractParam(std::string inKey,
                      std::string inHelp,
                      const bool inIsMulti):
            _key(std::move(inKey)), _help(std::move(inHelp)), _isMulti(inIsMulti){}

        std::string getKey() const{
            return _key;
        }

        bool isMulti() const {
            return _isMulti;
        }

        virtual bool applyValue(const std::string& inValue) = 0;
        virtual void applyDefault() = 0;
    };

    template <class ParamType>
    class MultiParam : public AbstractParam{
        std::optional<std::vector<ParamType>> _variable;
        const std::vector<ParamType> _default;

    public:
        MultiParam(std::string inKey,
                   std::string inHelp,
                   std::optional<std::vector<ParamType>> inVariable,
                   std::vector<ParamType> inDefault):
            AbstractParam(std::move(inKey), std::move(inHelp), false),
            _variable(std::move(inVariable)),
            _default(std::move(inDefault)){}

        bool applyValue(const std::string& inValue) final{
            bool convertionOk;
            auto value = Convert<ParamType>(inValue, &convertionOk);
            if(_variable && convertionOk){
                _variable->push_back(value);
                return true;
            }
            return false;
        }
        void applyDefault() final{
            if(_variable){
                _variable = _default;
            }
        }
    };


    template <class ParamType>
    class Param : public AbstractParam{
        std::optional<ParamType> _variable;
        const ParamType _default;

    public:
        Param(std::string inKey,
              std::string inHelp,
              std::optional<ParamType> inVariable,
              ParamType inDefault):
            AbstractParam(std::move(inKey), std::move(inHelp), true),
            _variable(std::move(inVariable)),
            _default(std::move(inDefault)){}

        bool applyValue(const std::string& inValue) final{
            bool convertionOk;
            auto value = Convert<ParamType>(inValue, &convertionOk);
            if(_variable && convertionOk){
                _variable = _default;
                return true;
            }
            return false;
        }
        void applyDefault() final{
            if(_variable){
                _variable = _default;
            }
        }
    };

    static bool StrSeemsAKey(const std::string& str){
        std::regex keyFormat("-{1,2}(\\D|[0-9]\\D)");
        std::cout << "StrSeemsAKey = "<< str << " => " << bool(std::regex_search(str, keyFormat)) << std::endl;//TODO
        return std::regex_search(str, keyFormat);
    }

    static std::string StrToKey(const std::string& str){
        std::regex keyValueFormat("-{1,2}([^=]+).*");
        std::sregex_iterator iter = std::sregex_iterator(str.begin(), str.end(), keyValueFormat);
        if(iter == std::sregex_iterator()){
            return "INVALID-FORMAT";
        }
        std::cout << "StrToKey = "<< str << " => " << *(iter.sub_match()).str() << std::endl;//TODO
        return (*iter).str();
    }

    static bool IsKeyValueFormat(const std::string& str){
        std::regex keyValueFormat("--[^=]+=.+");
        std::cout << "IsKeyValueFormat = "<< str << " => " << bool(std::regex_search(str, keyValueFormat)) << std::endl;//TODO
        return std::regex_search(str, keyValueFormat);
    }

    static std::pair<std::string,std::string> SplitKeyValue(const std::string& str){
        std::regex keyValueFormat("--([^=]+)=(.+)");
        std::sregex_iterator iter = std::sregex_iterator(str.begin(), str.end(), keyValueFormat);
        if(iter == std::sregex_iterator() || std::distance(iter, std::sregex_iterator()) < 2){
            return std::pair<std::string,std::string>();
        }
        const std::string key = (*iter).str();
        (++iter);
        const std::string value = (*iter).str();
        std::cout << "SplitKeyValue = "<< str << " => " << key << " / " << value << std::endl;//TODO
        return std::pair<std::string,std::string>(std::move(key), std::move(value));
    }


    std::vector<std::string> _argv;
    std::vector<std::unique_ptr<AbstractParam>> _params;

    bool _failsIfInvalid;
    bool _acceptUnregisteredParams;
    bool _isValid;

public:
    CLsimple(const int argc, char** argv,
             const bool inFailsIfInvalid = true,
             const bool inAcceptUnregisteredParams = true)
        : _failsIfInvalid(inFailsIfInvalid),
          _acceptUnregisteredParams(inAcceptUnregisteredParams),
          _isValid(true) {
        _argv.reserve(argc-1);
        for(int idxArg = 1 ; idxArg < argc ; ++idxArg){
            _argv.emplace_back(argv[idxArg]);
        }
    }

    CLsimple(const CLsimple&) = delete;
    CLsimple& operator=(const CLsimple&) = delete;

    bool failsIfInvalid() const{
        return _failsIfInvalid;
    }

    bool acceptUnregisteredParams() const{
        return _acceptUnregisteredParams;
    }

    bool isValid() const{
        return _isValid;
    }

    int getKeyPos(const std::string& inKey) const{
        for(int idxArg = 0 ; idxArg < int(_argv.size()) ; ++idxArg){
            if(StrSeemsAKey(_argv[idxArg]) && StrToKey(_argv[idxArg]) == inKey){
                return idxArg;
            }
        }
        return -1;
    }

    bool hasKey(const std::string& inKey) const{
        return getKeyPos(inKey) != -1;
    }

    void parse(){
        bool parseIsOK = true;
        int usedFields = 0;

        for(auto& param : _params){
            const int pos = getKeyPos(param->getKey());
            if(pos == -1){
                param->applyDefault();
            }
            else{
                usedFields += 1;
                if(IsKeyValueFormat(_argv[pos])){
                    const auto keyValue = SplitKeyValue(_argv[pos]);
                    parseIsOK &= param->applyValue(std::get<1>(keyValue));
                    usedFields += 1;
                }
                else if(pos+1 != int(_argv.size())){
                    if(param->isMulti()){
                        int idxVal = pos + 1;
                        while(idxVal != int(_argv.size()) && !StrSeemsAKey(_argv[idxVal])){
                            parseIsOK &= param->applyValue(_argv[idxVal]);
                            idxVal += 1;
                        }
                        if(idxVal == pos + 1){
                            param->applyDefault();
                            parseIsOK = false;
                        }
                    }
                    else{
                        parseIsOK &= param->applyValue(_argv[pos+1]);
                    }
                }
                else{
                    param->applyDefault();
                    parseIsOK = false;
                }
            }
        }
        if(_failsIfInvalid){
            _isValid = parseIsOK;
            if(!_acceptUnregisteredParams && usedFields != int(_argv.size())){
                _isValid = false;
            }
        }
    }

    template <class ParamType>
    void addMultiParameter(std::string inKey, std::string inHelp,
                           std::optional<std::vector<ParamType>> inVariable = std::nullopt,
                           std::vector<ParamType> inDefaultValue = std::vector<ParamType>()){
        std::unique_ptr<AbstractParam> newParam(new MultiParam<ParamType>(
                                                    std::move(inKey),
                                                    std::move(inHelp),
                                                    std::move(inVariable),
                                                    std::move(inDefaultValue)
                                                    ));
        _params.emplace_back(std::move(newParam));
    }

    template <class ParamType>
    void addParameter(std::string inKey, std::string inHelp,
                      std::optional<ParamType> inVariable = std::nullopt,
                      ParamType inDefaultValue = ParamType()){
        std::unique_ptr<AbstractParam> newParam(new Param<ParamType>(
                                                    std::move(inKey),
                                                    std::move(inHelp),
                                                    std::move(inVariable),
                                                    std::move(inDefaultValue)
                                                    ));
        _params.emplace_back(std::move(newParam));
    }
};


#endif
