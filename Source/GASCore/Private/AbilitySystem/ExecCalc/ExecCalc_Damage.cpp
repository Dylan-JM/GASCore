// Copyright (c) 2025 Dylan Marley, Dylan-JM @ GitHub


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Data/MyAbilityTypes.h"
#include "AbilitySystem/CoreGameplayTags.h"
#include "AbilitySystem/CoreAbilitySystemLibrary.h"
#include "AbilitySystem/CoreAttributeSet.h"
#include "Interface/CombatInterface.h"
#include "Kismet/GameplayStatics.h"

struct HDDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);


	
	HDDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCoreAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCoreAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCoreAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCoreAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCoreAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCoreAttributeSet, CriticalHitDamage, Source, false);

		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCoreAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCoreAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCoreAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCoreAttributeSet, PhysicalResistance, Target, false);
		
	}
};

static const HDDamageStatics& DamageStatics()
{
	static HDDamageStatics DStatics;
	return DStatics;
}


UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);

	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
}


void UExecCalc_Damage::DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FGameplayEffectSpec& Spec, FAggregatorEvaluateParameters EvaluationParameters,
						 const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& InTagsToDefs) const
{
	
	for (TTuple<FGameplayTag, FGameplayTag> Pair : GasTag::DamageTypesToDebuffs)
	{
		const FGameplayTag& DamageType = Pair.Key;
		const FGameplayTag& DebuffType = Pair.Value;
		const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageType, false, -1.f);
		if (TypeDamage > -.5f) // .5 padding for floating point [im]precision
		{
			// Determine if there was a successful debuff
			const float SourceDebuffChance = Spec.GetSetByCallerMagnitude(GasTag::Debuff_Chance, false, -1.f);

			float TargetDebuffResistance = 0.f;
			const FGameplayTag& ResistanceTag = GasTag::DamageTypesToResistances[DamageType];
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(InTagsToDefs[ResistanceTag], EvaluationParameters, TargetDebuffResistance);
			TargetDebuffResistance = FMath::Max<float>(TargetDebuffResistance, 0.f);
			const float EffectiveDebuffChance = SourceDebuffChance * ( 100 - TargetDebuffResistance) / 100.f;
			const bool bDebuff = FMath::RandRange(1,100) < EffectiveDebuffChance;
			if (bDebuff)
			{
				FGameplayEffectContextHandle ContextHandle = Spec.GetContext();
				UCoreAbilitySystemLibrary::SetIsSuccessfulDebuff(ContextHandle, true);
				UCoreAbilitySystemLibrary::SetDamageType(ContextHandle, DamageType);
			
				const float DebuffDamage = Spec.GetSetByCallerMagnitude(GasTag::Debuff_Damage, false, -1.f);
				const float DebuffDuration = Spec.GetSetByCallerMagnitude(GasTag::Debuff_Duration, false, -1.f);
				const float DebuffFrequency = Spec.GetSetByCallerMagnitude(GasTag::Debuff_Frequency, false, -1.f);
				UCoreAbilitySystemLibrary::SetDebuffDamage(ContextHandle, DebuffDamage);
				UCoreAbilitySystemLibrary::SetDebuffDuration(ContextHandle, DebuffDuration);
				UCoreAbilitySystemLibrary::SetDebuffFrequency(ContextHandle, DebuffFrequency);
			}
		}
	}
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;

	TagsToCaptureDefs.Add(GasTag::Attributes_Secondary_Armor, DamageStatics().ArmorDef);
	TagsToCaptureDefs.Add(GasTag::Attributes_Secondary_BlockChance, DamageStatics().BlockChanceDef);
	TagsToCaptureDefs.Add(GasTag::Attributes_Secondary_ArmorPenetration, DamageStatics().ArmorPenetrationDef);
	TagsToCaptureDefs.Add(GasTag::Attributes_Secondary_CriticalHitChance, DamageStatics().CriticalHitChanceDef);
	TagsToCaptureDefs.Add(GasTag::Attributes_Secondary_CriticalHitResistance, DamageStatics().CriticalHitResistanceDef);
	TagsToCaptureDefs.Add(GasTag::Attributes_Secondary_CriticalHitDamage, DamageStatics().CriticalHitDamageDef);

	TagsToCaptureDefs.Add(GasTag::Attributes_Resistance_Arcane, DamageStatics().ArcaneResistanceDef);
	TagsToCaptureDefs.Add(GasTag::Attributes_Resistance_Fire, DamageStatics().FireResistanceDef);
	TagsToCaptureDefs.Add(GasTag::Attributes_Resistance_Lightning, DamageStatics().LightningResistanceDef);
	TagsToCaptureDefs.Add(GasTag::Attributes_Resistance_Physical, DamageStatics().PhysicalResistanceDef);
	
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = SourceASC ? TargetASC->GetAvatarActor() : nullptr;

	int32 SourcePlayerLevel = 1;
	if (SourceAvatar->Implements<UCombatInterface>())
	{
		SourcePlayerLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	}
	
	int32 TargetPlayerLevel = 1;
	if (TargetAvatar->Implements<UCombatInterface>())
	{
		TargetPlayerLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);
	}
	

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// Debuff

	DetermineDebuff(ExecutionParams, Spec, EvaluationParameters, TagsToCaptureDefs);

	// Get damage set by caller magnitude
	float Damage = 0.f;
	for (const TTuple<FGameplayTag, FGameplayTag>& Pair  : GasTag::DamageTypesToResistances)
	{
		const FGameplayTag DamageTypeTag = Pair.Key;
		const FGameplayTag ResistanceTag = Pair.Value;
		
		checkf(TagsToCaptureDefs.Contains(ResistanceTag), TEXT("TagsToCaptureDefs doesn't contain Tag: [%s] in ExecCalc_Damage"), *ResistanceTag.ToString());
		const FGameplayEffectAttributeCaptureDefinition CaptureDef = TagsToCaptureDefs[ResistanceTag];

		float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key, false);
		if (DamageTypeValue <= 0.f)
		{
			continue;
		}
		
		float Resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, Resistance);
		Resistance = FMath::Clamp(Resistance, 0.f,100.f);

		DamageTypeValue *= (100.f - Resistance) / 100.f;
		
		if (UCoreAbilitySystemLibrary::IsRadialDamage(EffectContextHandle))
		{
			if(ICombatInterface* CombatInterface = Cast<ICombatInterface>(TargetAvatar))
			{
				CombatInterface->GetOnDamageSignature().AddLambda([&](float DamageAmount)
				{
					DamageTypeValue = DamageAmount;
				});
			}
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				TargetAvatar,
				DamageTypeValue,
				0.f,
				UCoreAbilitySystemLibrary::GetRadialDamageOrigin(EffectContextHandle),
				UCoreAbilitySystemLibrary::GetRadialDamageInnerRadius(EffectContextHandle),
				UCoreAbilitySystemLibrary::GetRadialDamageOuterRadius(EffectContextHandle),
				1.f,
				UDamageType::StaticClass(),
				TArray<AActor*>(),
				SourceAvatar,
				nullptr);
		}
		
		Damage += DamageTypeValue;

		
	}

	// Capture BlockChance on Target, and determine if there was a successful Block
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);

	
	bool bBlocked;
	if (UCoreAbilitySystemLibrary::IsBlockable(EffectContextHandle))
	{
		 bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
	}
	else
	{
		bBlocked = 0;
	}
	
	UCoreAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);

	float DamageConsumption = 2.f;
	
	// If Block, halve the damage
	Damage = bBlocked ? Damage / DamageConsumption : Damage;
	
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor, 0.f);


	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.f);

	const UCurveTable* SourceDamageCalcCoefficients = UCoreAbilitySystemLibrary::GetDamageCalculationCoefficients(SourceAvatar);
	const UCurveTable* TargetDamageCalcCoefficients = UCoreAbilitySystemLibrary::GetDamageCalculationCoefficients(TargetAvatar);
	const FRealCurve* ArmorPenetrationCurve = SourceDamageCalcCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourcePlayerLevel);

	const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;
	const FRealCurve* EffectiveArmorCurve = TargetDamageCalcCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetPlayerLevel);
	
	// Armor ignores a percentage of incoming damage.
	Damage *= ( 100 - EffectiveArmor * EffectiveArmorCoefficient ) / 100.f;

	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);

	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance, 0.f);

	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.f);
	
	const FRealCurve* CriticalHitResistanceCurve = TargetDamageCalcCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetPlayerLevel);

	// Critical Hit Resistance reduces Critical Hit Chance by a certain percentage
	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
	const bool bCriticalHit = FMath::RandRange(1,100) < EffectiveCriticalHitChance;

	UCoreAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);
	
	// Double damage plus a bonus if critical hit
	Damage = bCriticalHit ? 2.f * Damage + SourceCriticalHitDamage : Damage;
	
	const FGameplayModifierEvaluatedData EvaluatedData(UCoreAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
