// Fill out your copyright notice in the Description page of Project Settings.


#include "UKeyboardCanvasManager.h"
#include "Components/PanelWidget.h"
#include "Logging/LogMacros.h"
#include "Blueprint/WidgetTree.h"


void UKeyboardCanvasManager::NativeConstruct() 
{
	Super::NativeConstruct();
	SetupButtons();
	LoadWordsFromStruct();
	GenerateWordsToType(50);
}


void UKeyboardCanvasManager::GenerateWordsToType(int32 Count)
{
	TextField->SetText(FText::FromString(""));
	TextFieldGray->SetText(FText::FromString(""));
	SpaceIndexes.Empty();
	CurrentSpaceIndex = 0;
	ExpectedLetterIndex = 0;

	TextToType = GetRandomWord();

	for (size_t i = 0; i < Count; i++)
	{
		SpaceIndexes.Add(TextToType.Len());
		TextToType += " " + GetRandomWord();
	}

	TextFieldGray->SetText(FText::FromString(TextToType));
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


void UKeyboardCanvasManager::PrintLetter(FString Letter, bool bIsCtrlPressed) 
{
	if (TextField) {
		FString CurrentText = TextField->GetText().ToString()
			.LeftChop(1); // remove the _ from the end
		int TextLength = CurrentText.Len();
		FString NewText;
		

		if (Letter == TEXT("BACKSPACE"))
		{
			NewText = CurrentText.LeftChop(1);

			// so that the previos index is not -1
			if (CurrentSpaceIndex > 0)
			{
				int32 Diff = TextLength - SpaceIndexes[CurrentSpaceIndex - 1]; // not removing the space

				// removes the full up to the previous space
				if (bIsCtrlPressed ||  // if ctrl is pressed
					NewText.Len() <= SpaceIndexes[CurrentSpaceIndex - 1]) // if the previous chop went too far
				{
					NewText = CurrentText.LeftChop(Diff) + " ";
				}
			}
			// clears the first word
			else if (bIsCtrlPressed)
			{
				NewText = "";
			}
		}
		else if (Letter == TEXT("SHIFT"))
		{
			return;
		}
		else if (Letter == TEXT(" "))
		{
			CurrentSpaceIndex++;
			NewText = CurrentText + Letter;
			// if the previous word wasnt fully filled, it fills it with spaces
			while (NewText.Len() <= SpaceIndexes[CurrentSpaceIndex -1])
			{
				NewText += Letter;
			}
		}
		else
		{
			if (CurrentText.Len() < SpaceIndexes[CurrentSpaceIndex])
			{
				NewText = CurrentText + Letter;
			}
			else
			{
				NewText = CurrentText;
			}
		}

		TextField->SetText(FText::FromString(NewText + "_"));

		TextLength = TextField->GetText().ToString().Len();
		ExpectedLetterIndex = TextLength -1; // beacuse it adds an underscore
		HighlightExpectedKey(TextToType.GetCharArray()[ExpectedLetterIndex]);
	}
}


void UKeyboardCanvasManager::HighlightExpectedKey(char Character)
{
	TCHAR TCharacter = (TCHAR)Character;
	FString FCharacter(1, &TCharacter);

	for (auto& KeyboardButton : KeyboardButtonsArr)
	{
		KeyboardButton->HighlightIfExpected(FCharacter);
	}
}


