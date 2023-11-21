#include "ArgParser.h"

using namespace ArgumentParser;

std::string temp_key_for_string;

ArgParser::ArgParser(const std::string& name)
        : parser_name{name} {}

ArgParser::StringArgument& ArgParser::AddStringArgument(const std::string& full_name, const std::string& describe) {
    StringArgument* new_argument = new StringArgument(full_name);
    new_argument->TakeDescription(describe);
    array_string.push_back(new_argument);
    return *new_argument;
}

ArgParser::StringArgument& ArgParser::AddStringArgument(char short_name, const std::string& full_name, const std::string& describe) {
    StringArgument* new_argument = new StringArgument(short_name, full_name);
    new_argument->TakeDescription(describe);
    array_string.push_back(new_argument);
    return *new_argument;
}

std::string& ArgParser::GetStringValue(const std::string& s, uint64_t index) {
    for (const auto& argument : array_string) {
        if (argument->GetNameString() == s) {
            if (argument->GetMultiString()) {
                temp_key_for_string = argument->GetKeysString()[index];
                return temp_key_for_string;
            } else {
                temp_key_for_string = argument->GetKeyString();
                return temp_key_for_string;
            }
        }
    }
    return (std::string&) "ERROR";
}

ArgParser::IntArgument& ArgParser::AddIntArgument(const std::string& full_name, const std::string& describe) {
    IntArgument* new_argument = new IntArgument(full_name);
    new_argument->TakeDescription(describe);
    array_int.push_back(new_argument);
    return *new_argument;
}

ArgParser::IntArgument& ArgParser::AddIntArgument(char short_name, const std::string& full_name, const std::string& describe) {
    IntArgument* new_argument = new IntArgument(short_name, full_name);
    new_argument->TakeDescription(describe);
    array_int.push_back(new_argument);
    return *new_argument;
}

bool ArgumentParser::ArgParser::Parse(int argc, char** argv) {
    std::vector<std::string> arr;
    for (int i = 0; i < argc; ++i) {
        arr.push_back(argv[i]);
    }
    return Parse(arr);
}

bool ArgParser::Parse(const std::vector<std::string>& arr) {
    if (arr.empty()) {
        return true;
    }

    if (arr.size() == 1) {
        return CheckDefaultArguments();
    }

    for (int i = 1; i < arr.size(); i++) {
        std::string value;

        if (arr[i][0] == '-') {
            if (arr[i].length() <= 1) {
                std::cerr << "Error";
                return false;
            }

            if (arr[i][1] == '-') {
                if (ProcessLongArgument(arr[i], value)) {
                    if (use_help) {
                        return true;
                    }
                    continue;
                } else {
                    return false;
                }
            } else {
                if (ProcessShortArgument(arr[i], value)) {
                    continue;
                } else {
                    return false;
                }
            }
        } else {
            if (ProcessPositionalArgument(arr[i])) {
                continue;
            } else {
                return false;
            }
        }
    }

    return CheckMultiValueArguments();
}

bool ArgParser::CheckDefaultArguments() {
    if (array_string.empty() && array_int.empty() && array_flag.empty()) {
        return true;
    }

    for (const auto& string_arg : array_string) {
        if (!string_arg->HaveDefaultString()) {
            return false;
        }
    }

    for (const auto& int_arg : array_int) {
        if (!int_arg->HaveDefaultInt()) {
            return false;
        }
    }

    for (const auto& flag_arg : array_flag) {
        if (!flag_arg->GetDefault()) {
            return false;
        }
    }

    return true;
}

bool ArgParser::ProcessLongArgument(const std::string& arg, std::string& value) {
    std::string current_param;
    size_t index = arg.find('=');

    if (index == std::string::npos) {
        current_param = arg.substr(2);

        if (have_help && current_param == name_help) {
            use_help = true;
            std::cout << HelpDescription();
            return true;
        }

        bool have_flag = false;

        for (const auto& flag_arg : array_flag) {
            if (flag_arg->GetFlag() == current_param) {
                flag_arg->TakeKey();
                flag_arg->PushValueToPartner(flag_arg);
                have_flag = true;
                break;
            }
        }

        return have_flag;
    } else {
        current_param = arg.substr(2, index - 2);

        if (index == arg.length() - 1) {
            return false;
        }

        value = arg.substr(index + 1);
    }

    for (const auto& string_arg : array_string) {
        if (string_arg->GetNameString() == current_param) {
            string_arg->TakeKeyString(value);
            string_arg->PushValueToPartner(string_arg, value);
            break;
        }
    }

    for (const auto& int_arg : array_int) {
        if (int_arg->GetNameInt() == current_param) {
            if (int_arg->GetMultiInt()) {
                int_arg->TakeKeysInt(std::stoi(value));
                int_arg->PushValueToPartnerMulti(int_arg, value);
            } else {
                int_arg->TakeKeyInt(std::stoi(value));
                int_arg->PushValueToPartner(int_arg, value);
            }
            break;
        }
    }

    return true;
}

