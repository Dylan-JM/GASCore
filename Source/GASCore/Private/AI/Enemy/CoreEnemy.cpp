
#include "AI/Enemy/CoreEnemy.h"
#include "AbilitySystem/CoreGameplayTags.h"
#include "AbilitySystem/CoreAbilitySystemLibrary.h"
#include "AbilitySystem/CoreAttributeSet.h"
#include "AbilitySystem/CoreAbilitySystemComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/CoreCharacter.h"
#include "UI/Widget/MainWidget.h"


ACoreEnemy::ACoreEnemy(const class FObjectInitializer& ObjectInitializer) : ACoreCharacter(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	AbilitySystemComponent = CreateDefaultSubobject<UCoreAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSetBase = CreateDefaultSubobject<UCoreAttributeSet>("AttributeSet");
	
	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());

	BaseWalkSpeed = 450.f;
}

void ACoreEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACoreEnemy::BeginPlay()
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
	
	if (const UCoreAttributeSet* HDAS = Cast<UCoreAttributeSet>(AttributeSetBase))
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
			&ACoreEnemy::HitReactTagChanged
		);

		OnHealthChanged.Broadcast(HDAS->GetHealth());
		OnMaxHealthChanged.Broadcast(HDAS->GetMaxHealth());
	}
}

void ACoreEnemy::InitAbilityActorInfo()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		Cast<UCoreAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

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

void ACoreEnemy::InitializeDefaultAttributes() const
{
	UCoreAbilitySystemLibrary::InitializeDefaultAttributes(this, Level, AbilitySystemComponent);
}

int32 ACoreEnemy::GetPlayerLevel_Implementation()
{
	return Super::GetPlayerLevel_Implementation();
}


