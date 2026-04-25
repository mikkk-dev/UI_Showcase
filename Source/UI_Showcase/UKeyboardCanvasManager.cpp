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

	FString WordsToAdd = GetRandomWord();
	int LetterIndex = 0;
	for (size_t i = 0; i < Count; i++)
	{
		SpaceIndexes.Add(WordsToAdd.Len());
		WordsToAdd += " " + GetRandomWord();
	}

	//UE_LOG(LogTemp, Warning, TEXT("%s"));
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


void UKeyboardCanvasManager::PrintLetter(FString Letter, bool bIsCtrlPressed) 
{
	if (TextField) {
		FString CurrentText = TextField->GetText().ToString()
			.LeftChop(1); // remove the _ from the end
		int TextLength = CurrentText.Len();
		FString NewText;

		if (Letter == TEXT("BACKSPACE"))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), (bIsCtrlPressed ? TEXT("true") : TEXT("false")));
			NewText = CurrentText.LeftChop(1);

			if (CurrentSpaceIndex > 0)
			{
				int32 Diff = TextLength - SpaceIndexes[CurrentSpaceIndex - 1]; // not removing the space

				if (bIsCtrlPressed)
				{
					UE_LOG(LogTemp, Warning, TEXT("%d"), Diff);
					NewText = CurrentText.LeftChop(Diff) + " ";
				}
				else if (NewText.Len() <= SpaceIndexes[CurrentSpaceIndex - 1])
				{
					NewText = CurrentText.LeftChop(Diff) + " ";
					UE_LOG(LogTemp, Warning, TEXT("NewText %d PrevIndex %d"), NewText.Len(), SpaceIndexes[CurrentSpaceIndex - 1]);
				}
			}
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
	}
}