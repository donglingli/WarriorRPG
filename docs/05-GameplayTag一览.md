# 05 - 原生 GameplayTag 一览

> 返回索引：[项目说明.md](./项目说明.md)
> 定义于 [`WarriorGameplayTags.h`](../Source/Warrior/Public/WarriorGameplayTags.h)，实现于 [`WarriorGameplayTags.cpp`](../Source/Warrior/Private/WarriorGameplayTags.cpp)
> 最近更新：2026-11-16（Day5 新增 SetByCaller / HitPause / Enemy Ability / Shared 分类）

| 分类 | Tag | 用途 |
| :--- | :--- | :--- |
| 输入 | `InputTag.Move` / `InputTag.Look` | 原生移动/视角输入 |
| 输入 | `InputTag.EquipAxe` / `InputTag.UnequipAxe` | 装备/卸装备输入 |
| 输入 | `InputTag.LightAttack_Axe` / `InputTag.HeavyAttack_Axe` | 轻重攻击输入 |
| 玩家 | `Player.Weapon.Axe` | 武器 Tag（武器注册键） |
| 玩家 | `Player.Status.JumpToFinsher` | 跳终结技状态 |
| 玩家 SetByCaller | `Player.SetByCaller.AttackType.Light` / `Heavy` | 伤害计算：轻/重攻击连击段数 |
| 玩家技能 | `Player.Ability.Equip.Unequip_Axe` / `Attack_Light / Heavy_Axe` | 技能识别 |
| 玩家技能 | `Player.Ability.HitPause` | 攻击暂停技能 |
| 玩家事件 | `Player.Event.Equip / Unequip_Axe` | AnimNotify → GameplayEvent |
| 玩家事件 | `Player.Event.HitPause` | 命中/离开目标时触发攻击暂停 |
| 敌人 | `Enemy.Weapon` | 敌人武器 Tag |
| 敌人 | `Enemy.Ability.Melee` / `Ranged` | 敌人近战/远程技能预留 |
| 共享 | `Shared.Ability.HitReact` / `Death` | 受击 / 死亡技能（英雄与敌人共用） |
| 共享 | `Shared.Event.MeleeHit` | 近战命中事件（碰撞 → 攻击 GA） |
| 共享 | `Shared.Event.HitReact` | 受击事件 |
| 共享 | `Shared.SetByCaller.BaseDamage` | 伤害计算：基础伤害值 |
| 共享 | `Shared.Status.Death` | 死亡状态（血量归零时由 AttributeSet 打上） |

> 新增 Tag 时同步更新本表。
