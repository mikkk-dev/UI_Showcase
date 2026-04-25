// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UKeyboardButtonWidget.h"
#include "Components/TextBlock.h"
#include "Components/BackgroundBlur.h"
#include "Components/Border.h"
#include "UKeyboardCanvasManager.generated.h"

/**
 * 
 */
UCLASS()
class UI_SHOWCASE_API UKeyboardCanvasManager : public UUserWidget
{
	GENERATED_BODY()
	
public:

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextField;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextFieldGray;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextFieldRed;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CountDownTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WordsTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TyposTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WPMTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AccuracyTextBlock;

	UPROPERTY(meta = (BindWidget))
	UBackgroundBlur* BlurPanel;

	UPROPERTY(meta = (BindWidget))
	UBorder* WPMHoverArea;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> BlurOut;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup Values")
	UDataTable* WordsStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup Values")
	int32 NewLineThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup Values")
	int32 WordsToTypeCount;


	TArray<FString> AvailableWords;

	TArray<int32> SpaceIndexes;
	int32 CurrentSpaceIndex;
	TArray<int32> NewLineIndexes;

	float LetterWidth;
	
	FString TextToType;
	int32 ExpectedLetterIndex;
	
	TArray<UKeyboardButtonWidget*> KeyboardButtonsArr;

	FTimerHandle CountDownTimerHandle;
	int32 CountDownValue;
	bool bIsCountDownTimerRunning = false;

	int32 WordsCount;
	int32 TyposCount;
	int32 WPMValue;
	int32 AccuracyValue;

	int32 LettersTyped;
	int32 CorrectLetters;

	void PrintLetter(FString, bool);
	void SetupButtons();
	void SetEnabledButtons(bool);
	void LoadWordsFromStruct();
	void GenerateWordsToType(int32);
	void HighlightExpectedKey(char);
	void UpdateTextFieldRed();
	void RestartTimer(int32, int32);
	void UpdateTimer();
	void UpdateUIValues();

	FString GetRandomWord();

	virtual void NativeConstruct() override;
};
