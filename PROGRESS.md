# 项目进度记录

> 项目：Warrior（UE 5.6）
> 记录方式：每个开发日一条记录，当天结束时更新。
> 字段说明：产出 = 当天完成的内容；卡点 = 遇到的问题；解决 = 处理方式；关键记录 = 重要决定/经验，避免遗忘；个人总结 = 反思与改进方向。

---

## 进度总览

| 第几日 | 日期 | 今日产出 | 遇到的卡点 | 如何解决的 | 关键记录 | 个人总结 |
| :---: | :--- | :--- | :--- | :--- | :--- | :--- |
| Day1 | 2026.11.12 | 创建项目，增加测试场景，创建基础类，调试工具类；设置玩家相机，添加原生输入标签 | 无 | - | - | - |
| Day2 | 2026.11.13 | 自定义增强输入组件，绑定玩家输入 | 1. `UWarriorInputComponent::BindNativeInputAction` 参数填错<br>2. 绑定 Look 后运行无反应 | 1. 核对参数签名后修正<br>2. 查看 `IMC_Default`，将 GamePad XY 修改为 Mouse XY | - | 代码编写还是不熟，写得慢，频繁回头看已有的写法 |
| Day3 | 2026.11.14 | 玩家基础内容搭建完成（6/6）+ 晚间追加近战系统前置：Pawn 战斗组件扩展、武器注册（TMap Tag↔武器）、装卸装备输入绑定与技能授予（HeroStartUpAbilitySets）、装备 Montage + AnimNotify/GameplayEvent 挂载右手、ALI_Hero 动画层接口 | 1. `BindAction` C2665 回调签名不匹配<br>2. 装备 Montage 播一半就停 | 1. 回调参数改为 `const FGameplayTag&`<br>2. 移除误连到 EndAbility 的 Blend In 事件 | 详见当日记录：TWeakObjectPtr、软引用加载、AnimNotify+GameplayEvent 原理、FGameplayAbilitySpec | - |
| Day4 | 2026.11.15 | 近战系统完成（4/4）：链接动画层（持械奔跑切换）、装卸装备闭环（SpecHandle 技能生命周期）、连击系统（轻/重连击 + 轻攻击跳终结技）、轻重攻击 + 慢动作 + 全 Montage 音效；新增 WarriorFunctionLibrary。追加：敌人系统地基（Enemy 基类/蓝图动画/异步加载启动数据/GA 生成武器/属性集 ATTRIBUTE_ACCESSORS/启动 GameplayEffect + CurveTable） | 装备武器后仍无法切换持械奔跑 | 逐项排查（资产配置/LinkedAnimLayer/BlendSpace 均正常），根因：MasterAnimLayer_Hero 未实现 ALI_Hero 接口 | 链接层排查四步法；标准化技能实现流程（7 步）；技能随武器生命周期管理；敌人启动数据用 RequestAsyncLoad 异步加载（对照英雄的同步加载） | - |
| Day5 | 2026.11.16 | 攻击→碰撞→伤害→扣血链路打通（PawnCombatInterface、ToggleWeaponCollision、武器 Overlap 委托、MeeleHit 事件、ANS_ToggleWeaponCollision、GEExecCalc_DamageTaken、PostGameplayEffectExecute 扣血）；追加：受击反馈（HitReact GA + 材质参数）、攻击暂停（时间膨胀）、受击音效并发控制、GameplayCue 击中音效、死亡链路（血量归零打 Tag → GA_Death → 蓝图接口处理消融/粒子/销毁 + Niagara） | - | - | 触发器 GA 用 ActivateAbilityFromEvent（非 EventOnBeginAbility）；死亡链路逻辑与表现解耦；音效分类（AnimNotify+并发 / GameplayCue） | - |
| Day6 | 2026.11.17 | UI 系统完成（UIComponent/接口/委托/模板/绑定 + 受击头顶血条联动验证）；追加敌人 AI 基础：WarriorAIController（C++ 配置感知/TeamId/行为树）、AIC 蓝图层级、BB_Enemy_Base、BT_Guardian（距离判断 + 朝向/移动分支）、BTService 双实现（蓝图算距离 / C++ 改朝向） | - | - | AttributeSet 广播委托 → UIComponent → Widget，属性数据与 UI 表现解耦；AI 配置全部在 C++ 完成，蓝图只留子类差异化 | - |

