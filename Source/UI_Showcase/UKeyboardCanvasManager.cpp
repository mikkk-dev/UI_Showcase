// Fill out your copyright notice in the Description page of Project Settings.


#include "UKeyboardCanvasManager.h"
#include "Components/PanelWidget.h"
#include "Blueprint/WidgetTree.h"


void UKeyboardCanvasManager::NativeConstruct() 
{
	Super::NativeConstruct();
	SetupButtons();
	LoadWordsFromStruct();

	TextField->SetText(FText::FromString(""));
	TextFieldGray->SetText(FText::FromString(""));

	FString WordsToAdd = GetRandomWord();
	for (size_t i = 0; i < 50; i++)
	{
		WordsToAdd += " " + GetRandomWord();
	}
	
	TextFieldGray->SetText(FText::FromString(WordsToAdd));
}


FString UKeyboardCanvasManager::GetRandomWord()
{
	if (AvailableWords.Num() > 0)
	{
		return AvailableWords[FMath::RandRange(0, AvailableWords.Num() - 1)];
	}

	return "";
}


void UKeyboardCanvasManager::LoadWordsFromStruct()
{
	if (WordsStruct)
	{
		// Unreal when importing a CSV changes the Property Name to make it unique
		// This gets that property's name in the actual struct
		FName RealPropertyName;
		const UScriptStruct* RowStruct = WordsStruct->GetRowStruct();
		if (RowStruct)
		{
			for (TFieldIterator<FProperty> PropIt(RowStruct); PropIt; ++PropIt)
			{
				FProperty* Property = *PropIt;
				RealPropertyName = *Property->GetName();
			}
		}

		// This adds all of the values to AvailableWords 
		TArray<FName> RowNames = WordsStruct->GetRowNames();
		FProperty* WordProp = WordsStruct->GetRowStruct()->FindPropertyByName(RealPropertyName);
		if (WordProp && WordProp->IsA<FStrProperty>())
		{
			for (const FName& RowName : RowNames)
			{
				uint8* RowPtr = WordsStruct->FindRowUnchecked(RowName);
				if (RowPtr)
				{
					FString WordValue = CastFieldChecked<FStrProperty>(WordProp)->GetOptionalPropertyValue_InContainer(RowPtr);
					AvailableWords.Add(WordValue);
				}
			}
		}
	}
}


void UKeyboardCanvasManager::SetupButtons() 
{
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