# Warrior RPG

基于 **Unreal Engine 5.6 + C++** 的第三人称动作 RPG 战斗原型。核心是一套用 **Gameplay Ability System（GAS）** 搭建的技能 / 属性 / 战斗管线，配合组件化架构与数据驱动配置。

> **当前阶段**：战斗闭环开发中，同步进行「单机 → Listen Server 双端」的架构改造与性能优化。

---

## 技术栈

| 项 | 内容 |
|---|---|
| 引擎 | Unreal Engine 5.6 |
| 语言 | C++（核心逻辑）+ 蓝图（资源装配与表现层） |
| 关键插件 | `GameplayAbilities`、`EnhancedInput` |
| 模块 | `Warrior`（Runtime，依赖 Engine / EnhancedInput / GameplayAbilities） |
| 平台 | Windows |

---

## 已实现的系统

| 系统 | 说明 | 关键类 |
|---|---|---|
| **能力系统** | 自定义 ASC，按 GameplayTag 绑定输入；GA 三层继承（Base → Hero / Enemy）；自定义 ExecutionCalculation 做伤害结算 | `WarriorAbilitySystemComponent`、`WarriorGameplayAbility`、`GEExecCalc_DamageTaken` |
| **属性系统** | Health / Mana / Range / AttackPower / DefensePower / DamageTaken，成长曲线由 CurveTable 提供 | `WarriorAttributeSet` |
| **战斗组件** | 武器注册、切换、碰撞开关的抽象层，Hero / Enemy 各自实现命中回调 | `PawnCombatComponent`、`HeroCombatComponent`、`EnemyCombatComponent` |
| **武器系统** | 武器为独立 Actor，碰撞盒触发命中事件；伤害数值由 DataAsset + CurveTable 提供 | `WarriorWeaponBase`、`WarriorHeroWeapon` |
| **动画与逻辑解耦** | AnimNotify 发送 GameplayEvent，GA 用 `WaitGameplayEvent` 接收，武器挂载不依赖蓝图时序 | `Notify_SendGameplayEvent` |
| **数据驱动** | StartUpData / InputConfig / WeaponData 三类 DataAsset，软引用 + 异步加载 | `DataAsset_StartUpDataBase`、`DataAsset_InputConfig` |
| **AI** | AIController 配置感知与 TeamId；行为树 + Blackboard + BTService；EQS 自定义节点做侧移选点 | `WarriorAIController`、`BTService_OrientToTargetActor` |
| **UI 解耦** | UIComponent 分层 + 接口隔离，属性变化统一由 AttributeSet 广播 | `PawnUIComponent`、`IPawnUIInterface` |

---

## 架构约定

这些是项目里刻意保持的几条规则，不是随手写的：

- **C++ 定骨架，蓝图填血肉。** 类结构、生命周期、核心流程在 C++；数值、特效、蒙太奇等易变内容放蓝图与 DataAsset。
- **组合优先于继承。** ASC 是挂在角色上的组件，不是角色的基类——它要能被不同 Pawn 复用。
- **接口隔离而非基类继承。** `PawnCombatInterface` / `PawnUIInterface` 让战斗与 UI 不必知道对方的实现。
- **事件驱动而非状态轮询。** 武器挂载走动画事件，不在 Tick 里查状态。
- **配置与流程分离。** 改数值不动代码，改流程不动配置。

---

## 目录结构

```
Warrior/
├─ Source/Warrior/
│  ├─ Public|Private/
│  │  ├─ AbilitySystem/      ASC、AttributeSet、GA 基类、ExecutionCalculation
│  │  ├─ Characters/         玩家与敌人角色
│  │  ├─ Components/         输入 / 战斗 / UI 三类组件（含 PawnExtensionComponentBase 基类）
│  │  ├─ Controllers/        玩家控制器、AIController
│  │  ├─ DataAssets/         StartUpData、InputConfig、武器数据
│  │  ├─ Interfaces/         PawnCombatInterface、PawnUIInterface
│  │  ├─ Items/              武器 Actor
│  │  ├─ AI/                 行为树服务与 EQS 相关
│  │  └─ WarriorTypes/       类型定义、GameplayTags 集中管理
├─ Content/
│  ├─ Assets/ Collections/ Shared/ ThirdPerson/   美术与通用资源
│  ├─ PlayerCharacter/ EnemyCharacter/            角色蓝图与动画
│  ├─ GameModes/ GameplayCues/ Widgets/ Maps/     玩法、表现与关卡
├─ Config/                  Engine / Game / Input / GameplayTags 配置
├─ Docs/                    学习记录与设计文档
└─ PROGRESS.md              开发进度流水
```

---

## 进行中 / Roadmap

- [ ] **补齐战斗闭环**：敌人攻击流程、双键输入、翻滚、技能命中效果、格挡与完美格挡、格挡反击、锁敌与切目标
- [ ] **远程敌人与 Boss**：投射物、远程行为树分支、多阶段 Boss 状态机
- [ ] **单机 → Listen Server 双端**：技能激活的服务器权威化、属性复制、武器生成同步、UI 一致性
- [ ] **性能优化**：Insights 抓帧定位瓶颈，产出可量化的优化对比
- [ ] **架构重构**：连击状态从 GA 成员变量迁到可复制属性，流程从蓝图收进 C++ 基类

---

## 文档

| 路径 | 内容 |
|---|---|
| `Docs/Guide/` | 记录与学习的规则 |
| `Docs/Journal/` | 每日笔记：概念 → 代码对应 → 验证方式 → 档位 |
| `Docs/DesignDecisions.md` | 设计判断台账：每个设计决定的取舍与理由 |
| `Docs/Bugs/` | 踩坑记录：现象、误判、根因、复现方式 |
| `PROGRESS.md` | 教程与开发进度流水 |

---

## 如何运行

1. 安装 **Unreal Engine 5.6**
2. 右键 `Warrior.uproject` → **Generate Visual Studio project files**
3. 用 **Visual Studio 2022**（需安装「使用 C++ 的游戏开发」工作负载）打开生成的解决方案并编译
4. 编译通过后双击 `Warrior.uproject` 启动编辑器，地图见 `Content/Maps/`

---

## 项目背景

本项目以「第三人称 ARPG 战斗系统」为目标，前期参照公开教程搭建骨架，**其后的架构改造、多人同步与性能优化为独立完成**。文档目录中的设计判断与踩坑记录，用于沉淀每一步的取舍理由，而不只是记录做了什么。
