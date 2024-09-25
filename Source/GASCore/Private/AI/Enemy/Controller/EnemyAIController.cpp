
#include "AI/Enemy/Controller/EnemyAIController.h"

#include "Navigation/CrowdFollowingComponent.h"

AEnemyAIController::AEnemyAIController(FObjectInitializer const& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{

}


