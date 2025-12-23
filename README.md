# okn-script (OmniKillerNexus Script Module)

`okn-script` 提供 Lua/JavaScript/Python 多语言并存的脚本运行时，自动/半自动绑定生成，热重载与沙箱/配额，调试与性能剖析，Job 协同，事件桥接（ECS/渲染/物理），与 okn-asset 包/变体/字节码预编译对接，并提供 CLI 工具链。支持多语言资源选择/语音包策略，对接 okn-memory 分配器。

## 功能范围
- **多语言运行时**：Lua / JS / Python；脚本 VM/Context 管理，模块加载，协程/Promise/async 协同。
- **绑定/反射**：反射注册表 + 绑定生成器（自动/半自动 glue），暴露类型/函数/事件。
- **热重载**：脚本热更新，状态迁移策略可扩展。
- **沙箱/安全**：配额（时间/指令/内存），白名单/黑名单策略，禁用危险 API。
- **调度与多线程**：主线程执行 + Job System 适配（后台任务），与 okn-platform 的线程/Job 协同。
- **调试与观测**：远程调试（断点/单步/堆栈/变量），日志，性能剖析（耗时/分配统计）。
- **内存**：接入 okn-memory 分配器/配额；VM 内存钩子。
- **事件/桥接**：事件总线桥接 ECS/physics/render；资源/ID/句柄编组（向量/矩阵/颜色/变换/数组/字典等）。
- **资产/包**：与 okn-asset pack/variant 对接；脚本/字节码打包与校验；字节码预编译以缩短启动。
- **本地化**：locale/语音包切换策略桥接。
- **CLI 工具**：绑定生成、字节码编译、lint/格式化、包校验。
- **样例/测试**：Lua/JS/Python 示例，热重载/CLI 示例；VM/绑定/热重载/沙箱/编组/CLI 测试占位。

## 目录概要
- `include/okn/script/`：核心 API、VM/Context、绑定、热重载、沙箱、调度、调试、剖析、内存、事件、资产、本地化、CLI；runtime 子目录（Lua/JS/Python）；binding/sandbox/debug/profile/memory/event/asset/locale/scheduler/cli/utils 等。
- `src/`：与头文件对应实现。
- `samples/`：基础运行、热重载、CLI 示例。
- `tests/`：VM/绑定/热重载/沙箱/编组/CLI 等测试占位。
- `CMakeLists.txt`：主工程与 samples/tests 子目录。

## CMake 选项
- `BUILD_SAMPLES` / `BUILD_TESTS`
- `OKN_SCRIPT_ENABLE_LUA` / `_JS` / `_PYTHON`
- `OKN_SCRIPT_ENABLE_HOT_RELOAD`
- `OKN_SCRIPT_ENABLE_SANDBOX`
- `OKN_SCRIPT_ENABLE_DEBUGGER`
- `OKN_SCRIPT_ENABLE_PROFILE`
- `OKN_SCRIPT_ENABLE_CLI`
- `OKN_SCRIPT_ENABLE_MEMORY_HOOKS`

## 构建
```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . -j
```

## 集成要点
- 绑定生成器基于反射注册表输出 glue，减少手写。
- Job 协同：脚本后台任务通过 scheduler/job_adapter 适配 okn-platform Job。
- 事件与桥接：event_bridge/asset_bridge/locale_bridge 连接 ECS/physics/render 与资源/变体/多语言。
- 安全与观测：sandbox 配额 + debug/profiler hooks；内存钩子接入 okn-memory。