---

## 每日详细记录

### Day1 — 2026.11.12

- **今日产出**：
  - 创建项目，增加测试场景
  - 创建基础类，调试工具类
  - 设置玩家相机，添加原生输入标签
- **遇到的卡点**：无
- **如何解决的**：-
- **关键记录**：-
- **个人总结**：-

### Day2 — 2026.11.13

- **今日产出**：
  - 自定义增强输入组件（`UWarriorInputComponent`）
  - 绑定玩家输入
- **遇到的卡点**：
  1. 创建 `UWarriorInputComponent::BindNativeInputAction` 时参数填错
  2. 绑定 Look 后运行无反应
- **如何解决的**：
  1. 修正参数
  2. 查看 `IMC_Default`，将 GamePad XY 修改为 Mouse XY
- **关键记录**：-
- **个人总结**：代码编写还是不熟，写得慢！频繁看怎么写的。

### Day3 — 2026.11.14（已完成，6/6 项全部搞定）

**今日计划**：搭建完玩家基础内容，包含以下 6 项：

1. 动画和链接层（Animation Layers / Linked Anim Layers）
2. 使用 GAS 作为能力系统（ASC / AttributeSet / GameplayAbility）
3. 技能激活策略（Ability Activate Policy）
4. 设置武器类别（Weapon Type）
5. 英雄武器（Hero Weapon）
6. 启动数据和同步加载（Startup Data / Sync Loading）

- **今日产出**：
  - 动画和链接层：✅ 已完成
    - 增加 idle / jog 状态切换，idle / relax 状态切换（relax 状态在 C++ 处理）
    - 编写 AnimInstance 基础结构：`WarriorBaseAnimInstance` → `WarriorCharacterAnimInstance`（继承 Base）→ `WarriorHeroAnimInstance`（继承 Character）；`WarriorHeroLinkedAnimInstance`（继承 Base）
    - 新增蓝图资产：`BS_UnarmedLocomotion`、`ABP_Hero`
  - 接入 GAS 能力系统：✅ 已完成
    - 新建 `WarriorAbilitySystemComponent`、`WarriorAttributeSet`，并在 `WarriorBaseCharacter` 中创建并持有 AbilityComponent
    - 新建 `WarriorGameplayAbility`，实现技能赋予策略
  - 技能激活策略：✅ 已完成
    - 新增枚举 `EWarriorAbilityActivationPolicy`（`OnTriggerd` / `OnGiven`），重写 `OnGiveAbility` 和 `EndAbility`
    - 简单实现 `OnGiven` 类型的 `TryActivateAbility` 和 `ClearAbility` 调用
    - 创建蓝图类（Shared → GameplayAbility）：`GA_Shared_SpawnWeapon`，继承 `WarriorGameplayAbility`，用于给玩家和敌人生成武器
  - 武器类别：✅ 已完成
    - 新建 `WarriorWeaponBase`（继承 `AActor`）、`WarriorHeroWeapon`（继承 Base）
    - `WeaponBase` 增加 `StaticMeshComponent` 和 `BoxComponent`（作为碰撞盒），并暂时关闭碰撞
    - 蓝图：`BP_HeroWeaponBase`（继承 `WarriorHeroWeapon`）→ `BP_HeroAxe`（继承 `BP_HeroWeaponBase`），设置 Mesh 并调整碰撞盒
  - 英雄武器：✅ 已完成
    - 修改玩家 SkeletalMesh，给骨骼 `spine_02` 新增挂点 `AxeBackSocket`，用于挂载后背武器
    - `GA_Shared_SpawnWeapon` 蓝图节点流程：Event ActivateAbility → SpawnActorFromClass → IsValid → AttachActorToComponent → EndAbility；创建子蓝图 `GA_Hero_SpawnAxe`（放入 Character 目录）
    - 运行游戏，成功看见玩家装备武器到后背
  - 启动数据和同步加载：✅ 已完成
    - 新增 DataAsset 类：`DataAsset_StartUpDataBase`（Base）、`DataAsset_HeroStartUpData`（继承 Base）
    - Base 中声明两个 `TArray<TSubclassOf<UWarriorGameplayAbility>>` 变量：`ActiavteOnGivenAbilities`、`ReactiveAbilities`；成员函数 `GiveToAbilitySystemComponent` 内部调用 `GrantAbility`
    - 由 `DataAsset_HeroStartUpData` 创建 DataAsset 蓝图 `DA_Hero`，将 `GA_Hero_SpawnAxe` 放入，作为启动数据
    - `WarriorBaseCharacter.h` 声明启动数据软引用 `TSoftObjectPtr<UDataAsset_StartUpDataBase> CharacterStartUpData`
    - 在子类 `WarriorHeroCharacter::PossessedBy` 中同步加载 `CharacterStartUpData.LoadSynchronous`，加载成功后调用 `LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent)` 授予技能
