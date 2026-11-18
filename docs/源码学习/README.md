# 源码学习记录

> 目标：以 Warrior 项目中写过的每条系统链路为锚点，往引擎源码深处挖，学原理和设计模式。
> 原则：**按链路读，不通读**。从自己的调用点出发打断点 / 跟调用栈，比对着目录猜文件高效得多。

## 目录结构（按模块分文件夹）

```
docs/源码学习/
├── README.md            # 本文件：索引 + 方法论
├── GAS/                 # GameplayAbilitySystem（能力系统）
├── Input/               # EnhancedInput（增强输入）
├── Animation/           # Montage / AnimNotify / LinkedAnimLayers（动画）
└── Core/                # UObject / 反射 / GC / 委托（地基）
```

## 已完成记录

| 日期 | 模块 | 记录 | 关联项目系统 |
| :--: | :-- | :-- | :-- |
| 2026.11.17 | GAS | [01-TryActivateAbility技能激活链路.md](GAS/01-TryActivateAbility技能激活链路.md) | 技能激活（Day3~5 全部 GA） |

## 待追读路线（按优先级）

1. **GAS**：`CommitAbility`（Cost/Cooldown 真正应用点）→ 失败路径（`AbilityFailedCallbacks`）→ `EndAbility`/`CancelAbility` → `FScopedPredictionWindow` 预测三段式
2. **Input**：`BindAction` → `UInputMappingContext` → `FInputActionValue` 生成链路 → `PlayerInput::ProcessInputStack` 每帧流程
3. **Animation**：`Montage_Play` → `FAnimMontageInstance` → `AnimNotifyState` 成对调用保证 → GameplayEvent 从 Notify 到 ASC 的传递路径
4. **Core**：UObject 反射 / GC 追踪（`UPROPERTY`、`TWeakObjectPtr` 行为）→ 三套委托（`TDelegate`/`TMulticastDelegate`/`TScriptDelegate`）区别

## 阅读方法论

- 以"问题驱动"：每个卡点（如"Montage 播一半停了"）都是最好的源码入口
- 先读 `.h` 注释和类结构，再读 `.cpp`；UE 头文件注释往往是设计意图
- 跳过 `*.generated.h` 和 UHT 展开内容
- 追到反射 / GC 深层看不懂时先标记跳过，补完地基再回来
- 每条记录字段约定：**阅读目标 / 链路梳理 / 修正与补充 / 待确认(⚠️) / 下一步**
