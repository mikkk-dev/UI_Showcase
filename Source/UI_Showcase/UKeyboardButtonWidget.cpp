// Fill out your copyright notice in the Description page of Project Settings.


#include "UKeyboardButtonWidget.h"
#include "Components/Image.h"


void UKeyboardButtonWidget::NativeConstruct() 
{
	Super::NativeConstruct();

	bWasKeyPressed = false;


	if (BtnImage) {
		BtnImage->SetBrushFromTexture(TextureToApply);
	}
}

void UKeyboardButtonWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (APlayerController* PC = GetOwningPlayer())
	{
		bool bIsKeyPressed = PC->IsInputKeyDown(KeyToCheck);
		
		if (bWasKeyPressed != bIsKeyPressed)
		{
			if (bIsKeyPressed)
			{
				PlayAnimation(GoDown);
			}
			else
			{
				PlayAnimation(GoUp);
			}
		}

		bWasKeyPressed = bIsKeyPressed;
	}
}