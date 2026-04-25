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

	HighlightExpectedKey(TextToType[0]);
}


void UKeyboardCanvasManager::GenerateWordsToType(int32 Count)
{
	APlayerController* PlayerController = GetOwningPlayer();
	PlayerController->SetShowMouseCursor(true);

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


void UKeyboardCanvasManager::SetEnabledButtons(bool Enabled)
{
	for (auto& Btn : KeyboardButtonsArr)
	{
		Btn->SetEnabled(Enabled);
	}
}


void UKeyboardCanvasManager::PrintLetter(FString KeyStr, bool bIsCtrlPressed) 
{
	if (!bIsCountDownTimerRunning)
	{
		bIsCountDownTimerRunning = true;
		PlayAnimation(BlurOut);
		RestartTimer(15, 1);
		return;
	}

	if (CountDownValue < 0)
	{
		return;
	}

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
			LettersTyped++;
			CorrectLetters++;
		}
		NewText = CurrentText + KeyStr;
		WordsCount++;
		// if the previous word wasnt fully filled, it fills it with spaces
		while (NewText.Len() <= SpaceIndexes[CurrentSpaceIndex -1])
		{
			NewText += KeyStr;
			LettersTyped++;
		}

		if (NewLineIndexes.Contains(SpaceIndexes[CurrentSpaceIndex -1]))
		{
			NewText += "\n";
		}
	}
	else // any other character
	{
		if (CurrentSpaceIndex >= SpaceIndexes.Num() || // prevent index out of bounds
			CurrentText.Len() >= SpaceIndexes[CurrentSpaceIndex]) // prevent typing letters than the current word 
		{
			NewText = CurrentText;
		}
		else
		{
			LettersTyped++;
			// czy zgadza sie z tekstem do napisania
			if (TextToType[TextLength] == KeyStr[0])
			{
				NewText = CurrentText + KeyStr;
				CorrectLetters++;
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
	if (ExpectedLetterIndex >= TextToType.Len())
	{
		return;
	}
	HighlightExpectedKey(TextToType.GetCharArray()[ExpectedLetterIndex]);
		
	UpdateTextFieldRed();
	UpdateUIValues();
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
	TyposCount = 0;

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
				TyposCount++;
			}
		}
	}
	TextFieldRed->SetText(FText::FromString(TextToApply));
}


void UKeyboardCanvasManager::RestartTimer(int32 FullTime, int32 Delay)
{
	CountDownValue = FullTime;
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(CountDownTimerHandle, this, &UKeyboardCanvasManager::UpdateTimer, 1, true, Delay);
}


void UKeyboardCanvasManager::UpdateTimer()
{
	CountDownTextBlock->SetText(FText::FromString(FString::FromInt(CountDownValue)));
	CountDownValue--;

	if (CountDownValue < 0) 
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearTimer(CountDownTimerHandle);
		SetEnabledButtons(false);
		UE_LOG(LogTemp, Warning, TEXT("Hello World"));
	}
}


void UKeyboardCanvasManager::UpdateUIValues()
{
	WordsTextBlock->SetText(FText::FromString(FString::FromInt(WordsCount)));
	TyposTextBlock->SetText(FText::FromString(FString::FromInt(TyposCount)));

	int32 TimePassed = 15 - CountDownValue;
	if (TimePassed > 0)
	{
		WPMValue = (WordsCount / float(TimePassed)) * 60;
	}
	WPMTextBlock->SetText(FText::FromString(FString::FromInt(WPMValue)));
	
	if (LettersTyped > 0)
	{
		AccuracyValue = (CorrectLetters / float(LettersTyped)) * 100;
	}
	AccuracyTextBlock->SetText(FText::FromString(FString::FromInt(AccuracyValue) + "%"));
}

