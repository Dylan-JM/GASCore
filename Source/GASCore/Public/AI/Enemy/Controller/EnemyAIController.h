
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class ABaseEnemy;
class UBehaviorTreeComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UAISenseConfig_Damage;
struct FAIStimulus;

UCLASS()
class GASCORE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	explicit AEnemyAIController(FObjectInitializer const& ObjectInitializer);

	

};
