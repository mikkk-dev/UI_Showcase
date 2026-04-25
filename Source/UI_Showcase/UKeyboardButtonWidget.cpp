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
		bool bIsShiftPressed = PC->IsInputKeyDown(EKeys::LeftShift) || PC->IsInputKeyDown(EKeys::RightShift);
		bool bIsCtrlPressed = PC->IsInputKeyDown(EKeys::LeftControl) || PC->IsInputKeyDown(EKeys::RightControl);
		
		if (bWasKeyPressed != bIsKeyPressed)
		{
			if (bIsKeyPressed)
			{
				LetterSendDelegate.ExecuteIfBound(bIsShiftPressed ? LetterToSend.ToUpper() : LetterToSend, bIsCtrlPressed);
				PlayAnimation(GoDown);
				PlaySound(SoundToPlay);
			}
			else
			{
				PlayAnimation(GoUp);
			}
		}

		bWasKeyPressed = bIsKeyPressed;
	}
}


void UKeyboardButtonWidget::HighlightIfExpected(FString Key)
{
	if (Key != LetterToSend)
	{
		BtnImage->SetOpacity(0.5f);
	}
	else
	{
		BtnImage->SetOpacity(1.0f);
	}

}

