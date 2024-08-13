#pragma once

#include "GithubToolsTypes.h"

#include <CoreMinimal.h>

struct FGitHubToolsFilePatchViewListItem : public TSharedFromThis< FGitHubToolsFilePatchViewListItem >
{
public:
    TSharedRef< SWidget > GenerateWidgetForItem();

    static TSharedPtr< FGitHubToolsFilePatchViewListItem > Create( FString string );

protected:
    FGitHubToolsFilePatchViewListItem() = default;
    explicit FGitHubToolsFilePatchViewListItem( FString && string );

private:
    FString String;
};

typedef TSharedPtr< FGitHubToolsFilePatchViewListItem > FGitHubToolsFilePatchViewListItemPtr;

class SGitHubToolsFilePatch final : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsFilePatch )
    {}

    SLATE_ATTRIBUTE( FGithubToolsPullRequestFileInfosPtr, FileInfos )
    SLATE_ARGUMENT( TSharedPtr< SWindow >, ParentWindow )

    SLATE_END_ARGS()

    virtual ~SGitHubToolsFilePatch() override;

    void Construct( const FArguments & arguments );

private:
    TSharedRef< ITableRow > GenerateRowForItem( FGitHubToolsFilePatchViewListItemPtr item, const TSharedRef< STableViewBase > & owner_table ) const;
    void PopulateListItems();

    FGithubToolsPullRequestFileInfosPtr FileInfos;
    TSharedPtr< SListView< FGitHubToolsFilePatchViewListItemPtr > > ListView;
    TArray< FGitHubToolsFilePatchViewListItemPtr > ListItems;

    TWeakPtr< SWindow > WeakParentWindow;
};

#undef LOCTEXT_NAMESPACE