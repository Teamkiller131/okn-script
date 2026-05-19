#include <okn/script/binding/binding_generator.hpp>
namespace okn::script {
BindingGenerator::BindingGenerator() = default;
BindingGenerator::~BindingGenerator() = default;
auto BindingGenerator::register_type(const std::string& type_name) -> void {
    BindingEntry entry; entry.name = type_name; entry.type_signature = "class"; entries_.push_back(entry);
}
auto BindingGenerator::register_method(const std::string& type_name, const std::string& method_name, const std::string& signature) -> void {
    BindingEntry entry; entry.name = type_name + "." + method_name; entry.type_signature = signature; entry.is_method = true; entries_.push_back(entry);
}
auto BindingGenerator::register_function(const std::string& name, const std::string& signature) -> void {
    BindingEntry entry; entry.name = name; entry.type_signature = signature; entries_.push_back(entry);
}
auto BindingGenerator::generate(const std::string& target_language) -> std::string {
    std::string result;
    for (const auto& e : entries_) {
        if (target_language == "cpp") result += format_cpp_binding(e) + "\n";
        else if (target_language == "lua") result += format_lua_binding(e) + "\n";
        else if (target_language == "js") result += format_js_binding(e) + "\n";
        else if (target_language == "python") result += format_python_binding(e) + "\n";
    }
    return result;
}
auto BindingGenerator::generate_cpp(const std::string&) -> bool { return true; }
auto BindingGenerator::generate_lua(const std::string&) -> bool { return true; }
auto BindingGenerator::generate_js(const std::string&) -> bool { return true; }
auto BindingGenerator::generate_python(const std::string&) -> bool { return true; }
auto BindingGenerator::entries() const -> std::vector<BindingEntry> { return entries_; }
auto BindingGenerator::clear() -> void { entries_.clear(); }
auto BindingGenerator::format_cpp_binding(const BindingEntry& e) -> std::string { return "// [cpp] bind: " + e.name + " -> " + e.type_signature; }
auto BindingGenerator::format_lua_binding(const BindingEntry& e) -> std::string { return "-- [lua] bind: " + e.name + " -> " + e.type_signature; }
auto BindingGenerator::format_js_binding(const BindingEntry& e) -> std::string { return "// [js] bind: " + e.name + " -> " + e.type_signature; }
auto BindingGenerator::format_python_binding(const BindingEntry& e) -> std::string { return "# [py] bind: " + e.name + " -> " + e.type_signature; }
} // namespace okn::script
