
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndWidget.generated.h"
   
class UTextBlock;
class UButton;

UCLASS()
class GAMEGOCNHINTHU3_API UEndWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
	void UpdateResultText(FText NewText);

private:
	UFUNCTION()
	void PlayAgainClicked();

	UFUNCTION()
	void QuitClicked();

private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ResultText;

	UPROPERTY(meta = (BindWidget))
	UButton* PlayAgainButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;
};
