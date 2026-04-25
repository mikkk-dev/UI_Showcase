// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UKeyboardButtonWidget.h"
#include "Components/TextBlock.h"
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
	uint32 ExpectedLetterIndex;
	
	TArray<UKeyboardButtonWidget*> KeyboardButtonsArr;

	FTimerHandle CountDownTimerHandle;
	int32 CountDownValue;

	void PrintLetter(FString, bool);
	void SetupButtons();
	void LoadWordsFromStruct();
	void GenerateWordsToType(int32);
	void HighlightExpectedKey(char);
	void UpdateTextFieldRed();
	void RestartTimer();
	void UpdateTimer();

	FString GetRandomWord();

	virtual void NativeConstruct() override;
};