- **遇到的卡点**：
  1. `GetVelocity()` 是 Character 的方法，非 MovementComponent 的方法，一开始误用 MovementComponent 调用
  2. 使用 `BlueprintReadOnly` 时属性必须是 Protected / Public 类型，Private 类型在蓝图中不可见
  3. 获取 CharacterMovementComponent：`Character->GetCharacterMovement()` 返回正确类型，而 `GetMovementComponent()` 返回的是 `UPawnMovementComponent*`
- **如何解决的**：
  1. 改为在 Character 上调用 `GetVelocity()`
  2. 将需要暴露给蓝图的属性改为 Protected
  3. 使用 `Character->GetCharacterMovement()` 获取 `UCharacterMovementComponent*`
- **关键记录**：
  - `NativeThreadSafeUpdateAnimation` 运行在工作线程（非游戏线程），动画有性能问题时才考虑使用
  - GAS 接入要点：
    - 在构造函数中 `CreateDefaultSubobject` 创建 AbilitySystemComponent 和 AttributeSet
    - 在 `PossessedBy` 中检查 AbilitySystemComponent 后调用 `InitAbilityActorInfo(this, this)`
    - 继承 `IAbilitySystemInterface` 并重写纯虚函数 `GetAbilitySystemComponent`
    - Spec = 技能规格说明（GameplayAbilitySpec）
  - 技能策略链路：GA → Give Ability to BaseCharacter → Activate Ability → OnGiven 一次性 / OnTriggered → EndAbility（ClearAbility）
  - 软引用两种加载方式：同步加载（`LoadSynchronous`）与异步加载（`LoadAsynchronous`）
    - 启动数据适合同步加载：数据量小；若异步加载，可能先进入游戏看见玩家，过一两秒才看见装备出现
    - 异步加载适合大量敌人的加载方式：后台加载不占用游戏进程
  - `AbilitySystem->GiveAbility` 的参数 `FGameplayAbilitySpec`：新建时参数传入 GA，需要对 `SourceObject`、`Level` 赋值

**晚间追加产出（近战系统前置部分，提前完成 Day4 的一部分）**：

1. **Pawn 组件扩展**：结构 `PawnExtensionComponentBase` → `PawnCombatComponent` → `HeroCombatComponent` / `EnemyCombatComponent`；`PawnExtensionComponentBase` 负责提供辅助函数；`WarriorHeroCharacter` 持有 `HeroCombatComponent` 成员变量
2. **注册生成的武器**：
   - `PawnCombatComponent` 用 `TMap` 保存 Tag ↔ `WarriorWeaponBase` 指针；`RegisterSpawnedWeapon` 注册已装备武器、`GetWarriorCarriedWeaponByTag` 获取指定 Tag 武器、`GetCurrentEquippedWeapon` 获取当前装备武器
   - `WarriorGameplayAbility` 声明 `FORCEINLINE` 方法 `GetPawnCombatComponentByActorInfo` 获取 PawnCombatComponent；`HeroGameplayAbility` 中使用 `TWeakObjectPtr` 保存 WarriorCharacter 引用
3. **装卸装备能力输入绑定**：
   - `DataAsset_InputActionConfig` 增加数组 `AbilityInputActions`
   - `HeroInputComponent` 绑定输入，实现模板函数 `UWarriorInputComponent::BindAbilityInputAction`
   - 新增 InputAction：`IA_EquipAxe`；蓝图 `DA_InputActionConfig.AbilityInputActions` 添加数据（InputActionTag = `InputTag.EquipAxe`，InputAction = `IA_EquipAxe`）
