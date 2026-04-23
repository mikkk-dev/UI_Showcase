// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "UKeyboardButtonWidget.generated.h"

/**
 * 
 */

DECLARE_DELEGATE_OneParam(FLetterSendDelegate, FString);


UCLASS()
class UI_SHOWCASE_API UKeyboardButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FLetterSendDelegate LetterSendDelegate;


protected:
	bool bWasKeyPressed;

	UPROPERTY(meta = (BindWidget))
	UImage* BtnImage;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> GoUp;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> GoDown;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup Values")
	UTexture2D* TextureToApply;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup Values")
	FKey KeyToCheck;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup Values")
	USoundBase* SoundToPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup Values")
	FString LetterToSend;




	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;
};
