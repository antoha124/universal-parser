#pragma once
#include <iostream>
#include <vector>

namespace ArgumentParser {
    class ArgParser {
    private:
        bool CheckDefaultArguments();
        bool ProcessLongArgument(const std::string& arg, std::string& value);
        bool ProcessShortArgument(const std::string& arg, std::string& value);
        bool ProcessPositionalArgument(const std::string& arg);
        bool CheckMultiValueArguments();
        std::string name_help;
        char short_help;
        std::string help_argument;
        bool have_help = false;
        bool use_help = false;
        class StringArgument {
        public:
            StringArgument(const std::string& name) : name{name} {}
            StringArgument(char n, const std::string& name) : name{name}, short_name{n} {}

            void Default(const std::string& value) {
                key = value;
                default_string = true;
            }

            void StoreValue(std::string& value) {
                linker.push_back(&value);
            }

            void TakeKeyString(const std::string value) {
                key = value;
            }

            void PushValueToPartner(StringArgument* string_arg, std::string value) {
                for (const auto& temp : string_arg->linker) {
                    *temp = value;
                }
            }

            std::string GetNameString() const {
                return name;
            }

            char GetShortString() const {
                return short_name;
            }

            bool GetMultiString() const {
                return multi_value;
            }

            std::string GetKeyString() const {
                return key;
            }

            std::vector<std::string> GetKeysString() const {
                return keys;
            }

            bool HaveDefaultString() const {
                return default_string;
            }

            std::string GetDescription() const {
                return definition;
            }

            uint64_t GetMinString() const {
                return min_args_count;
            }

            StringArgument& MultiValue(uint64_t min_count = 0) {
                multi_value = true;
                min_args_count = min_count;
                return *this;
            }

            StringArgument& TakeDescription(const std::string describe) {
                definition = describe;
                return *this;
            }

        private:
            char short_name = ' ';
            bool default_string = false;
            bool multi_value = false;
            uint64_t min_args_count = 0;
            std::string key;
            std::vector<std::string> keys;
            std::vector<std::string*> linker;
            std::string name;
            std::string definition;
        };

        class IntArgument {
        public:
            IntArgument(const std::string& param) : name{param} {}
            IntArgument(char p, std::string param) : name{std::move(param)}, short_name{p} {}

            void Default(int value) {
                key = value;
                default_value = true;
            }

            void StoreValue(int value) {
                linker.push_back(&value);
            }

            void PushValueToPartnerMulti(IntArgument* int_arg, std::string& value) {
                for (const auto& temp : int_arg->linker_multi) {
                    (*temp).push_back(std::stoi(value));
                }
            }

            void PushValueToPartner(IntArgument* int_arg, std::string& value) {
                for (const auto& temp : int_arg->linker) {
                    *temp = std::stoi(value);
                }
            }

            bool PositionGet() const {
                return positional;
            }

            void StoreValues(std::vector<int>& numbers) {
                linker_multi.push_back(&numbers);
            }

            void TakeKeyInt(const int value) {
                key = value;
            }

            void TakeKeysInt(const int value) {
                keys.push_back(value);
            }

            std::string GetNameInt() const {
                return name;
            }

            char GetShortInt() const {
                return short_name;
            }

            bool GetMultiInt() const {
                return multi_value;
            }

            int GetKeyInt() const {
                return key;
            }

            std::vector<int> GetKeysInt() const {
                return keys;
            }

            bool HaveDefaultInt() const {
                return default_value;
            }

            std::string GetDescription() const {
                return definition;
            }

            uint64_t GetMinInt() const {
                return min_args_count;
            }

            IntArgument& MultiValue(uint64_t min_count = 0) {
                multi_value = true;
                min_args_count = min_count;
                return *this;
            }

            IntArgument& Positional() {
                positional = true;
                return *this;
            }

            IntArgument& TakeDescription(const std::string describe) {
                definition = describe;
                return *this;
            }

        private:
            char short_name = ' ';
            bool default_value = false;
            bool positional = false;
            bool multi_value = false;
            uint64_t min_args_count = 0;
            std::vector<int*> linker;
            std::vector<std::vector<int>*> linker_multi;
            std::vector<int> keys;
            std::string definition;
            std::string name;
            int key;
        };

        class FlagArgument {
        public:
            FlagArgument(const std::string name) : flag{name} {}
            FlagArgument(const char n, const std::string name) : short_flag{n}, flag{name} {}

            void Default(bool value) {
                key = value;
                default_flag = true;
            }

            std::string GetFlag() const {
                return flag;
            }

            char GetShortFlag() const {
                return short_flag;
            }

            void TakeKey() {
                key = true;
            }

            bool GetKey() const {
                return key;
            }

            std::string GetDefinition() const {
                return definition;
            }

            void PushValueToPartner(FlagArgument* flag_arg) {
                for (const auto& linker : flag_arg->linker) {
                    *linker = true;
                }
            }

            bool GetDefault() const {
                return default_flag;
            }

            void StoreValue(bool& value) {
                value = key;
                linker.push_back(&value);
            }

            FlagArgument& TakeDescription(const std::string describe) {
                definition = describe;
                return *this;
            }

        private:
            char short_flag = ' ';
            bool default_flag = false;
            std::string flag;
            std::string definition;
            bool key = false;
            std::vector<bool*> linker;
        };
        std::vector<StringArgument*> array_string;
        std::vector<IntArgument*> array_int;
        std::vector<FlagArgument*> array_flag;

    public:
        std::string parser_name;

        ArgParser(const std::string& s);

        bool Parse(int argc, char** argv);
        bool Parse(const std::vector<std::string>& arr);
        StringArgument& AddStringArgument(const std::string& full_name, const std::string& describe = "");
        StringArgument& AddStringArgument(char short_name, const std::string& full_name, const std::string& describe = "");

        std::string& GetStringValue(const std::string& s, uint64_t index = 0);

        IntArgument& AddIntArgument(const std::string& full_name, const std::string& describe = "");
        IntArgument& AddIntArgument(char short_name, const std::string& full_name, const std::string& describe = "");

        int GetIntValue(const std::string& s, uint64_t index = 0);

        FlagArgument& AddFlag(const std::string& full_name, const std::string& describe = "");
        FlagArgument& AddFlag(char short_name, const std::string& full_name, const std::string& describe = "");

        bool GetFlag(const std::string& s);

        void AddHelp(char short_name, const std::string& full_name, const std::string& describe);
        const std::string HelpDescription();
        std::string GenerateStringParameterDescription(const StringArgument* param);
        std::string GenerateIntParameterDescription(const IntArgument* param);
        std::string GenerateFlagDescription(const FlagArgument* flag);

        bool Help();

        ~ArgParser() {
            array_string.clear();
            array_flag.clear();
            array_int.clear();
        }
    };
}  // namespace ArgumentParser