4. **将"装备斧头"技能设置给玩家**：
   - `DataAsset_HeroStartUpData` 新增结构体 `FWarriorHeroAbilitySet`（`FGameplayTag InputTag` + `TSubclassOf<UWarriorGameplayAbility> AbilityToGrant`），新增数组 `TArray<FWarriorHeroAbilitySet> HeroStartUpAbilitySets`
   - 覆写父类方法 `GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InWarriorASCToGive, int32 ApplyLevel = 1)`
   - 蓝图 `DA_Hero` 为 `HeroStartUpAbilitySets` 添加元素；进入游戏后按 `'` 查看角色身上技能情况
5. **角色绑定 Action 输入并调用 ASC 激活技能**：通过 `GetActivatableAbilities` 获取可激活技能（返回 `FGameplayAbilitySpec`），检查通过后调用 `TryActivateAbility`
6. **播放武器装备动画**：
   - `GA_Hero_EquipAxe` 使用 `PlayMontageAndWait` 节点
   - 创建 Montage：`AM_Hero_Axe_Equip`，新建插槽 `UpperBodySlot`；用 `SaveCachedPose` 保存 Locomotion 姿势，缓存的 Locomotion 连接 Layered Blend per bone（分层骨骼为 `spine_01`）
7. **播放动画的同时完成武器挂载右手**：
   - `WarriorGameplayTag` 新增两个 Tag：`Player.Event.Equip.Axe` / `Player.Event.Unequip.Axe`
   - 新建 AnimNotify 蓝图 `Notify_SendGameplayEvent`（调用 `SendGameplayEventToActor`）
   - `AM_Hero_Axe_Equip` 中添加 `Notify_SendGameplayEvent` 并设置 Tag
   - 骨骼 `hand_r` 右手添加插槽 `AxeRightHandSocket` 并调整位置和旋转
   - `GA_Hero_EquipAxe` 中 `WaitGameplayEvent` 等待 `Player.Event.Equip.Axe`，事件到达后用 `AttachActorToComponent` 将斧头挂载到右手（连接 `EventReceive` 引脚）
8. **切换持械移动状态，使用动画层接口**：创建动画层接口 `ALI_Hero`，在 `ABP_Hero` 中使用；目前关卡中切换后变成 TPose，链接层动画放到明天完善

**追加卡点与解决**：
- 卡点 1（输入绑定 C2665）：覆写输入绑定时调用 `BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Started, ContextObject, InputPressedFunc, AbilityInputActionConfig.InputTag)` 编译报错 C2665（没有重载函数可以转换所有参数类型）
  - 排查 1：`InputPressedFunc` 参数少了 `const FInputActionValue& Value`，补全后依旧报错
  - 排查 2：回调参数类型应为 `FGameplayTag InputTag` 或 `const FGameplayTag& InputTag`，写成 `FGameplayTag& InputTag`，修改后编译通过
  - **原理**：EnhancedInput 的 `BindAction` 是模板绑定，要求回调函数签名与内部委托签名**完全一致**；非 const 引用参数（`FGameplayTag&`）在委托语义中会被视为"输出参数"，与期望的只读输入参数签名不匹配，模板推导失败，于是报"没有可匹配的重载"。而 `const FGameplayTag&` 是只读引用，签名兼容，故可通过
- 卡点 2（装备动画只播放一半）：按 1 触发装备动画只播放一半就停
  - 排查：检查 `PlayMontageAndWait` 到 `EndAbility` 的连线，发现多连接了 Blend In，导致提前 EndAbility；移除后正常
  - **原理**：`PlayMontageAndWait` 是异步等待节点，有多个在不同时机触发的完成引脚（如 OnCompleted / OnBlendOut / OnInterrupted 等，Blend In 相关事件触发最早）；`EndAbility` 会终止能力并停止正在播放的 Montage。把触发时机较早的事件连到 `EndAbility`，能力在动画未播完时就提前结束，表现为"动画只播放一半"。只有 `OnCompleted`（按需选择）才应连接 `EndAbility`
