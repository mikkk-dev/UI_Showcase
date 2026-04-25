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
	CalculateLetterWidth();
}


void UKeyboardCanvasManager::GenerateWordsToType(int32 Count)
{
	TextField->SetText(FText::FromString(""));
	TextFieldGray->SetText(FText::FromString(""));
	SpaceIndexes.Empty();
	NewLineIndexes.Empty();
	CurrentSpaceIndex = 0;
	ExpectedLetterIndex = 0;
	int32 NewLineThreshold = 60;
	bool bIsFirstLine = true;

	TextToType = GetRandomWord();
	int32 LineWidthCounter = TextToType.Len();

	for (size_t i = 0; i < Count; i++)
	{
		int32 CurrentTextLength = TextToType.Len();
		FString RandomWord = GetRandomWord();

		if (LineWidthCounter + RandomWord.Len() + 1 > NewLineThreshold) // the amount of letters that fit in a line of text, +1 because of the space
		{
			TextToType += " \n" + RandomWord;
			LineWidthCounter = RandomWord.Len();
			NewLineIndexes.Add(CurrentTextLength);
			bIsFirstLine = false;
		}
		else
		{
			TextToType += " " + RandomWord;
			LineWidthCounter += RandomWord.Len() + 1; // +1 because of the space
		}

		SpaceIndexes.Add(bIsFirstLine ? CurrentTextLength : CurrentTextLength); 
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


void UKeyboardCanvasManager::PrintLetter(FString KeyStr, bool bIsCtrlPressed) 
{
	if (TextField) {
		FString CurrentText = TextField->GetText().ToString()
			.LeftChop(1); // remove the _ from the end
		int TextLength = CurrentText.Len();
		FString NewText;
		

		if (KeyStr == TEXT("BACKSPACE"))
		{
			// prevent goind to previous tab
			if (CurrentText.EndsWith("\n"))
			{
				return;
			}

			NewText = CurrentText.LeftChop(1);

			// so that the previos index is not -1
			if (CurrentSpaceIndex > 0)
			{
				int32 Diff = TextLength - SpaceIndexes[CurrentSpaceIndex - 1]; // not removing the space

				// removes the full up to the previous space
				if (bIsCtrlPressed ||  // if ctrl is pressed
					NewText.Len() <= SpaceIndexes[CurrentSpaceIndex - 1]) // if the previous chop went too far
				{
					// checks if the text being chopped has a \n, if so it adds back the new line
					if (CurrentText.RightChop(TextLength - Diff).Contains("\n"))
					{
						NewText = CurrentText.LeftChop(Diff) + " \n";
					}
					else
					{
						NewText = CurrentText.LeftChop(Diff) + " ";
					}
				}
			}
			// clears the first word
			else if (bIsCtrlPressed)
			{
				NewText = "";
			}
		}
		else if (KeyStr == TEXT("SHIFT"))
		{
			return;
		}
		else if (KeyStr == TEXT(" "))
		{

			CurrentSpaceIndex++;
			NewText = CurrentText + KeyStr;
			// if the previous word wasnt fully filled, it fills it with spaces
			while (NewText.Len() <= SpaceIndexes[CurrentSpaceIndex -1])
			{
				NewText += KeyStr;
			}

			if (NewLineIndexes.Contains(SpaceIndexes[CurrentSpaceIndex -1]))
			{
				NewText += "\n";
			}
		}
		else
		{
			if (CurrentSpaceIndex >= SpaceIndexes.Num() || // prevent index out of bounds
				CurrentText.Len() >= SpaceIndexes[CurrentSpaceIndex]) // prevent typing letters than the current word 
			{
				NewText = CurrentText;
			}
			else
			{
				NewText = CurrentText + KeyStr;
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

	UE_LOG(LogTemp, Warning, TEXT("letter %s"), *FCharacter);

	for (auto& KeyboardButton : KeyboardButtonsArr)
	{
		KeyboardButton->HighlightIfExpected(FCharacter);
	}
}

// measure letter width so that i can shift the textfield by that amount
void UKeyboardCanvasManager::CalculateLetterWidth()
{
	LetterWidth = 20;
	//FSlateFontInfo FontInfo = TextField->GetFont();
	//TSharedRef<FSlateFontMeasure> FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	//LetterWidth = FontMeasureService->Measure(TEXT("A"), FontInfo).X;
}