bool ArgParser::ProcessShortArgument(const std::string& arg, std::string& value) {
    char current = arg[1];

    if (have_help && current == short_help) {
        std::cout << HelpDescription();
        return true;
    }

    bool have_flag = false;

    for (const auto& flag_arg : array_flag) {
        if (flag_arg->GetShortFlag() == current) {
            flag_arg->TakeKey();
            flag_arg->PushValueToPartner(flag_arg);
            have_flag = true;
            break;
        }
    }

    if (have_flag) {
        for (int l = 2; l < arg.length(); l++) {
            for (const auto& flag_arg : array_flag) {
                if (flag_arg->GetShortFlag() == arg[l]) {
                    flag_arg->TakeKey();
                    flag_arg->PushValueToPartner(flag_arg);
                    break;
                }
            }
        }
        return true;
    }

    if (arg.length() == 2 && !have_flag) {
        return false;
    }

    if (arg.length() >= 3 && arg[2] != '=') {
        return false;
    }

    if (arg.length() < 4) {
        return false;
    }

    value = arg.substr(3);

    for (const auto& string_arg : array_string) {
        if (string_arg->GetShortString() == current) {
            string_arg->TakeKeyString(value);
            string_arg->PushValueToPartner(string_arg, value);
            break;
        }
    }

    for (const auto& int_arg : array_int) {
        if (int_arg->GetShortInt() == current) {
            if (int_arg->GetMultiInt()) {
                int_arg->TakeKeysInt(std::stoi(value));
                int_arg->PushValueToPartnerMulti(int_arg, value);
            } else {
                int_arg->TakeKeyInt(std::stoi(value));
                int_arg->PushValueToPartner(int_arg, value);
            }
            break;
        }
    }

    return true;
}

bool ArgParser::ProcessPositionalArgument(const std::string& arg) {
    for (const auto& int_arg : array_int) {
        if (int_arg->PositionGet()) {
            if (int_arg->GetMultiInt()) {
                int_arg->TakeKeysInt(std::stoi(arg));
                int_arg->PushValueToPartnerMulti(int_arg, const_cast<std::string&>(arg));
            } else {
                int_arg->TakeKeyInt(std::stoi(arg));
                int_arg->PushValueToPartner(int_arg, const_cast<std::string&>(arg));
            }
            break;
        }
    }
    return true;
}

bool ArgParser::CheckMultiValueArguments() {
    for (const auto& int_arg : array_int) {
        if (int_arg->GetMultiInt() && int_arg->GetKeysInt().size() < int_arg->GetMinInt()) {
            return false;
        }
    }

    for (const auto& string_arg : array_string) {
        if (string_arg->GetMultiString() && string_arg->GetKeysString().size() < string_arg->GetMinString()) {
            return false;
        }
    }

    return true;
}

int ArgParser::GetIntValue(const std::string& s, uint64_t index) {
    for (const auto& argument : array_int) {
        if (argument->GetNameInt() == s) {
            if (argument->GetMultiInt()) {
                return argument->GetKeysInt()[index];
            } else {
                return argument->GetKeyInt();
            }
        }
    }
    return 0;
}

ArgParser::FlagArgument& ArgParser::AddFlag(const std::string& full_name, const std::string& describe) {
    FlagArgument* new_argument = new FlagArgument(full_name);
    new_argument->TakeDescription(describe);
    array_flag.push_back(new_argument);
    return *new_argument;
}

ArgParser::FlagArgument& ArgParser::AddFlag(char short_name, const std::string& full_name, const std::string& describe) {
    FlagArgument* new_argument = new FlagArgument(short_name, full_name);
    new_argument->TakeDescription(describe);
    array_flag.push_back(new_argument);
    return *new_argument;
}

bool ArgParser::GetFlag(const std::string& s) {
    for (const auto& argument : array_flag) {
        if (argument->GetFlag() == s) {
            return argument->GetKey();
        }
    }
    return false;
}

void ArgParser::AddHelp(char short_name, const std::string& full_name, const std::string& describe) {
    have_help = true;
    name_help = full_name;
    short_help = short_name;
    help_argument = describe;
}

std::string ArgParser::GenerateStringParameterDescription(const StringArgument* param) {
    std::string description = '-' + param->GetShortString() + ", --" + param->GetNameString() +
                              "=<string>,  " + param->GetDescription();
    if (param->GetMinString() != 0) {
        description += " [repeated, min args = " + std::to_string(param->GetMinString()) + "]\n";
    }
    if (param->HaveDefaultString()) {
        description += "[default = " + param->GetKeysString()[0] + "]\n";
    }
    return description;
}

std::string ArgParser::GenerateIntParameterDescription(const IntArgument* param) {
    std::string description = '-' + param->GetShortInt() + ", --" + param->GetNameInt() +
                              "=<int>,  " + param->GetDescription();
    if (param->GetMinInt() != 0) {
        description += " [repeated, min args = " + std::to_string(param->GetMinInt()) + "]\n";
    }
    if (param->HaveDefaultInt()) {
        description += "[default = " + std::to_string(param->GetKeyInt()) + "]\n";
    }
    return description;
}

std::string ArgParser::GenerateFlagDescription(const FlagArgument* flag) {
    std::string description = '-' + (flag->GetShortFlag()) + ", --" + flag->GetFlag() +
                              "=<flag>,  " + flag->GetDefinition();
    if (flag->GetDefault()) {
        if (flag->GetKey()) {
            description += "[default = true]\n";
        } else {
            description += "[default = false]\n";
        }
    }
    return description;
}

const std::string ArgParser::HelpDescription() {
    std::string ans = parser_name + '\n' + help_argument + '\n' + '\n';

    for (const auto& param : array_string) {
        ans += GenerateStringParameterDescription(param);
    }

    for (const auto& param : array_int) {
        ans += GenerateIntParameterDescription(param);
    }

    for (const auto& flag : array_flag) {
        ans += GenerateFlagDescription(flag);
    }

    return ans;
}

bool ArgParser::Help() {
    return use_help;
}