- **原理补充（AnimNotify + GameplayEvent 挂载机制）**：动画驱动的逻辑解耦——Montage 播放到特定帧时由动画系统触发 AnimNotify，`Notify_SendGameplayEvent` 通过 `SendGameplayEventToActor` 向角色的 ASC 发送带 Tag 的事件；GA 侧用 `WaitGameplayEvent` 异步监听该 Tag，事件到达才执行 `AttachActorToComponent` 挂载。好处：① 挂载时机与动画帧精确同步（挂载点由动画师在时间轴上控制）；② GA 逻辑与动画资产解耦（同一个 GA 可配合不同 Montage）；③ GameplayEvent 走 ASC 事件通道，天然兼容网络同步

**追加关键记录**：
- `TWeakObjectPtr`：弱指针，用于保存 WarriorCharacter 引用，不阻止垃圾回收、不增加引用计数，仅持有引用（适合组件互相引用，避免循环引用导致的内存泄漏）

- **个人总结**：-

### Day4 — 2026.11.15（已完成，近战系统 4/4）

**今日计划**：近战系统四块（装卸装备 / 连击系统 / 完善链接动画层 / 轻重攻击和音效）

- **今日产出**：
  - 完善动画链接层，成功切换到持械奔跑：✅ 已完成
    - 创建 `ABP_MasterAnimLayer_Hero`，继承 `WarriorHeroLinkerAnimLayer`；AnimGraph 中用 BlendSpacePlayer 节点连接到 OutPose
    - 创建 `ABP_AnimLayer_HeroAxe`，继承 `MasterAnimLayer_Hero`
    - 创建 `WarriorStructType`，定义结构体 `FWarriorHeroWeaponData`，存放 `TSubclassOf<UWarriorHeroLinkerAnimLayer> WarriorAnimLayerToLink`
    - `WarriorHeroWeapon` 新增成员变量 `FWarriorHeroWeaponData HeroWeaponData`
    - `HeroCombatComponent` 新增成员函数 `AWarriorHeroWeapon* GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const`
    - `GA_Hero_EquipAxe` 中调用 `GetHeroCarriedWeaponByTag` 获取武器，再使用 `SkeletalMeshComponent` 的 LinkAnimClassLayers（AnimLinkByClass 节点）链接动画层
  - 完善装备装卸功能：✅ 已完成
    - 新增 Tag：`Player.Ability.Equip.Axe` / `Player.Ability.Unequip.Axe`
    - `FWarriorHeroWeaponData` 新增成员：`UInputMappingContext* WeaponInputMappingContext`、`TArray<FWarriorHeroAbilitySet> DefaultWeaponAbilities`；并添加到 `GA_Hero_EquipAxe` 和 `GA_Hero_UnequipAxe` 中
    - 从 `Hero_UnEquip_Axe` 派生 Montage，修改 Slot 和 Notify
    - 创建 `GA_Hero_UnequipAxe`：添加 `PlayMontageAndWait`、`WaitGameplayEvent` 节点
    - `GA_Hero_EquipAxe` 蓝图新增函数 `HandleEquipWeapon(UWarriorHeroWeapon* InWeaponToEquip)`：添加 WeaponInputMappingContext、调用 `GrantWarriorAbility` 并保存返回的 GameplayAbilitySpecHandle（保存到 `WarriorHeroWeapon` 中）
    - `GA_Hero_UnequipAxe`：取消动画链接层、移除 InputMappingContext、移除 GrantAbility（用 SpecHandle）、重置 EquipWeaponTag
    - 验证：可正确取消装备武器并过渡到空手移动动画
  - 连击系统：✅ 已完成
    - 通过 GameplayAbility 蓝图类实现：轻攻击连击、重攻击连击、轻攻击跳转终结技（重攻击最后一段）
    - 编写函数库 `WarriorFunctionLibrary`，提供设置和查询 GameplayTag 的接口
  - 轻重攻击和音效：✅ 已完成
    - 轻重攻击最后一段增加 AnimNotifyState 实现慢动作效果
    - 所有轻重攻击、装卸装备 Montage 均添加音效（AnimNotify - Play Sound）
- **遇到的卡点**：
  1. 上述操作后，装备武器依然无法切换到持械奔跑动画
