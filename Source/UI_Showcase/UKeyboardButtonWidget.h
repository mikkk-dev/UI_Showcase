// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "UKeyboardButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class UI_SHOWCASE_API UKeyboardButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:


protected:
	bool bWasKeyPressed;

	UPROPERTY(meta = (BindWidget))
	UImage* BtnImage;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> GoUp;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> GoDown;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Values")
	UTexture2D* TextureToApply;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Values")
	FKey KeyToCheck;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Values")
	USoundBase* SoundToPlay;




	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;
};
