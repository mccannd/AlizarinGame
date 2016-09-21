// Interface that works for anything that may be affected by damage
// implemented by characters and destructible objects

#pragma once

#include "DamageableInterface.generated.h"


UINTERFACE(BlueprintType)
class ALIZARINGAME_API UDamageableInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class ALIZARINGAME_API IDamageableInterface
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

public:
	// application of damage and healing effects over time
	// will later be extended to cover status effects?
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage")
		void TickDamage(float deltaSeconds);
	
	// standard damage calculation handling
	// will later replace with USTRUCT for more detailed handling
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage")
		void CalculateDamage(float damage);
};