- **如何解决的**（排查过程）：
  1. 检查 AnimLayer 是否正确设置到 `BP_HeroAxe` 的成员变量中 → 确认无误
  2. 检查 `ABP_Hero` 是否使用 LinkedAnimLayer 节点 → 确认无误
  3. 检查 `AnimLayer_Hero` 是否挂载 BlendSpace → 确认无误
  4. 检查 `MasterAnimLayer_Hero` 是否实现 AnimLayerInterface 接口 → **未实现（根因）**
  - **根因**：`MasterAnimLayer_Hero` 未实现 AnimLayerInterface（`ALI_Hero`）接口，导致节点连线做在了主 AnimGraph 中；实现接口后，应双击 State 变量、在 State 内部做动画混合。修改后切换正常
- **关键记录**：
  - 技能随武器生命周期管理：装备武器时 `GrantWarriorAbility` 返回 `GameplayAbilitySpecHandle` 保存到 `WarriorHeroWeapon`，卸下武器时用该 SpecHandle 精确移除对应技能；配合 `DefaultWeaponAbilities` 让武器数据自带默认技能与 IMC，实现"武器即配置"的数据驱动设计
  - 标准化玩家技能实现流程（7 步）：
    1. 添加 AbilityTag（GameplayTag）
    2. 添加新的按键输入并添加到配置
    3. 创建 GA 蓝图并继承 `WarriorHeroGameplayAbility`
    4. 创建 Montage
    5. 蓝图中添加 Ability InputAction 数据内容
    6. GiveAbility（无需关心，上述配置添加后自动生效）
    7. 在 GA 蓝图中实现输入到动画播放的节点逻辑
  - `WarriorFunctionLibrary` 函数库：集中提供 GameplayTag 的设置/查询接口，避免各处散落查询逻辑

**追加产出（敌人系统地基）**：

1. **敌人所需基类**：
   - `WarriorEnemyCharacter`、`EnemyCombatComponent`、`DataAsset_EnemyStartUpData`、`WarriorEnemyWeapon`、`WarriorEnemyGameplayAbility`
   - 重写 `WarriorEnemyCharacter()` 构造函数
   - `WarriorEnemyGameplayAbility` 增加辅助函数：`AWarriorEnemyCharacter* GetEnemyCharacterFromActorInfo()`、`UEnemyCombatComponent* GetEnemyCombatComponentFromActorInfo()`
2. **敌人蓝图与动画**：
   - `BP_EnemyCharacter_Base`（继承 `WarriorEnemyCharacter`）→ `BP_Grunting_Base`（继承 Base）→ `BP_Grunting_Guardian`（继承 Grunting_Base）
   - 新建模板 `ABP_Enemy_Base`，新建 `ABP_Guardian`（继承 Base）并实现动画绑定
3. **异步加载敌人启动数据**：重写 `WarriorEnemyCharacter::PossessedBy`，使用 `UAssetManager::GetStreamableManager().RequestAsyncLoad` 动态加载 Enemy 启动数据
4. **敌人武器生成**：创建 `GA_Grunting_SpawnWeapon`（继承 `GA_Shared_SpawnWeapon`）用于给 Enemy 生成武器，填入所需绑定配置，成功添加武器
5. **属性集**：在 `UWarriorAttributeSet` 定义属性集，使用 `#define ATTRIBUTE_ACCESSORS` 宏生成访问器，构造函数中初始化属性值
6. **启动 GameplayEffect**：
   - `UDataAsset_StartUpDataBase` 新增数组 `TArray<TSubclassOf<UGameplayEffect>> StartUpGameplayEffects`，在 `GiveToAbilitySystemComponent` 中调用 `ApplyGameplayEffectsToSelf`
   - 创建对应 Effect 和 CurveTable：ModifierOp 均使用 Override；Max 属性使用 `FScalableFloat` 绑定 CurveTable；`GE_XXX_Static` 用于 Current 属性；AttributeBased 并用对应的 Max 属性
   - 填入绑定配置，成功添加 GameplayEffect
- **个人总结**：-

### Day5 — 2026.11.16（已完成：攻击伤害链路 + 受击死亡闭环）

**今日产出**：攻击 → 碰撞 → 伤害计算 → 扣血链路已打通 ✅

