// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/StartUpData/DataAsset_EnemyStartUpData.h"
#include "AbilitySystem/Abilities/WarriorEnemyGameplayAbility.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"

void UDataAsset_EnemyStartUpData::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InWarriorASCToGive, int32 ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(InWarriorASCToGive, ApplyLevel);

	if (EnemyCombatAbilities.IsEmpty()) return;

	for (const TSubclassOf<UWarriorEnemyGameplayAbility>& AbilityClass : EnemyCombatAbilities)
	{
		if (AbilityClass)
		{
			FGameplayAbilitySpec AbilitySpec(AbilityClass);
			AbilitySpec.SourceObject = InWarriorASCToGive->GetAvatarActor();
			AbilitySpec.Level = ApplyLevel;

			InWarriorASCToGive->GiveAbility(AbilitySpec);
		}
	}
}
