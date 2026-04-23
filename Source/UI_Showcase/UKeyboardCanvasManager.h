// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UKeyboardButtonWidget.h"
#include "Components/TextBlock.h"
#include "UKeyboardCanvasManager.generated.h"

/**
 * 
 */
UCLASS()
class UI_SHOWCASE_API UKeyboardCanvasManager : public UUserWidget
{
	GENERATED_BODY()
	
public:

protected:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextField;
	
	TArray<UKeyboardButtonWidget*> KeyboardButtonsArr;

	void PrintLetter(FString);

	virtual void NativeConstruct() override;
};
