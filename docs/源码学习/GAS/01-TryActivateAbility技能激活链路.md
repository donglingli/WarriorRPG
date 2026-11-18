# GAS — TryActivateAbility 技能激活链路

> 日期：2026.11.17
> 阅读目标：从项目入口 `TryActivateAbility`（Day3 装备技能激活处调用）追到 `ActivateAbility`，理解激活全链路与网络处理。
> 引擎源码位置：`Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp`

## 链路梳理

```
TryActivateAbility(Handle, bAllowRemoteActivation)
  └─ 按 Handle 在 ActivatableAbilities.Items 中找到 Spec → 委托给 ↓
InternalTryActivateAbility
  ├─ 重置失败 Tag：InternalTryActivateAbilityFailureTags.Reset()
  ├─ ABILITYLIST_SCOPE_LOCK()：锁定技能列表，防止激活期间 Spec 被移除
  ├─ 检查 Spec 有效性 / PendingRemove / Ability / ActorInfo
  ├─ 检查 NetMode 与 GetNetExecutionPolicy() 是否匹配（ServerOnly / LocalPredicted 等）
  ├─ UGameplayAbility::CanActivateAbility
  │    ├─ 激活是否被抑制（suppressed）
  │    ├─ DoesAbilitySatisfyTagRequirements：ActivationOwnedTags / ActivationBlockedTags（技能互斥）
  │    ├─ CheckCooldown：本质检查 cooldown GE 的 tag 是否精确匹配（只检查，不应用）
  │    └─ CheckCost：本质检查 CostGE 能否应用（只检查，不应用）
  ├─ 按实例化类型处理：
  │    ├─ InstancedPerExecution：每次激活复制新实例
  │    ├─ InstancedPerActor：复用已有实例（Spec.GetPrimaryInstance()）
  │    └─ NonInstanced：直接用 CDO，无状态
  ├─ 权威端无预测键时用 FScopedPredictionWindow 生成，写入 ActivationInfo.GenerationPredictionKey
  └─ CallActivateAbility
       ├─ PreActivate：补完激活前置逻辑（加 Tag、Notify 激活、应用 block/cancel 配置 Tag、Spec 活跃计数）
       └─ ActivateAbility：进入蓝图/技能本体逻辑（CurrentActorInfo 等在此存到实例上）
MarkAbilitySpecDirty → Spec 状态复制
记录激活时间 → return true
```

## 修正与补充

1. **分层归属**：Spec 有效性 / PendingRemove / NetMode 等检查都在 `InternalTryActivateAbility` 里；`TryActivateAbility` 本身职责很薄——找 Spec、委托、失败时走 `AbilityFailedCallbacks` 失败回调链（这也是 `InternalTryActivateAbilityFailureTags.Reset()` 的呼应点：失败 Tags 在失败路径填充并广播）。
2. **CanActivateAbility 补全**：除了抑制/冷却/消耗，还有 `DoesAbilitySatisfyTagRequirements`（ActivationOwnedTags / ActivationBlockedTags，技能互斥的实现处）和 `BlockedAbilityTags`。
3. **检查与提交分离（GAS 核心模式）**：激活阶段对 Cost / Cooldown 只做**检查**，真正的应用在 **`CommitAbility` → ApplyCost / ApplyCooldown**。激活点可以被预测端提前调用，扣费必须等到确认要跑才发生。
4. **MarkAbilitySpecDirty 有两条独立复制通道**：
   - Spec dirty → 复制给非拥有端（simulated proxy 看到的技能状态）
   - 拥有端客户端（autonomous proxy）的激活确认**不走** Spec 复制，走 PredictionKey + `GenericConfirm/GenericReject` 复制事件
5. **CallActivateAbility 补充**：还负责把 `CurrentActorInfo / CurrentSpecHandle / CurrentActivationInfo` 存到实例上（这是 `ActivateAbility` 里能直接用 `CurrentActorInfo` 的原因）。

## 待确认

- ⚠️ "记录 AbilityLastActivatedTime"：该字段的确切声明位置和用途尚未核对源码，待回头确认。

## 项目关联

- 本链路入口对应项目 Day3 的 `HeroInputComponent` 输入绑定 → `TryActivateAbility` 激活技能
- Day4 的"技能随武器生命周期管理"（GrantWarriorAbility 返回 SpecHandle 保存到武器）依赖本文的 Spec / Handle 概念

## 下一步追读计划（按序）

1. `CommitAbility`：Cost / Cooldown 真正的应用点（+ `CommitCheck`）
2. 失败路径：`CanActivateAbility` 返回 false 后的失败枚举与 `AbilityFailedCallbacks` 订阅方
3. `EndAbility` / `CancelAbility`：与"技能随武器生命周期管理"（Day4）直接相关
4. `FScopedPredictionWindow`：预测窗口的 begin/end 语义，预测+确认+回滚三段式
