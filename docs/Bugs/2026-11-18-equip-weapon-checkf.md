# Bug · 装备武器时 checkf 触发（双端环境）

**发现日期**：2026-11-18
**状态**：⏸ 挂起，计划 Week 5 网络改造时统一修
**触发条件**：PIE 用 Listen Server + 2 客户端时，装备武器阶段触发 checkf 断言
**单机表现**：不触发

## 为什么挂起

不是懒。修改它需要先回答"双端下哪条路径多跑了一次"，而这个答案要等网络改造把
权威/复制的概念铺开之后才看得懂。现在硬修只能靠试错，修好了也不知道为什么好。

**Week 5 Day 29-30 修它。** 在那之前它不影响 Week 1-4 的任何安排（单机照常跑）。

## 待定位（Week 5 开工第一件事）

装备路径上共 4 个可能触发的断言，靠 Call Stack 的调用点区分：

| 文件 | 断言消息 | 触发条件 |
|---|---|---|
| `Components/Combat/PawnCombatComponent.cpp:12` | `a tag has registed: %s` | 同一 tag 注册两次 |
| `Items/Weapons/WarriorWeaponBase.cpp:30 / 46` | `forgot to assgin owner, weapon: %s` | 武器 Instigator 为空时碰到目标 |
| `AbilitySystem/WarriorAttributeSet.cpp:35` | `forgot implement IPawnUIInterface` | GE 执行时目标 Avatar 未实现 UI 接口 |
| `DataAssets/StartUpData/DataAsset_StartUpDataBase.cpp:11` | （无消息，`check`） | ASC 为空时初始化启动数据 |

## 已确认的线索

- `Source` 里**没有任何 `SpawnActor` 调用**，而 `RegisterSpawnedWeapon` 是 `BlueprintCallable`
  → **武器在蓝图里生成**，那个 SpawnActor 节点大概率没有 `Authority` 判断。
- 单机只有一个世界，双端有两个世界 + `PossessedBy` / `OnRep_PlayerState` 两条初始化路径。

## 第一定律（Week 5 会反复用到）

> 单机代码改成多人后，90% 的 bug 不是"逻辑错了"，是"同一段代码在多端/多路径各跑了一次"。

三种形态：
1. **双世界执行**：服务器世界一份、客户端世界一份
2. **双路径执行**：`PossessedBy`（服务器）+ `OnRep_PlayerState`（客户端），服务器上可能两条都走
3. **顺序错乱**：客户端 `BeginPlay` 早于关键数据复制到达

## 修完之后回来补

- 根因：
- 改法：
- 如果重来，怎么更快定位：
- 面试怎么讲（一句话）：
