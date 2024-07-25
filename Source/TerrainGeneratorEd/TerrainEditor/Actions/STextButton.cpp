//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "STextButton.h"
#include "SlateOptMacros.h"

#include "Widgets/Input/SCheckBox.h"

#define LOCTEXT_NAMESPACE "STextButton"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STextButton::Construct(const STextButton::FArguments& InArgs)
{
	OnButtonClicked = InArgs._OnButtonClicked;

	ChildSlot
	[
		SNew(SBox)
		.VAlign(EVerticalAlignment::VAlign_Fill)
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		.HeightOverride(40.f)
		//.WidthOverride(400.f)
		[
			SNew(SButton)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.HAlign(EHorizontalAlignment::HAlign_Fill)		
			.OnClicked(this, &STextButton::OnButtonWasClicked)
			[
				SNew(STextBlock)
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 15))
				.Text(InArgs._Text)
				.Justification(ETextJustify::Center)
			]
		]
	];	
}

FReply STextButton::OnButtonWasClicked()
{
	OnButtonClicked.Execute();
	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE