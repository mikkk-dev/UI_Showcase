// Fill out your copyright notice in the Description page of Project Settings.


#include "UKeyboardButtonWidget.h"
#include "Components/Image.h"


void UKeyboardButtonWidget::NativeConstruct() 
{
	Super::NativeConstruct();

	bWasKeyPressed = false;
	bIsEnabled = true;

	if (BtnImage) {
		BtnImage->SetBrushFromTexture(TextureToApply);
	}
}

void UKeyboardButtonWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bIsEnabled)
	{
		return;
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		bool bIsKeyPressed = PC->IsInputKeyDown(KeyToCheck);
		bool bIsShiftPressed = PC->IsInputKeyDown(EKeys::LeftShift) || PC->IsInputKeyDown(EKeys::RightShift);
		bool bIsCtrlPressed = PC->IsInputKeyDown(EKeys::LeftControl) || PC->IsInputKeyDown(EKeys::RightControl); // checking for this because of Ctrl Delete functionality
		
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
		BtnImage->SetColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f, 0.5f));
	}
	else
	{
		BtnImage->SetColorAndOpacity(FLinearColor(0.333f, 0.177f, 1.0f, 1.0f));
	}

}


void UKeyboardButtonWidget::SetEnabled(bool Enabled)
{
	bIsEnabled = Enabled;

}

