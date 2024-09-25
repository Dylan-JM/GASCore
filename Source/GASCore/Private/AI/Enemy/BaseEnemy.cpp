
#include "AI/Enemy/BaseEnemy.h"
#include "AbilitySystem/MyGameplayTags.h"
#include "AbilitySystem/MyAbilitySystemLibrary.h"
#include "AbilitySystem/MyAttributeSet.h"
#include "AbilitySystem/MyAbilitySystemComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerCharacter/BaseCharacter.h"
#include "UI/Widget/MainWidget.h"


ABaseEnemy::ABaseEnemy(const class FObjectInitializer& ObjectInitializer) : ABaseCharacter(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	AbilitySystemComponent = CreateDefaultSubobject<UMyAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSetBase = CreateDefaultSubobject<UMyAttributeSet>("AttributeSet");
	
	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());

	BaseWalkSpeed = 450.f;
}

void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitAbilityActorInfo();

	if (HasAuthority())
	{
		AddCharacterAbilities();
	}
	
	if (UMainWidget* MainHUDWidget = Cast<UMainWidget>(HealthBar->GetUserWidgetObject()))
	{
		MainHUDWidget->SetWidgetController(this);
	}
	
	if (const UMyAttributeSet* HDAS = Cast<UMyAttributeSet>(AttributeSetBase))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HDAS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
		
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HDAS->GetMaxHealthAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);
		
		AbilitySystemComponent->RegisterGameplayTagEvent(GasTag::Effect_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&ABaseEnemy::HitReactTagChanged
		);

		OnHealthChanged.Broadcast(HDAS->GetHealth());
		OnMaxHealthChanged.Broadcast(HDAS->GetMaxHealth());
	}
}

void ABaseEnemy::InitAbilityActorInfo()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		Cast<UMyAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

		if (HasAuthority())
		{
			InitializeDefaultAttributes();
		}
		OnAscRegistered.Broadcast(AbilitySystemComponent);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "ASC invalid");
	}

}

void ABaseEnemy::InitializeDefaultAttributes() const
{
	UMyAbilitySystemLibrary::InitializeDefaultAttributes(this, Level, AbilitySystemComponent);
}

int32 ABaseEnemy::GetPlayerLevel_Implementation()
{
	return Super::GetPlayerLevel_Implementation();
}


