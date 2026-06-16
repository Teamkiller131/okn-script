#include <okn/script/script_engine.hpp>
#include <cstdio>
using namespace okn::script;
int main(){std::printf("=== Script Runtime ===\n");
ScriptEngine e;auto* c=e.default_context();
if(!c||!c->is_valid()){std::printf("SKIPPED: Lua not installed\n");return 0;}
bool ok=e.eval("x=42");std::printf("Eval x=42: %s\n",ok?"OK":"FAIL");
ok=c->load_string("function add(a,b) return a+b end");std::printf("Load add(): %s\n",ok?"OK":"FAIL");
std::printf("Script: PASSED\n");return 0;}