1. **武器碰撞盒开启/关闭切换**：
   - 创建接口类 `PawnCombatInterface`，定义纯虚函数 `virtual UPawnCombatComponent* GetPawnCombatComponent() const = 0`；`CharacterBase` 继承该接口，并和其子类实现该接口
   - `UWarriorFunctionLibrary` 新增原生函数 `NativeGetPawnCombatComponentFromActor` 和蓝图函数 `BP_GetPawnCombatComponentFromActor`
   - `PawnCombatComponent` 新增函数 `ToggleWeaponCollision`，切换武器碰撞盒 QueryOnly / NoCollision
2. **武器命中委托**：
   - `WeaponBase` 注册 Overlap 的 Begin/End 委托回调 `OnCollisionBoxBeginOverlap`、`OnCollisionBoxEndOverlap`
   - `WeaponBase` 中创建单参数委托 `DECLARE_DELEGATE_OneParam(FOnTargetInteractedDelegate, AActor*)`，声明 `OnWeaponHitTarget`、`OnWeaponPulledFromTarget`，调用 `ExecuteIfBound`
   - `PawnCombatComponent` 注册并绑定这两个委托的回调函数，子类中实现回调逻辑
3. **命中事件通知**：GameplayTag 新增 `Shared_Event_MeeleHit`；回调函数中调用 `UAbilitySystemBlueprintLibrary::SendGameplayEventToActor` 发送命中事件；轻重攻击 GA 蓝图添加 `WaitGameplayEvent` 节点处理并打印日志验证流程
4. **碰撞切换通知**：创建 AnimNotifyState 蓝图 `ANS_ToggleWeaponCollision` 处理武器碰撞切换；对所有轻重攻击 Montage 均添加该通知
5. **ApplyDamage 流程**：
   - GA 触发命中后，`MakeGameplayEffectSpecHandle` 生成效果规范句柄：构造 MakeOutGoingSpec 实例，填充 `SetSetByCallerMagnitude`
   - `WarriorGameplayAbility` 编写 `ApplyEffectSpecHandleToTarget` 实现（调用 `ApplyGameplayEffectSpecToTarget`，返回 `FActiveGameplayEffectHandle`）；攻击蓝图中调用并连接
6. **伤害计算（ExecCalc）**：
   - 创建 `GEExecCalc_DamageTaken : public UGameplayEffectExecutionCalculation`，构造函数中捕获 `AttackPower`、`DefensePower`
   - 重写 `Execute_Implementation`：获取相关属性值，计算伤害，添加到 Modifier（DamageTaken）
7. **扣血**：`UWarriorAttributeSet` 重写 `PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)`，获取伤害值进行血量扣除

**追加产出（命中反馈 / 死亡处理 / 死亡效果）**：

实现链路沿用标准化流程：1.添加 AbilityTag → 2.Montage → 3.GameplayAbility Blueprint

1. **受击反馈**：
   - 创建受击反馈 GA：`GA_Enemy_HitReact_Base`，连接 `PlayMontageAndWait` 等节点实现受击动画播放
   - 修改材质并创建材质实例
   - GA 中添加材质参数节点 `SkeletalMeshComponent->SetScalarParameterValueOnMaterial`，填入 ParameterName / ParameterValue 显示受击反馈效果
2. **攻击暂停（时间膨胀）**：作为玩家技能授予给玩家，通过 GameplayEvent 触发，使用时间膨胀（Time Dilation）实现
3. **受击音效**：创建音效并发（Concurrency）资产，敌人受击音效最大并发播放数量设置为 1
4. **武器击中音效**：使用 GameplayCue 处理，GA 中调用 `ExecuteGameplayCueToOwner`
5. **死亡能力**：
   - 添加死亡 Tag；在 AttributeSet 数值写入时（`PostGameplayEffectExecute`）判断当前血量是否为 0，为 0 则调用函数库 `UWarriorFunctionLibrary::AddGameplayTagToActorIfNone` 添加死亡标签
   - 创建 `GA_Death`，填写对应触发标签，打印死亡日志验证生效；将 `GA_Death` 添加到敌人启动数据中
6. **死亡角色处理与效果**：
   - 使用蓝图接口实现死亡后角色处理：暂停动画、关闭碰撞、材质消融、粒子播放、销毁角色；在 `EnemyBase`（BP）中实现该接口
   - `GA_Death` 在 `EventOnEndAbility` 时调用该蓝图接口函数，同时处理武器材质消融和销毁
   - 添加 Niagara 附加到 Mesh，设置 Niagara 参数使粒子颜色与角色材质消融颜色一致

