//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "STextCheckbox.h"
#include "SlateOptMacros.h"

#include "Widgets/Input/SCheckBox.h"

#define LOCTEXT_NAMESPACE "STextCheckbox"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STextCheckbox::Construct(const STextCheckbox::FArguments& InArgs)
{
	OnCheckStateChanged = InArgs._OnCheckStateChanged;

	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		//.FillWidth(1.0f)
		[
			SNew(STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 15))
			.Text(InArgs._Text)
			.Justification(ETextJustify::Left)
		]
		+ SHorizontalBox::Slot()
		//.FillWidth(1.0f)
		.AutoWidth()
		[
			SAssignNew(CheckBox, SCheckBox)
			.IsChecked(InArgs._IsChecked)
			.OnCheckStateChanged(this, &STextCheckbox::OnCheckBoxStateChanged)
		]		
			
	];	
}

ECheckBoxState STextCheckbox::GetCheckedState() const
{
	return CheckBox->GetCheckedState();
}

void STextCheckbox::OnCheckBoxStateChanged(ECheckBoxState NewState)
{
	OnCheckStateChanged.ExecuteIfBound(NewState);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE