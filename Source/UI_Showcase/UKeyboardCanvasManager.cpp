// Fill out your copyright notice in the Description page of Project Settings.


#include "UKeyboardCanvasManager.h"
#include "Components/PanelWidget.h"
#include "Blueprint/WidgetTree.h"


void UKeyboardCanvasManager::NativeConstruct() 
{
	Super::NativeConstruct();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("PrintLetter called!"));
	}

	TextField->SetText(FText::FromString(" "));
	KeyboardButtonsArr.Empty();

	TArray<UWidget*> AllWidgets;
	WidgetTree->GetAllWidgets(AllWidgets);

	for (UWidget* Widget : AllWidgets)
	{
		if (UKeyboardButtonWidget* Button = Cast<UKeyboardButtonWidget>(Widget))
		{
			KeyboardButtonsArr.Add(Button);
		}
	}

	for (auto& Btn : KeyboardButtonsArr)
	{
		Btn->LetterSendDelegate.BindUObject(this, &UKeyboardCanvasManager::PrintLetter);
	}
}


void UKeyboardCanvasManager::PrintLetter(FString letter) 
{
	if (TextField) {
		FString CurrentText = TextField->GetText().ToString();
		int TextLength = CurrentText.Len();
		FString NewText;

		if (letter == TEXT("BACKSPACE"))
		{
			NewText = CurrentText.LeftChop(1);
		}
		else if (letter == TEXT("SHIFT"))
		{
			return;
		}
		else
		{
			NewText = CurrentText + letter;
		}


		TextField->SetText(FText::FromString(NewText));
	}
}