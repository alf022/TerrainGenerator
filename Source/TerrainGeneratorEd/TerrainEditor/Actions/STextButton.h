//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Framework/SlateDelegates.h"

class STextBlock;
class SButton;

class STextButton : public SCompoundWidget
{		
public:
	SLATE_BEGIN_ARGS(STextButton) 

		: _Text()		
		, _OnButtonClicked()
	{
	}	

	/** The text displayed in this text block */
	SLATE_ATTRIBUTE(FText, Text)

	/** Called when the button is clicked */
	SLATE_EVENT(FOnClicked, OnButtonClicked)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	
private:	
	TSharedPtr<STextBlock> Text;
	
protected:	
	/** Delegate called when the check box changes state */
	FOnClicked OnButtonClicked;

	FReply OnButtonWasClicked();
};