- **遇到的卡点**：-
- **如何解决的**：-
- **关键记录**：
  - **触发器 GA 不能使用 `Event OnBeginAbility` 节点，应使用 `ActivateAbilityFromEvent` 节点**（由 Tag 触发的 GA，事件数据通过 ActivateAbilityFromEvent 传入）
  - 死亡链路设计：血量归零 → Attribute 层打死亡 Tag → Tag 触发 `GA_Death` → EndAbility 时通过蓝图接口处理表现层（动画/碰撞/消融/粒子/销毁），逻辑与表现解耦
  - 音效分类处理：角色级音效用 Montage 上的 AnimNotify + 并发资产控制（最大并发 1）；武器击中音效用 GameplayCue（`ExecuteGameplayCueToOwner`），走 GAS 网络同步通道
- **个人总结**：-

### Day6 — 2026.11.17（已完成）

**今日产出**（UI 系统）：

1. **UI 组件结构**：
   - 创建 `PawnUIComponent` → `HeroUIComponent` / `EnemyUIComponent`
   - 创建 `PawnUIInterface` 接口，提供辅助函数获取 UIComponent
2. **属性变化通知机制**：
   - UIComponent 添加 `DYNAMIC_MULTICAST` 委托，供 AttributeSet 在数值写入时调用，通知 UI 更新（解耦：AttributeSet 只广播，不直接操作 UI）
3. **UI 模板（Widget 蓝图）**：
   - 创建血条模板、武器 Icon 模板
   - 添加英雄模板（HUD），怪物头顶血条
4. **数据绑定**：绑定委托，实现 UI 进度条更新、武器 Icon 更新
5. **联动验证**：受击时头顶血条即时更新验证通过（委托链路：伤害扣血 → AttributeSet 广播 → UIComponent 委托 → 头顶血条 Widget 刷新）

**追加产出（敌人 AI 基础）**：

1. **AI 控制器**：
   - 创建 `WarriorAIController`（C++）：绑定行为树、设置 TeamId、启用 CrowdPerceptionComponent、添加 AISenseConfig 感知配置等均在 C++ 完成
   - 蓝图层级：`AIC_Enemy_Base`（继承 WarriorAIController）→ `AIC_Guardian`（用于 Guardian 类型敌人）
2. **黑板**：创建 `BB_Enemy_Base`
3. **行为树与 Service**：
   - 创建行为树 `BT_Guardian`
   - 蓝图 Service：`BTService_GetDisToTarget`，计算敌人到目标的距离并存储到黑板
   - C++ BTService：`UBTService_OrientToTargetActor`，改变敌人朝向目标
4. **行为树逻辑**：
   - Selector 节点判断目标距离：小于 600 → 继续执行"朝向目标"；大于 600 → 执行"移动到目标"
   - 判断目标距离的中止条件（Abort）设置为 Both
   - "朝向目标"（Selector）挂载 `BTService_OrientToTargetActor` 执行目标朝向改变

- **遇到的卡点**：-
- **如何解决的**：-
- **关键记录**：-
- **个人总结**：-

---

## 待办 / 下一步计划

- [x] 动画和链接层（Animation Layers / Linked Anim Layers）✅ Day3
- [x] 使用 GAS 作为能力系统（ASC / AttributeSet / GameplayAbility）✅ Day3
- [x] 技能激活策略（Ability Activate Policy）✅ Day3
- [x] 设置武器类别（Weapon Type）✅ Day3
- [x] 英雄武器（Hero Weapon）✅ Day3
- [x] 启动数据和同步加载（Startup Data / Sync Loading）✅ Day3

## 下一步计划

**近战系统**（Day4 方向，规划已确定），包含以下几块：

- [x] 武器装备、装卸装备能力（Equip / Unequip）✅ Day4 完成（装备/卸下链路闭环，可过渡回空手动画）
- [x] 连击系统（Combo）✅ Day4 完成（轻/重连击 + 轻攻击跳终结技）
- [x] 完善链接动画层（Linked Anim Layers）✅ Day4 完成，持械奔跑切换正常
- [x] 轻重攻击和音效（Light / Heavy Attack + Sound）✅ Day4 完成（末段慢动作 AnimNotifyState + AnimNotify 音效）
