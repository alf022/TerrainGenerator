//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Framework/SlateDelegates.h"

class STextBlock;
class SCheckBox;

/** Delegate that is executed when the check box state changes */
DECLARE_DELEGATE_OneParam(FOnIsCheckedChanged, ECheckBoxState);

class STextCheckbox : public SCompoundWidget
{		
public:
	SLATE_BEGIN_ARGS(STextCheckbox) 

		: _Text()
		, _IsChecked(ECheckBoxState::Unchecked)
		, _OnCheckStateChanged()
	{
	}	

	/** The text displayed in this text block */
	SLATE_ATTRIBUTE(FText, Text)

	SLATE_ATTRIBUTE(ECheckBoxState, IsChecked)

	/** Called when the checked state has changed */
	SLATE_EVENT(FOnIsCheckedChanged, OnCheckStateChanged)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	
	ECheckBoxState GetCheckedState() const;

private:	
	TSharedPtr<STextBlock> Text;
	TSharedPtr<SCheckBox> CheckBox;
	
protected:	
	/** Delegate called when the check box changes state */
	FOnIsCheckedChanged OnCheckStateChanged;

	void OnCheckBoxStateChanged(ECheckBoxState NewState);
};