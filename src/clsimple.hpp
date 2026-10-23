//////////////////////////////////////////////////////
/// MIT License, 2022
/// Author: Bérenger Bramas
/// See https://gitlab.inria.fr/bramas/clsimple
//////////////////////////////////////////////////////

#ifndef CLSIMPLE_HPP
#define CLSIMPLE_HPP

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <regex>
#include <sstream>
#include <typeinfo>
#include <type_traits>
#include <limits>
#include <cassert>
#include <set>
#include <unordered_map>
#include <functional>
#include <initializer_list>
#include <utility>

class CLsimple{
    template <class ParamType>
    static ParamType Convert(const std::string& str, bool* outFlag = nullptr){
        ParamType value{};
        bool valid = false;
        if constexpr(std::is_same<ParamType, std::string>::value){
            value = str;
            valid = true;
        }
        else if constexpr(std::is_same<ParamType, bool>::value){
            if(str == "true" || str == "True" || str == "TRUE"){
                value = true;
                valid = true;
            }
            else if(str == "false" || str == "False" || str == "FALSE"){
                valid = true;
            }
            else{
                bool integerValid = false;
                const int integer = Convert<int>(str, &integerValid);
                value = integer != 0;
                valid = integerValid;
            }
        }
        else if(!str.empty()){
            std::istringstream iss(str);
            // Unsigned extraction otherwise accepts negative numbers by wrapping.
            if constexpr(std::is_unsigned<ParamType>::value){
                iss >> std::ws;
                if(iss.peek() == '-'){
                    if(outFlag){ *outFlag = false; }
                    return value;
                }
            }
            iss >> value;
            valid = !iss.fail() && iss.peek() == std::char_traits<char>::eof();
        }
        if(outFlag){
            *outFlag = valid;
        }
        return value;
    }

    template <class ParamType>
    static std::string TypeToStr(){
        if constexpr(std::is_same<ParamType,bool>::value){
            return "Boolean";
        }
        if constexpr(std::numeric_limits<ParamType>::is_integer){
            return "Integer";
        }
        if constexpr(std::is_floating_point<ParamType>::value){
            return "Real number";
        }
        if constexpr(std::is_same<ParamType,std::string>::value){
            return "String";
        }
        return typeid(std::remove_reference_t<ParamType>).name();
    }


    enum ParamTypes{
        NoArg,
        Multi,
        Single
    };

    class AbstractParam{
        const std::vector<std::string> _keys;
        const std::string _help;
        const ParamTypes _paramType;
        const int _mandatoryGroup;

    public:
        AbstractParam(std::vector<std::string> inKeys,
                      std::string inHelp,
                      const ParamTypes inType,
                      const int inMandatoryGroup):
            _keys(std::move(inKeys)), _help(std::move(inHelp)),
            _paramType(inType), _mandatoryGroup(inMandatoryGroup){
            assert(_keys.size());
        }

        virtual ~AbstractParam(){}

        std::vector<std::string> getKeys() const{
            return _keys;
        }

        std::string getHelp() const{
            return _help;
        }

        bool isSingle() const {
            return _paramType == Single;
        }

        bool isMulti() const {
            return _paramType == Multi;
        }

        bool isNoArg() const {
            return _paramType == NoArg;
        }

        bool isMandatory() const{
            return _mandatoryGroup != NotMandatory;
        }

        int mandatoryGroup() const{
            return _mandatoryGroup;
        }

        virtual bool applyValue(const std::string& inValue) = 0;
        virtual void applyDefault() = 0;
        virtual void resetValue(){ applyDefault(); }
        virtual std::string getTypeStr() const = 0;
    };

    template <class ParamType>
    class MultiParam : public AbstractParam{
        std::optional<std::reference_wrapper<std::vector<ParamType>>> _variable;
        const std::vector<ParamType> _default;

    public:
        MultiParam(std::vector<std::string> inKeys,
                   std::string inHelp,
                   const int inMandatoryGroup,
                   std::optional<std::reference_wrapper<std::vector<ParamType>>> inVariable,
                   std::vector<ParamType> inDefault):
            AbstractParam(std::move(inKeys), std::move(inHelp), Multi, inMandatoryGroup),
            _variable(std::move(inVariable)),
            _default(std::move(inDefault)){}

        bool applyValue(const std::string& inValue) final{
            bool conversionOk = false;
            const auto value = Convert<ParamType>(inValue, &conversionOk);
            if(conversionOk && _variable){
                _variable->get().push_back(value);
            }
            return conversionOk;
        }
        void resetValue() final{
            if(_variable){
                _variable->get().clear();
            }
        }
        void applyDefault() final{
            if(_variable){
                _variable->get() = _default;
            }
        }
        std::string getTypeStr() const final{
            return "List of " + TypeToStr<ParamType>() + "s";
        }
    };


