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
	GenerateWordsToType(WordsToTypeCount);
	RestartTimer();

	HighlightExpectedKey(TextToType[0]);
}


void UKeyboardCanvasManager::GenerateWordsToType(int32 Count)
{
	TextField->SetText(FText::FromString(""));
	TextFieldGray->SetText(FText::FromString(""));
	TextFieldRed->SetText(FText::FromString(""));

	SpaceIndexes.Empty();
	NewLineIndexes.Empty();
	CurrentSpaceIndex = 0;
	ExpectedLetterIndex = 0;

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
		}
		else
		{
			TextToType += " " + RandomWord;
			LineWidthCounter += RandomWord.Len() + 1; // +1 because of the space
		}

		SpaceIndexes.Add(CurrentTextLength); 
	}
	SpaceIndexes.Add(TextToType.Len());

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
	FString CurrentText = TextField->GetText().ToString()
		.LeftChop(1); // remove the _ from the end
	int TextLength = CurrentText.Len();
	FString NewText;

	if (TextLength >= TextToType.Len())
	{
		return;
	}
		

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

		if (CurrentSpaceIndex < SpaceIndexes.Num())
		{
			CurrentSpaceIndex++;
		}
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
			if (TextToType[TextLength] == KeyStr[0])
			{
				NewText = CurrentText + KeyStr;
			}
			else
			{
				NewText = CurrentText + " ";
			}
		}
	}

	TextField->SetText(FText::FromString(NewText + "_"));

	TextLength = TextField->GetText().ToString().Len();
	ExpectedLetterIndex = TextLength -1; // beacuse it adds an underscore
	HighlightExpectedKey(TextToType.GetCharArray()[ExpectedLetterIndex]);
		
	UpdateTextFieldRed();
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


void UKeyboardCanvasManager::UpdateTextFieldRed()
{
	FString TypedString = TextField->GetText().ToString();
	TArray<TCHAR> TypedChars = TypedString.GetCharArray();

	FString TextToApply = "";
	for (size_t i = 0; i < TypedChars.Num() - 2; i++) // minus the underscore and a blank
	{
		if (TypedChars[i] == '\n') {
			TextToApply += "\n";
		}
		else 
		{
			if (TypedChars[i] == TextToType[i])
			{
				TextToApply += " ";
			}
			else
			{
				TextToApply += TextToType[i];
			}
		}
	}
	TextFieldRed->SetText(FText::FromString(TextToApply));
}


void UKeyboardCanvasManager::RestartTimer()
{
	CountDownValue = 30;
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(CountDownTimerHandle, this, &UKeyboardCanvasManager::UpdateTimer, 1, true, 0);
}


void UKeyboardCanvasManager::UpdateTimer()
{
	CountDownTextBlock->SetText(FText::FromString(FString::FromInt(CountDownValue)));
	CountDownValue--;

	if (CountDownValue < 0) {

		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearTimer(CountDownTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("Hello World"));
	}
}

