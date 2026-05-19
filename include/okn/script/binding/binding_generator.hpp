#pragma once
#include <okn/script/script_types.hpp>
#include <string>
#include <vector>
#include <functional>
namespace okn::script {
struct BindingEntry {
    std::string name;
    std::string type_signature;
    std::string glue_code;
    bool is_method = false;
};
class BindingGenerator {
public:
    BindingGenerator();
    ~BindingGenerator();
    auto register_type(const std::string& type_name) -> void;
    auto register_method(const std::string& type_name, const std::string& method_name, const std::string& signature) -> void;
    auto register_function(const std::string& name, const std::string& signature) -> void;
    auto generate(const std::string& target_language) -> std::string;
    auto generate_cpp(const std::string& output_path) -> bool;
    auto generate_lua(const std::string& output_path) -> bool;
    auto generate_js(const std::string& output_path) -> bool;
    auto generate_python(const std::string& output_path) -> bool;
    [[nodiscard]] auto entries() const -> std::vector<BindingEntry>;
    auto clear() -> void;
private:
    std::vector<BindingEntry> entries_;
    auto format_cpp_binding(const BindingEntry& entry) -> std::string;
    auto format_lua_binding(const BindingEntry& entry) -> std::string;
    auto format_js_binding(const BindingEntry& entry) -> std::string;
    auto format_python_binding(const BindingEntry& entry) -> std::string;
};
} // namespace okn::script
