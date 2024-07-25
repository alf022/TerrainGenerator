//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "STerrainEditorActionsTab.h"
#include "SlateOptMacros.h"
#include "TerrainGeneratorEd/TerrainEditor/SBaseTerrainEditor.h"
#include "TerrainGeneratorEd/TerrainEditor/Viewport/STerrainEditorViewport.h"

#include "STextCheckbox.h"
#include "Widgets/Layout/SWrapBox.h"
#include "STextButton.h"
#include "Terrain/Data/TerrainData.h"
#include "Terrain/Subsystems/TerrainGeneratorSubsystem.h"
#include "Terrain/Types/TerrainGeneratorTypes.h"
#include "TerrainGeneratorEd/Settings/TerrainGeneratorEditorSettings.h"

#define LOCTEXT_NAMESPACE "STerrainEditorActionsTab"

#define TerrainDataPath "/TerrainGenerator/Data/DebugTerrainData.DebugTerrainData"
#define SlotsPadding 5.f

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STerrainEditorActionsTab::Construct(const FArguments& InArgs, const TSharedRef<SBaseTerrainEditor>& InTerrainEditor)
{
	TerrainEditor = InTerrainEditor;

	const uint8 MaxEnum = StaticCast<uint8>(ETerrainGen_State::ETGS_MAX);
	for (int32 i = 0; i < MaxEnum; i++)
	{
		StateOptions.Add(MakeShareable(new FText(FText::FromString(GetETerrainGen_StateName(i)))));
	}

	SelectedState = MakeShareable(new FText(*StateOptions[3]));

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(SlotsPadding)
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(.5f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SAssignNew(TextboxUseGrid, STextCheckbox)
					.Text(LOCTEXT("Use Grid", "Use Grid"))
					.IsChecked(ECheckBoxState::Checked)
					.OnCheckStateChanged(this, &STerrainEditorActionsTab::OnCheckBoxStateChangedDraw)
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SAssignNew(TextboxUseGridMesh, STextCheckbox)
					.Text(LOCTEXT("Use Grid Mesh", "Use Grid Mesh"))
					.IsChecked(ECheckBoxState::Checked)
					.OnCheckStateChanged(this, &STerrainEditorActionsTab::OnCheckBoxStateChangedDraw)
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(.5f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SWrapBox)
					//.UseAllottedWidth(true)			
					.FlowDirectionPreference(EFlowDirectionPreference::LeftToRight)
					+ SWrapBox::Slot()
					[
						SAssignNew(TextboxDrawCentralCell, STextCheckbox)
						.Text(LOCTEXT("Show Central Cell", "Show Central Cell"))
						.IsChecked(ECheckBoxState::Checked)
						.OnCheckStateChanged(this, &STerrainEditorActionsTab::OnCheckBoxStateChangedDraw)
					]
					+ SWrapBox::Slot()
					[
						SAssignNew(TextboxDrawInitialCell, STextCheckbox)
						.Text(LOCTEXT("Show Initial Cell", "Show Initial Cell"))
						.IsChecked(ECheckBoxState::Unchecked)
						.OnCheckStateChanged(this, &STerrainEditorActionsTab::OnCheckBoxStateChangedDraw)
					]
					+ SWrapBox::Slot()
					[
						SAssignNew(TextboxDrawInitialRoom, STextCheckbox)
						.Text(LOCTEXT("Show Initial Room", "Show Initial Room"))
						.IsChecked(ECheckBoxState::Checked)
						.OnCheckStateChanged(this, &STerrainEditorActionsTab::OnCheckBoxStateChangedDraw)
					]
					+ SWrapBox::Slot()
					[
						SAssignNew(TextboxDrawRoomBorders, STextCheckbox)
						.Text(LOCTEXT("Show Room Borders", "Show Room Borders"))
						.IsChecked(ECheckBoxState::Unchecked)
						.OnCheckStateChanged(this, &STerrainEditorActionsTab::OnCheckBoxStateChangedDraw)
					]
					+ SWrapBox::Slot()
					[
						SAssignNew(TextboxDrawRoomCollision, STextCheckbox)
						.Text(LOCTEXT("Show Room Collision", "Show Room Collision"))
						.IsChecked(ECheckBoxState::Unchecked)
						.OnCheckStateChanged(this, &STerrainEditorActionsTab::OnCheckBoxStateChangedDraw)
					]
					+ SWrapBox::Slot()
					[
						SAssignNew(TextboxDrawCorridors, STextCheckbox)
						.Text(LOCTEXT("Show Corridors", "Show Corridors"))
						.IsChecked(ECheckBoxState::Checked)
						.OnCheckStateChanged(this, &STerrainEditorActionsTab::OnCheckBoxStateChangedDraw)
					]
					+ SWrapBox::Slot()
					[
						SAssignNew(TextboxDrawWalls, STextCheckbox)
						.Text(LOCTEXT("Show Walls", "Show Walls"))
						.IsChecked(ECheckBoxState::Checked)
						.OnCheckStateChanged(this, &STerrainEditorActionsTab::OnCheckBoxStateChangedDraw)
					]
					+ SWrapBox::Slot()
					[
						SAssignNew(TextboxDrawDoors, STextCheckbox)
						.Text(LOCTEXT("Show Doors", "Show Doors"))
						.IsChecked(ECheckBoxState::Unchecked)
						.OnCheckStateChanged(this, &STerrainEditorActionsTab::OnCheckBoxStateChangedDraw)
					]
					+ SWrapBox::Slot()
					[
						SAssignNew(TextboxDrawCells, STextCheckbox)
						.Text(LOCTEXT("Show Cells", "Show Cells"))
						.IsChecked(ECheckBoxState::Checked)
						.OnCheckStateChanged(this, &STerrainEditorActionsTab::OnCheckBoxStateChangedDraw)
					]					
				]			
			]
		]				
		+ SVerticalBox::Slot()
		.Padding(SlotsPadding)
		.AutoHeight()
		[
			SNew(SBox)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.HAlign(EHorizontalAlignment::HAlign_Fill)
			.HeightOverride(50.f)
			[
				SAssignNew(SeedText, STextBlock)
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 15))
				.Text(LOCTEXT("Seed: -: ", "Seed: -"))
				.Justification(ETextJustify::Center)
			]
		]
		+ SVerticalBox::Slot()
		.Padding(SlotsPadding)
		//.FillHeight(1.f)
		.AutoHeight()
		[		
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(STextButton)
				.Text(LOCTEXT("Generate new", "Generate new"))
				.OnButtonClicked(this, &STerrainEditorActionsTab::GenerateTerrainButtonClicked)
			]			
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.HAlign(EHorizontalAlignment::HAlign_Fill)
			[
				SNew(SBox)
				.VAlign(EVerticalAlignment::VAlign_Fill)
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				.HeightOverride(50.f)
				[		
					SAssignNew(StateComboBox, SComboBox<TSharedPtr<FText>>)				
					.OptionsSource(&StateOptions)
					.InitiallySelectedItem(SelectedState)
					.OnSelectionChanged_Lambda([this](TSharedPtr<FText> NewSelection, ESelectInfo::Type SelectInfo)
					{
						if (!NewSelection.IsValid())
						{
							return;
						}

						SelectedState = NewSelection;
						int32 Index = 0;
						if (this->StateOptions.Find(SelectedState, Index))
						{
							if (!GetTerrainGeneratorSubsystem())
							{
								return;
							}

							GetTerrainGeneratorSubsystem()->EditorGenerateTerrain(TerrainData, StaticCast<ETerrainGen_State>(Index), true);
						}
					})
					.OnGenerateWidget_Lambda([](TSharedPtr<FText> Option)
					{
						return SNew(STextBlock)
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 15))
							.Text(*Option);
					})
					[
						SNew(STextBlock)
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 15))
						.Text_Lambda([this]()
						{										
							return SelectedState.IsValid() ? *SelectedState : FText::FromString("-"); //WallsLayout
						})
					]							
				]
			]
		]			
	];	
		
	LoadTerrainData();
	GenerateTerrainButtonClicked();	
}