    template <class ParamType>
    class Param : public AbstractParam{
        std::optional<std::reference_wrapper<ParamType>> _variable;
        const ParamType _default;

    public:
        Param(std::vector<std::string> inKeys,
              std::string inHelp,
              const int inMandatoryGroup,
              std::optional<std::reference_wrapper<ParamType>> inVariable,
              ParamType inDefault):
            AbstractParam(std::move(inKeys), std::move(inHelp), Single, inMandatoryGroup),
            _variable(std::move(inVariable)),
            _default(std::move(inDefault)){}

        bool applyValue(const std::string& inValue) final{
            bool conversionOk = false;
            const auto value = Convert<ParamType>(inValue, &conversionOk);
            if(conversionOk && _variable){
                _variable->get() = value;
            }
            return conversionOk;
        }
        void applyDefault() final{
            if(_variable){
                _variable->get() = _default;
            }
        }
        std::string getTypeStr() const final{
            return TypeToStr<ParamType>();
        }
    };

    class ParamNoArg : public AbstractParam{
    public:
        ParamNoArg(std::vector<std::string> inKeys,
              std::string inHelp,
                   const int inMandatoryGroup):
            AbstractParam(std::move(inKeys), std::move(inHelp), NoArg, inMandatoryGroup){}

        bool applyValue([[maybe_unused]] const std::string& inValue) final{
            return true;
        }
        void applyDefault() final{
        }
        std::string getTypeStr() const final{
            return "(No argument)";
        }
    };

    static bool StrSeemsAKey(const std::string& str){
        if(str.size() < 2 || str[0] != '-'){
            return false;
        }
        static const std::regex numberFormat(
            R"(^-([0-9]+(\.[0-9]*)?|\.[0-9]+)([eE][+-]?[0-9]+)?$)");
        return !std::regex_match(str, numberFormat);
    }

    static std::string StrToKey(const std::string& str){
        const std::size_t begin = str.compare(0, 2, "--") == 0 ? 2 : 1;
        return str.substr(begin, str.find('=', begin) - begin);
    }

    static bool IsKeyValueFormat(const std::string& str){
        return str.compare(0, 2, "--") == 0 && str.find('=', 2) != std::string::npos
                && str.find('=', 2) > 2;
    }

    static std::pair<std::string,std::string> SplitKeyValue(const std::string& str){
        const auto separator = str.find('=', 2);
        return {str.substr(2, separator - 2), str.substr(separator + 1)};
    }

    template <class ParamType>
    void processParam(ParamType&& param, bool* parseIsOK = nullptr,
                      std::set<int>* usedFields = nullptr, std::ostringstream* inError = nullptr) const {

        int pos = -1;
        for(const auto& key : param->getKeys()){
            pos = getKeyPos(key);
            if(pos != -1){ break; }
        }
        if(parseIsOK){ *parseIsOK = false; }
        if(pos == -1){
            param->applyDefault();
            return;
        }

        param->resetValue();
        if(usedFields){ usedFields->insert(pos); }
        bool valid = true;
        auto apply = [&](const std::string& value){
            if(!param->applyValue(value)){
                valid = false;
                if(inError){
                    *inError << "[ERROR] Error in parsing the value \"" << value
                             << "\" for arg \"" << _argv[pos] << "\"\n";
                }
            }
        };
        const bool inlineValue = IsKeyValueFormat(_argv[pos]);
        if(param->isNoArg()){
            if(inlineValue){
                valid = false;
                if(inError){
                    *inError << "[ERROR] Argument \"" << _argv[pos]
                             << "\" does not accept a value\n";
                }
            }
        }
        else{
            int count = 0;
            if(inlineValue){
                apply(SplitKeyValue(_argv[pos]).second);
                ++count;
            }
            int idxVal = pos + 1;
            while((param->isMulti() || count == 0) && idxVal < int(_argv.size())
                  && !StrSeemsAKey(_argv[idxVal])){
                apply(_argv[idxVal]);
                if(usedFields){ usedFields->insert(idxVal); }
                ++idxVal;
                ++count;
            }
            if(count == 0){
                valid = false;
                if(inError){
                    *inError << "[ERROR] Cannot get value, but we expect one, for arg \""
                             << _argv[pos] << "\"\n";
                }
            }
        }
        if(!valid){ param->applyDefault(); }
        if(parseIsOK){ *parseIsOK = valid; }
    }

    std::string _title;
    std::string _exec;

    std::vector<std::string> _argv;
    std::shared_ptr<std::vector<std::unique_ptr<AbstractParam>>> _params;

    bool _failsIfInvalid;
    bool _acceptUnregisteredParams;
    bool _isValid;

    std::shared_ptr<std::ostringstream> errorStr;

public:
    static const int NotMandatory = -1;

    CLsimple(const std::string inTitle,
             const int argc, const char *const argv[],
             const bool inFailsIfInvalid = true,
             const bool inAcceptUnregisteredParams = false)
        : _title(std::move(inTitle)),
          _params(new std::vector<std::unique_ptr<AbstractParam>>),
          _failsIfInvalid(inFailsIfInvalid),
          _acceptUnregisteredParams(inAcceptUnregisteredParams),
          _isValid(true),
          errorStr(new std::ostringstream){
        if(argc){
            _exec = argv[0];
            _argv.reserve(argc-1);
            for(int idxArg = 1 ; idxArg < argc ; ++idxArg){
                _argv.emplace_back(argv[idxArg]);
            }
        }
    }

    CLsimple(const CLsimple&) = default;
    CLsimple& operator=(const CLsimple&) = default;

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

    bool hasOneOfKeys(const std::vector<std::string>& inKeys) const{
        for(const auto& key : inKeys){
            const int pos = getKeyPos(key);
            if(pos != -1){
                return true;
            }
        }
        return false;
    }

    bool hasOneOfKeys(std::initializer_list<std::string> inKeys) const{
        std::vector<std::string> keys = inKeys;
        return hasOneOfKeys(keys);
    }

    template <class ParamType>
    std::vector<ParamType> getValues(std::vector<std::string> inKeys,
                  std::vector<ParamType> inDefaultValue = std::vector<ParamType>(),
                  bool* outOk = nullptr) const{
        std::vector<ParamType> variable;
        std::optional<std::reference_wrapper<std::vector<ParamType>>> variableRef(variable);
        MultiParam<ParamType> newParam(std::move(inKeys), "", NotMandatory,
                                                    std::move(variableRef),
                                                    std::move(inDefaultValue));
        processParam(&newParam, outOk);
        return variable;
    }

    template <class ParamType>
    ParamType getValue(std::vector<std::string> inKeys,
                  ParamType inDefaultValue = ParamType(),
                  bool* outOk = nullptr) const{
        ParamType variable;
        std::optional<std::reference_wrapper<ParamType>> variableRef(variable);
        Param<ParamType> newParam(std::move(inKeys), "", NotMandatory,
                                                    std::move(variableRef),
                                                    std::move(inDefaultValue));
        processParam(&newParam, outOk);
        return variable;
    }

    bool parse(){
        _isValid = true;
        errorStr->str("");
        errorStr->clear();
        std::unordered_map<int, bool> parseIsOK;
        std::set<int> usedFields;

        // Process param values
        for(auto& param : *_params){
            bool parseIsOKLocal = true;
            processParam(param, &parseIsOKLocal, &usedFields, errorStr.get());
            if(hasOneOfKeys(param->getKeys()) && !parseIsOKLocal){
                _isValid = false;
            }
            if(parseIsOK.find(param->mandatoryGroup()) == parseIsOK.end()){
                parseIsOK[param->mandatoryGroup()] = parseIsOKLocal;
            }
            else{
                parseIsOK[param->mandatoryGroup()] &= parseIsOKLocal;
            }
        }
        if(_failsIfInvalid){
            if(!_acceptUnregisteredParams && usedFields.size() != _argv.size()){
                (*errorStr) << "[ERROR] The following arguments are invalid:\n";
                for(int idx = 0 ; idx < int(_argv.size()) ; ++idx){
                    if(usedFields.find(idx) == usedFields.end()){
                        (*errorStr) << "[ERROR] \"" << _argv[idx] << "\"\n";
                    }
                }
                _isValid = false;
            }
        }
        // Ensure that one mandatory group is used
        std::unordered_map<int, bool> groupStates;
        for(auto& param : *_params){
            if(param->isMandatory()){
                if(groupStates.find(param->mandatoryGroup()) == groupStates.end()){
                    groupStates[param->mandatoryGroup()] = parseIsOK[param->mandatoryGroup()];
                }
                const auto& keys = param->getKeys();
                groupStates[param->mandatoryGroup()] &= hasOneOfKeys(keys);
            }
        }
        if(groupStates.size()){
            bool oneGroupIsOk = false;
            for(auto iter : groupStates){
                oneGroupIsOk = iter.second;
                if(oneGroupIsOk){
                    break;
                }
            }
            if(!oneGroupIsOk){
                (*errorStr) << "[ERROR] No mandatory parameter group was fully provided with valid values.\n";
            }

            _isValid &= oneGroupIsOk;
        }
        return _isValid;
    }

    template <class ParamType>
    void addMultiParameter(std::vector<std::string> inKeys, std::string inHelp,
                           std::optional<std::reference_wrapper<std::vector<ParamType>>> inVariable = std::nullopt,
                           std::vector<ParamType> inDefaultValue = std::vector<ParamType>(),
                           const int inMandatoryGroup = NotMandatory){
        std::unique_ptr<AbstractParam> newParam(new MultiParam<ParamType>(
                                                    std::move(inKeys),
                                                    std::move(inHelp),
                                                    inMandatoryGroup,
                                                    std::move(inVariable),
                                                    std::move(inDefaultValue)
                                                    ));
        (*_params).emplace_back(std::move(newParam));
    }

    template <class ParamType>
    void addParameter(std::vector<std::string> inKeys, std::string inHelp,
                      std::optional<std::reference_wrapper<ParamType>> inVariable = std::nullopt,
                      ParamType inDefaultValue = ParamType(),
                      const int inMandatoryGroup = NotMandatory){
        std::unique_ptr<AbstractParam> newParam(new Param<ParamType>(
                                                    std::move(inKeys),
                                                    std::move(inHelp),
                                                    inMandatoryGroup,
                                                    std::move(inVariable),
                                                    std::move(inDefaultValue)
                                                    ));
        (*_params).emplace_back(std::move(newParam));
    }

    void addParameterNoArg(std::vector<std::string> inKeys, std::string inHelp,
                           const int inMandatoryGroup = NotMandatory){
        std::unique_ptr<AbstractParam> newParam(new ParamNoArg(
                                                    std::move(inKeys),
                                                    std::move(inHelp),
                                                    inMandatoryGroup
                                                    ));
        (*_params).emplace_back(std::move(newParam));
    }

    template <class StreamClass>
    void printHelp(StreamClass& inStream, const bool inPrintError = true) const{
        auto join = [](auto&& elements, auto&& delimiter) -> std::string {
            std::string res;
            for(const auto& elm : elements){
                if(!res.empty()){
                    res += delimiter;
                }
                res += elm;
            }
            return res;
        };

        inStream << "[HELP] \n";
        inStream << "[HELP] " << _title << "\n";
        inStream << "[HELP] \n";
        if(!_isValid){
            inStream << "[HELP] \n";
            inStream << "[HELP] Current parameters are not valid...\n";
            inStream << "[HELP] \n";
        }
        for(auto& param : *_params){
            inStream << "[HELP] Parameter names: {" << join(param->getKeys(),", ") << "}\n";
            inStream << "[HELP]  - Description: " << param->getHelp() << "\n";
            inStream << "[HELP]  - Type: " << param->getTypeStr() << "\n";
            if(param->isMandatory()){
                inStream << "[HELP]  - Is mandatory (group " << param->mandatoryGroup() << ")\n";
            }
            inStream << "[HELP]\n";
        }
        inStream << "[HELP]" << std::endl;


        inStream << "[HELP] Command line with all args: " << _exec << " ";
        for(auto& param : *_params){
            assert(param->getKeys().size());
            if(param->isMulti()){
                inStream << "--" << param->getKeys()[0] << " [" << param->getTypeStr() << "] ";
            }
            else if(param->isSingle()){
                inStream << "--" << param->getKeys()[0] << "=[" << param->getTypeStr() << "] ";
            }
            else{
                inStream << "--" << param->getKeys()[0] << " ";
            }
        }
        inStream << std::endl;
        inStream << "[HELP]" << std::endl;

        if(inPrintError){
            printError(inStream);
        }
    }

    template <class StreamClass>
    void printError(StreamClass& inStream) const{
        const std::string errors = (*errorStr).str();
        if(errors.length()){
            inStream << "[ERROR] There are some errors...\n";
            inStream << errors << std::endl;
        }
    }

    template <class ValType>
    static ValType GetMapping(const std::string& inKey, const std::vector<std::pair<std::string,ValType>>& inMapping,
                              const ValType defaultVal = ValType()){
        for(auto& mp : inMapping){
            if(mp.first == inKey){
                return mp.second;
            }
        }
        return defaultVal;
    }

    template <class ValType>
    static ValType GetMapping(const std::string& inKey,
                              std::initializer_list<std::pair<std::string, ValType>> inMapping,
                              const ValType defaultVal = ValType()){
        for(auto& mp : inMapping){
            if(mp.first == inKey){
                return mp.second;
            }
        }
        return defaultVal;
    }
};


#endif
