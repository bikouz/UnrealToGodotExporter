#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "GodotExportTypes.h"

class IDetailsView;

class SGodotExportWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SGodotExportWindow) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	FReply OnExportAll();
	FReply OnExportSelected();
	FReply OnExportSelectedFolders();
	FReply OnBrowseGodotProject();
	void ApplyResult(const FGodotExportResult& Result);

	TSharedPtr<IDetailsView> DetailsView;
	TSharedPtr<class SMultiLineEditableTextBox> LogBox;
	FString LogText;
};