void STerrainEditorActionsTab::LoadTerrainData()
{
	// Search for data override in editor settings.
	const UTerrainGeneratorEditorSettings* Settings = GetDefault<UTerrainGeneratorEditorSettings>();
	TerrainData = Settings->EditorTerrainData.LoadSynchronous();

	if(TerrainData)
	{
		return;
	}

	// Default one in case there is no override in settings
	FSoftObjectPath Path = FSoftObjectPath();
	Path.SetPath(FPackageName::ExportTextPathToObjectPath(TEXT(TerrainDataPath)));
	TerrainData = Cast<UTerrainData>(Path.TryLoad());
}

FReply STerrainEditorActionsTab::GenerateTerrainButtonClicked()
{
	if (!GetTerrainGeneratorSubsystem())
	{
		return FReply::Handled();
	}
		
	int32 Index = 0;
	const bool bFound = StateOptions.Find(SelectedState, Index);
	if (bFound && SelectedState.IsValid())
	{		
		GetTerrainGeneratorSubsystem()->EditorGenerateTerrain(TerrainData, StaticCast<ETerrainGen_State>(Index), false);
	}
	else
	{
		GetTerrainGeneratorSubsystem()->EditorGenerateTerrain(TerrainData, ETerrainGen_State::WallsLayout, false);
	}
		
	UpdateSeedText();
	return FReply::Handled();
}

