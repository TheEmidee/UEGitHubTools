#pragma once

#include <CoreMinimal.h>

struct FGithubToolsPullRequestInfos;

enum class EGitHubToolsPullRequestReviewEvent : uint8
{
    Approve,
    Comment,
    Dismiss,
    RequestChanges
};

enum class EGitHubToolsPullRequestReviewState : uint8
{
    Approved,
    ChangesRequested,
    Commented,
    Dismissed,
    Pending,
    Unknown
};

enum class EGitHubToolsFileChangedState : uint8
{
    Unknown,
    Added,
    Modified,
    Removed,
    Renamed
};

enum class EGitHubToolsFileViewedState : uint8
{
    Unknown,
    Viewed,
    UnViewed,
    Dismissed
};

enum class EGitHubToolsCommitStatusState : uint8
{
    Error,
    Failure,
    Expected,
    Pending,
    Success,
    Unknown
};

enum class EGitHubToolsPullRequestsState : uint8
{
    Closed,
    Merged,
    Open,
    Unknown
};

struct FGithubToolsPullRequestComment
{
    FGithubToolsPullRequestComment() = default;
    explicit FGithubToolsPullRequestComment( const TSharedRef< FJsonObject > & json_object );

    FString Id;
    FText Author;
    FText Date;
    FText Comment;
    FString Path;
};

typedef TSharedPtr< FGithubToolsPullRequestComment > FGithubToolsPullRequestCommentPtr;

struct FGithubToolsPullRequestFilePatch
{
    FGithubToolsPullRequestFilePatch() = default;
    explicit FGithubToolsPullRequestFilePatch( const FString & file_name, const FString & patch );

    FString FileName;
    FString Patch;
};

typedef TSharedPtr< FGithubToolsPullRequestFilePatch > FGithubToolsPullRequestFilePatchPtr;

struct FGithubToolsPullRequestFileInfos
{
    FGithubToolsPullRequestFileInfos() = default;
    FGithubToolsPullRequestFileInfos( const FString & path, const FString & change_type, const FString & viewed_state );

    void UpdateViewedState( EGitHubToolsFileViewedState new_viewed_state );
    bool IsUAsset() const;

    FString Path;
    FText AssetName;
    FText PackageName;
    EGitHubToolsFileChangedState ChangedState;
    const FSlateBrush * ChangedStateBrush;
    FText ChangedStateToolTip;
    EGitHubToolsFileViewedState ViewedState;
    const FSlateBrush * ViewedStateBrush;
    FText ViewedStateToolTip;
    bool bHasUnresolvedConversations;
    FString Patch;
    TSharedPtr< FGithubToolsPullRequestInfos > PRInfos;
};

typedef TSharedPtr< FGithubToolsPullRequestFileInfos > FGithubToolsPullRequestFileInfosPtr;

enum class EGitHubToolsReviewState : uint8
{
    ChangesRequested,
    Comment,
    Unknown
};

struct FGithubToolsPullRequestReviewThreadInfos
{
    FGithubToolsPullRequestReviewThreadInfos() = default;
    explicit FGithubToolsPullRequestReviewThreadInfos( const TSharedRef< FJsonObject > & json_object );

    FString Id;
    bool bIsResolved;
    FString ResolvedByUserName;
    FString FileName;
    TArray< FGithubToolsPullRequestCommentPtr > Comments;
    int PRNumber;
};

typedef TSharedPtr< FGithubToolsPullRequestReviewThreadInfos > FGithubToolsPullRequestReviewThreadInfosPtr;

struct FGithubToolsPullRequestPendingReviewInfos
{
    FGithubToolsPullRequestPendingReviewInfos() = default;

    FString Id;
    TArray< FGithubToolsPullRequestCommentPtr > Comments;
};

typedef TSharedPtr< FGithubToolsPullRequestPendingReviewInfos > FGithubToolsPullRequestPendingReviewInfosPtr;

struct FGitHubToolsPullRequestCheckInfos
{
    FGitHubToolsPullRequestCheckInfos() = default;
    explicit FGitHubToolsPullRequestCheckInfos( const TSharedRef< FJsonObject > & json );

    FString Context;
    FString StateStr;
    FString Description;
    EGitHubToolsCommitStatusState State;
};

typedef TSharedPtr< FGitHubToolsPullRequestCheckInfos > FGitHubToolsPullRequestCheckInfosPtr;

struct FGithubToolsPullRequestInfos : TSharedFromThis< FGithubToolsPullRequestInfos >
{
    FGithubToolsPullRequestInfos() = default;
    explicit FGithubToolsPullRequestInfos( const TSharedRef< FJsonObject > & json );

    bool CanCommentFiles() const;
    bool HasPendingReviews() const;
    void SetFiles( const TArray< FGithubToolsPullRequestFileInfosPtr > & files, const TArray< FGithubToolsPullRequestFilePatchPtr > & patches );

    FString ViewerLogin;
    int Number;
    FString Id;
    FString Title;
    FText Author;
    FString BaseRefName;
    FString Body;
    int ChangedFiles;
    int CommitCount;
    FString CreatedAt;
    FString HeadRefName;
    bool bIsDraft;
    bool bIsMergeable;
    EGitHubToolsPullRequestsState State;
    FString URL;
    bool bApprovedByMe;
    bool bHasUnresolvedConversations;
    TArray< FGithubToolsPullRequestFileInfosPtr > FileInfos;
    TArray< FGithubToolsPullRequestReviewThreadInfosPtr > Reviews;
    TArray< FGitHubToolsPullRequestCheckInfosPtr > Checks;
    TArray< FGithubToolsPullRequestPendingReviewInfosPtr > PendingReviews;
};

FORCEINLINE bool FGithubToolsPullRequestInfos::HasPendingReviews() const
{
    return !PendingReviews.IsEmpty();
}

typedef TSharedPtr< FGithubToolsPullRequestInfos > FGithubToolsPullRequestInfosPtr;

struct FGitHubToolsFileInfosTreeItem
{
    explicit FGitHubToolsFileInfosTreeItem( const FString & path ) :
        Path( path )
    {}

    FString Path;
    FGithubToolsPullRequestFileInfosPtr FileInfos;
    TArray< TSharedPtr< FGitHubToolsFileInfosTreeItem > > Children;
};

typedef TSharedPtr< FGitHubToolsFileInfosTreeItem > FGitHubToolsFileInfosTreeItemPtr;