void STerrainEditorActionsTab::UpdateSeedText()
{
	if (!GetTerrainGeneratorSubsystem())
	{
		return;
	}

	const FString textString = FString("Seed: " + FString::FromInt(GetTerrainGeneratorSubsystem()->GetStream().GetInitialSeed()));
	SeedText->SetText(FText::FromString(textString));
}

UTerrainGeneratorSubsystem* STerrainEditorActionsTab::GetTerrainGeneratorSubsystem()
{
	if (!TerrainGeneratorSubsystem)
	{
		TerrainGeneratorSubsystem = GEngine->GetEngineSubsystem<UTerrainGeneratorSubsystem>();
	}

	return TerrainGeneratorSubsystem;
}

void STerrainEditorActionsTab::OnCheckBoxStateChangedDraw(ECheckBoxState NewState) const
{
	if (!TerrainEditor.IsValid())
	{
		return;
	}

	if (!TerrainEditor->GetTerrainEditorViewport())
	{
		return;
	}

	FViewportConfiguration NewViewportConfiguration;
	NewViewportConfiguration.bUseGrid = TextboxUseGrid->GetCheckedState() == ECheckBoxState::Checked;
	NewViewportConfiguration.bUseGridMesh = TextboxUseGridMesh->GetCheckedState() == ECheckBoxState::Checked;
	NewViewportConfiguration.bDrawInitialCell = TextboxDrawInitialCell->GetCheckedState() == ECheckBoxState::Checked;
	NewViewportConfiguration.bDrawCentralCell = TextboxDrawCentralCell->GetCheckedState() == ECheckBoxState::Checked;
	NewViewportConfiguration.bDrawInitialRoom = TextboxDrawInitialRoom->GetCheckedState() == ECheckBoxState::Checked;
	NewViewportConfiguration.bDrawRoomBorders = TextboxDrawRoomBorders->GetCheckedState() == ECheckBoxState::Checked;
	NewViewportConfiguration.bDrawRoomCollision = TextboxDrawRoomCollision->GetCheckedState() == ECheckBoxState::Checked;
	NewViewportConfiguration.bDrawCorridors = TextboxDrawCorridors->GetCheckedState() == ECheckBoxState::Checked;
	NewViewportConfiguration.bDrawWalls = TextboxDrawWalls->GetCheckedState() == ECheckBoxState::Checked;
	NewViewportConfiguration.bDrawDoors = TextboxDrawDoors->GetCheckedState() == ECheckBoxState::Checked;
	NewViewportConfiguration.bDrawCells = TextboxDrawCells->GetCheckedState() == ECheckBoxState::Checked;
	TerrainEditor->GetTerrainEditorViewport()->SetDrawConfiguration(NewViewportConfiguration);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE